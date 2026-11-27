#include "blank/moje.h"
#include "mqtt_pub.h"
#include "loc.h"
#include "scheduler.h"
#include "core.h"
#include "ui_wpis.h"

#ifdef _MQTT_
#include <MQTTClient.h>
#endif


/* ================================================================ */
/* Jedno polaczenie MQTT na caly proces                             */
/* ================================================================ */
#ifdef _MQTT_
static MQTTClient mqtt_client = NULL;
#endif

/* ================================================================ */
/* Persystencja konfiguracji: ~/.stud_sys_mqtt.cfg                  */
/* ================================================================ */
static const char *mqtt_sciezka(void)
{
    static char path[256];
    const char *home = getenv("HOME");
    if (home) snprintf(path, sizeof(path), "%s/.stud_sys_mqtt.cfg", home);
    else       snprintf(path, sizeof(path), "./mqtt.cfg");
    return path;
}

static void mqtt_zapisz_konfig(const struct mqtt_cfg *cfg)
{
    FILE *fp = fopen(mqtt_sciezka(), "w");
    if (fp == NULL) return;
    fprintf(fp, "broker=%s\n",   cfg->broker);
    fprintf(fp, "port=%d\n",     cfg->port);
    fprintf(fp, "topic=%s\n",    cfg->topic);
    fprintf(fp, "client_id=%s\n",cfg->client_id);
    fprintf(fp, "user=%s\n",     cfg->user);
    fprintf(fp, "password=%s\n", cfg->password);
    fprintf(fp, "qos=%d\n",      cfg->qos);
    fprintf(fp, "node_id=%d\n",  cfg->node_id);
    fprintf(fp, "interval=%d\n", cfg->interval);
    fclose(fp);
}

/* Kopiuje wartosc z pliku konfiguracyjnego. Za dluga wartosc jest pomijana
   (zostaje poprzednia/domyslna), bo ucieta nazwa hosta czy haslo i tak
   bylyby bledne. */
static void mqtt_kopiuj_pole(char *dst, size_t n, const char *val)
{
    size_t len = strlen(val);
    if (len >= n) return;
    memcpy(dst, val, len + 1);
}

static void mqtt_wczytaj_konfig(struct mqtt_cfg *cfg)
{
    char line[256], key[64], val[192];
    FILE *fp = fopen(mqtt_sciezka(), "r");
    if (fp == NULL) return;
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%63[^=]=%191[^\n]", key, val) != 2) continue;
        if      (!strcmp(key, "broker"))    mqtt_kopiuj_pole(cfg->broker, sizeof(cfg->broker), val);
        else if (!strcmp(key, "port"))      cfg->port      = atoi(val);
        else if (!strcmp(key, "topic"))     mqtt_kopiuj_pole(cfg->topic, sizeof(cfg->topic), val);
        else if (!strcmp(key, "client_id")) mqtt_kopiuj_pole(cfg->client_id, sizeof(cfg->client_id), val);
        else if (!strcmp(key, "user"))      mqtt_kopiuj_pole(cfg->user, sizeof(cfg->user), val);
        else if (!strcmp(key, "password"))  mqtt_kopiuj_pole(cfg->password, sizeof(cfg->password), val);
        else if (!strcmp(key, "qos"))       cfg->qos      = atoi(val);
        else if (!strcmp(key, "node_id"))   cfg->node_id  = atoi(val);
        else if (!strcmp(key, "interval"))  cfg->interval = atoi(val);
    }
    fclose(fp);
}

static void mqtt_ustaw_domyslne(struct mqtt_cfg *cfg)
{
    snprintf(cfg->broker,    sizeof(cfg->broker),    "localhost");
    cfg->port    = 1883;
    snprintf(cfg->topic,     sizeof(cfg->topic),     "stud_sys");
    snprintf(cfg->client_id, sizeof(cfg->client_id), "stud_sys_%d", (int)NODE_NO);
    cfg->user[0]     = 0;
    cfg->password[0] = 0;
    cfg->qos      = 0;
    cfg->node_id  = (int)NODE_NO;
    cfg->interval = 30;
    cfg->connected  = 0;
    cfg->pub_count  = 0;
    cfg->err_count  = 0;
}

/* ================================================================ */
/* Polaczenie / rozlaczenie                                         */
/* ================================================================ */
static int mqtt_polacz(struct mqtt_cfg *cfg)
{
#ifdef _MQTT_
    char addr[160];
    MQTTClient_connectOptions conn = MQTTClient_connectOptions_initializer;

    if (mqtt_client != NULL) {
        MQTTClient_disconnect(mqtt_client, 500);
        MQTTClient_destroy(&mqtt_client);
        mqtt_client = NULL;
    }
    snprintf(addr, sizeof(addr), "tcp://%s:%d", cfg->broker, cfg->port);

    if (MQTTClient_create(&mqtt_client, addr, cfg->client_id,
                          MQTTCLIENT_PERSISTENCE_NONE, NULL) != MQTTCLIENT_SUCCESS) {
        cfg->connected = 0;
        cfg->err_count++;
        return -1;
    }
    conn.keepAliveInterval = 20;
    conn.cleansession      = 1;
    conn.connectTimeout    = 3;
    if (cfg->user[0]) {
        conn.username = cfg->user;
        conn.password = cfg->password;
    }
    if (MQTTClient_connect(mqtt_client, &conn) != MQTTCLIENT_SUCCESS) {
        MQTTClient_destroy(&mqtt_client);
        mqtt_client    = NULL;
        cfg->connected = 0;
        cfg->err_count++;
        return -1;
    }
    cfg->connected = 1;
    return 0;
#else
    (void)cfg;
    return -1;
#endif
}

static void mqtt_rozlacz(struct mqtt_cfg *cfg)
{
#ifdef _MQTT_
    if (mqtt_client != NULL) {
        MQTTClient_disconnect(mqtt_client, 1000);
        MQTTClient_destroy(&mqtt_client);
        mqtt_client = NULL;
    }
#endif
    cfg->connected = 0;
}

/* ================================================================ */
/* Publikacja danych wszystkich aktywnych uslug RT                  */
/* ================================================================ */
#ifdef _MQTT_
static void name_to_slug(const char *name, char *slug, int maxlen)
{
    int i, j = 0;
    if (!name) { slug[0] = 'x'; slug[1] = 0; return; }
    for (i = 0; name[i] && j < maxlen - 1; i++) {
        char c = name[i];
        if (c >= 'A' && c <= 'Z') c = (char)(c + 32);
        if (c == ' ' || c == '\t' || c == '.') c = '_';
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_')
            slug[j++] = c;
    }
    slug[j] = 0;
    if (j == 0) { slug[0] = 'x'; slug[1] = 0; }
}
#endif /* _MQTT_ */

static void mqtt_publikuj_wszystkie(struct mqtt_cfg *cfg)
{
#ifdef _MQTT_
    int i;
    char topic[160], payload[384], slug[48];
    long ts = (long)time(NULL);

    if (mqtt_client == NULL || !cfg->connected) return;

    for (i = 0; i <= AgendaMax; i++) {
        struct agenda *A = SysA[i];
        if (A == NULL || A->number_of_calls == 0) continue;
        if ((A->S)->main_modul == NULL) continue;
        if ((A->S)->decyzje == dec_mqtt) continue; /* pominac siebie */

        name_to_slug(A->name ? A->name : (A->S)->name, slug, sizeof(slug));
        snprintf(topic, sizeof(topic), "%s/%d/%s",
                 cfg->topic, cfg->node_id, slug);
        snprintf(payload, sizeof(payload),
                 "{\"ts\":%ld,\"node\":%d,\"service\":\"%s\","
                 "\"state\":%d,\"alarm\":%d,"
                 "\"dana\":%.4f,\"wart_min\":%.4f,\"wart_max\":%.4f,\"czas\":%.1f}",
                 ts, cfg->node_id,
                 A->name ? A->name : (A->S)->name,
                 A->state, A->rt.alarm,
                 A->rt.value, A->rt.val_min, A->rt.val_max, A->rt.elapsed);

        {
            MQTTClient_message     msg   = MQTTClient_message_initializer;
            MQTTClient_deliveryToken tok = 0;
            msg.payload    = payload;
            msg.payloadlen = (int)strlen(payload);
            msg.qos        = cfg->qos;
            msg.retained   = 0;
            if (MQTTClient_publishMessage(mqtt_client, topic, &msg, &tok)
                    == MQTTCLIENT_SUCCESS) {
                MQTTClient_waitForCompletion(mqtt_client, tok, 1000);
                cfg->pub_count++;
            } else {
                cfg->connected = 0;
                cfg->err_count++;
            }
        }
    }
#else
    (void)cfg;
#endif
}

/* ================================================================ */
/* Modul glowny RT — wywolywany co cfg->interval sekund             */
/* ================================================================ */
int mqtt_main(void *DA)
{
    struct agenda  *A   = (struct agenda *)DA;
    struct mqtt_cfg *cfg = (struct mqtt_cfg *)(A->data);

    if (!cfg->connected) {
        mqtt_polacz(cfg);   /* proba reconnect co kazde wywolanie */
    }
    if (cfg->connected) {
        mqtt_publikuj_wszystkie(cfg);
    }
    A->state = 0;
    return A->state;
}

/* ================================================================ */
/* Formularz MQTT: rysuje go mqtt_odswiez_ekran(), wiec zostaje na  */
/* ekranie takze podczas wpisu wartosci (ui_czytaj_*).              */
/* ================================================================ */
#define MQTT_N_MENU 10
static struct {
    struct mqtt_cfg *cfg;   /* NULL: formularz zamkniety */
    char pola[7][80];
    char *menu[MQTT_N_MENU];
    int sel;
    struct ui_tlo tlo;
} MW;

/* Licznikami i stanem polaczenia zarzadza zlecenie RT w agendzie;
   formularz pokazuje je na zywo, gdy zlecenie istnieje. */
static const struct mqtt_cfg *mqtt_stan_rt(void)
{
    int i;
    for (i = 1; i <= AgendaMax; i++) {
        struct agenda *A = SysA[i];
        if (A != NULL && (A->S)->decyzje == dec_mqtt)
            return (const struct mqtt_cfg *)(A->data);
    }
    return MW.cfg;
}

static void mqtt_odswiez_ekran(int wymus)
{
    static time_t ostatnio = 0;
    time_t teraz = time(NULL);
    const struct mqtt_cfg *c = MW.cfg, *rt;
    char *w[MQTT_N_MENU + 1];
    int i, len, maxlen, xm, ym, yp = Y_G0 + 1, xp = X_L0;
    if (c == NULL) return;
    if (!wymus && teraz == ostatnio) return;
    ostatnio = teraz;
    rt = mqtt_stan_rt();
    snprintf(MW.pola[0], 80, L_MQTT_STATUS_LINE,
             rt->connected ? L_MQTT_STATE_CONNECTED : L_MQTT_STATE_DISCONNECTED,
             rt->pub_count, rt->err_count);
    snprintf(MW.pola[1], 80, L_MQTT_FIELD_BROKER, c->broker);
    snprintf(MW.pola[2], 80, L_MQTT_FIELD_PORT, c->port);
    snprintf(MW.pola[3], 80, L_MQTT_FIELD_TOPIC, c->topic);
    snprintf(MW.pola[4], 80, L_MQTT_FIELD_NODE_ID, c->node_id);
    snprintf(MW.pola[5], 80, L_MQTT_FIELD_QOS, c->qos);
    snprintf(MW.pola[6], 80, L_MQTT_FIELD_INTERVAL, c->interval);
    maxlen = (int)strlen(L_MQTT_TITLE);
    if (maxlen < 5) maxlen = 5; /* "<Esc>" */
    for (i = 0; i < MQTT_N_MENU; i++) {
        w[i] = (i < 7) ? MW.pola[i] : MW.menu[i];
        len = (int)strlen(w[i]);
        if (len > maxlen) maxlen = len;
    }
    w[MQTT_N_MENU] = "<Esc>";
    xm = xp + maxlen + 3;
    if (xm > MXR_MAX) xm = MXR_MAX;
    ym = yp + MQTT_N_MENU + 2;
    if (ym > MYR_MAX) ym = MYR_MAX;
    ui_przywroc_tlo(&MW.tlo); /* formularz mogl sie zwezic */
    ui_rysuj_okno(yp, xp, ym, xm, L_MQTT_TITLE, w, MQTT_N_MENU + 1, MW.sel, 2);
    term_flush();
}

/* Komunikat w wierszu statusu, czeka na klawisz */
static void mqtt_komunikat_status(void)
{
    ui_czekaj_klawisz(mqtt_odswiez_ekran);
    term_printf(MY_MAX, X_L0, ATTR_A, "%-*s", MX_MAX, " ");
}

/* ================================================================ */
/* Blankiet konfiguracji MQTT                                       */
/* ================================================================ */
int mqtt_blankiet(int nr_rekordu, int ob_pocz, int ob_konc,
                  int x_lewy_gorny, int y_lewy_gorny,
                  int kod_uslugi, char *adres_rek0_uslugi)
{
    struct mqtt_cfg cfg;
    int  w;

    (void)nr_rekordu; (void)ob_pocz;  (void)ob_konc;
    (void)x_lewy_gorny; (void)y_lewy_gorny;
    (void)kod_uslugi; (void)adres_rek0_uslugi;

    /* Zaladuj konfiguracje (domyslna + z pliku) */
    mqtt_ustaw_domyslne(&cfg);
    mqtt_wczytaj_konfig(&cfg);
#ifdef _MQTT_
    cfg.connected = (mqtt_client != NULL) ? 1 : 0;
#else
    cfg.connected = 0;
#endif

    /* Statyczne pozycje akcji */
    MW.menu[7] = L_MQTT_MENU_CONNECT;
    MW.menu[8] = L_MQTT_MENU_DISCONNECT;
    MW.menu[9] = L_BUDZ_MENU_EXIT;
    MW.cfg = &cfg;
    MW.sel = 0;
    ui_zapamietaj_tlo(&MW.tlo, Y_G0 + 1, MQTT_N_MENU + 3);

    for (;;) {
        w = ui_klawisz_menu(ui_czekaj_klawisz(mqtt_odswiez_ekran), &MW.sel,
                            MQTT_N_MENU, mqtt_odswiez_ekran);
        if (w == UI_NIC) continue;

        if (w < 0 || w == 9) {
            ui_przywroc_tlo(&MW.tlo);
            ui_zwolnij_tlo(&MW.tlo);
            MW.cfg = NULL;
            term_flush();
            return BLANKIET_UI_EXIT;
        }

        switch (w) {
        case 0: break; /* Status — tylko podglad */
        case 1: ui_czytaj_text(cfg.broker,    sizeof(cfg.broker),    L_MQTT_PROMPT_BROKER, mqtt_odswiez_ekran); break;
        case 2: cfg.port     = ui_czytaj_int(L_MQTT_PROMPT_PORT,    1, 65535, cfg.port, mqtt_odswiez_ekran);    break;
        case 3: ui_czytaj_text(cfg.topic,     sizeof(cfg.topic),     L_MQTT_PROMPT_TOPIC, mqtt_odswiez_ekran);  break;
        case 4: cfg.node_id  = ui_czytaj_int(L_MQTT_PROMPT_NODE_ID, 1, 9999,  cfg.node_id, mqtt_odswiez_ekran); break;
        case 5: cfg.qos      = ui_czytaj_int(L_MQTT_PROMPT_QOS,     0, 2,     cfg.qos, mqtt_odswiez_ekran);    break;
        case 6: cfg.interval = ui_czytaj_int(L_MQTT_PROMPT_INTERVAL, 1, 3600, cfg.interval, mqtt_odswiez_ekran); break;

        case 7: /* Polacz i zapisz */
            snprintf(cfg.client_id, sizeof(cfg.client_id), "stud_sys_%d", cfg.node_id);
            mqtt_rozlacz(&cfg);
            if (mqtt_polacz(&cfg) == 0) {
                /* Dodaj lub zaktualizuj zlecenie RT w agendzie */
                {
                    int i, found = 0;
                    for (i = 1; i <= AgendaMax; i++) {
                        struct agenda *A = SysA[i];
                        if (A == NULL) continue;
                        if ((A->S)->decyzje != dec_mqtt) continue;
                        /* juz istnieje — zaktualizuj interwal i cfg */
                        A->Interval = cfg.interval;
                        *(struct mqtt_cfg *)(A->data) = cfg;
                        found = 1; break;
                    }
                    if (!found) {
                        struct agenda *A = service_default(dec_mqtt);
                        if (A != NULL) {
                            A->mode             = 'p';
                            A->number_of_calls  = -1;
                            A->Interval         = cfg.interval;
                            A->delay            = cfg.interval;
                            A->prior            = 50;
                            *(struct mqtt_cfg *)(A->data) = cfg;
                            insert_to_agenda(A);
                            free_agenda_mem(A);
                        }
                    }
                }
                mqtt_zapisz_konfig(&cfg);
                term_printf(MY_MAX, X_L0, ATTR_A,
                            L_MQTT_OK_MSG,
                            cfg.broker, cfg.port, cfg.topic, cfg.node_id);
                mqtt_komunikat_status();
            } else {
                term_printf(MY_MAX, X_L0, ATTR_A,
                            L_MQTT_ERR_MSG,
                            cfg.broker, cfg.port, cfg.err_count);
                mqtt_komunikat_status();
            }
            /* Synchronizuj stan z agenda */
            {
                int i;
                for (i = 1; i <= AgendaMax; i++) {
                    struct agenda *A = SysA[i];
                    if (A && (A->S)->decyzje == dec_mqtt)
                        ((struct mqtt_cfg *)(A->data))->connected = cfg.connected;
                }
            }
            break;

        case 8: /* Rozlacz */
            mqtt_rozlacz(&cfg);
            {
                int i;
                for (i = 1; i <= AgendaMax; i++) {
                    struct agenda *A = SysA[i];
                    if (A && (A->S)->decyzje == dec_mqtt)
                        ((struct mqtt_cfg *)(A->data))->connected = 0;
                }
            }
            break;
        }
    }
}

/* ================================================================ */
/* Standardowe funkcje uslugi (dane/wpis/dec)                       */
/* ================================================================ */
char *dane_mqtt(int ob_pocz, int ob_konc, int *rozmiar_ob)
{
    static struct agenda *A = NULL;
    (void)ob_pocz; (void)ob_konc;
    *rozmiar_ob = sizeof(struct agenda) + sizeof(struct mqtt_cfg);
    A = (struct agenda *)Malloc(*rozmiar_ob);
    if (A == NULL) return NULL;
    lwmall++;
    A->S    = Service;
    A->data = (A + 1);
    return (char *)A;
}

void wpis_mqtt(int ob_pocz, int ob_konc, char *D,
               int rozmiar_ob, char zapis[], char *Kod_op)
{
    (void)ob_pocz; (void)ob_konc; (void)D;
    (void)rozmiar_ob; (void)zapis; (void)Kod_op;
}

int dec_mqtt(int decyzja, int kod_decyzji, int nr_dec,
             int kod_uslugi, int np, int *nr_rekordu)
{
    (void)decyzja; (void)kod_decyzji; (void)nr_dec;
    (void)np; (void)nr_rekordu;
    return kod_uslugi;
}

/* ================================================================ */
/* Auto-start: jesli plik konfiguracji istnieje — dodaj do agendy  */
/* ================================================================ */
void mqtt_auto_start(void)
{
    struct mqtt_cfg cfg;
    struct agenda  *A;
    FILE *fp;

    /* Sprawdz czy plik konfiguracji istnieje */
    fp = fopen(mqtt_sciezka(), "r");
    if (fp == NULL) return;
    fclose(fp);

    mqtt_ustaw_domyslne(&cfg);
    mqtt_wczytaj_konfig(&cfg);

    /* Proba polaczenia */
    if (mqtt_polacz(&cfg) != 0) return;

    /* Dodaj do agendy */
    A = service_default(dec_mqtt);
    if (A == NULL) { mqtt_rozlacz(&cfg); return; }
    A->mode            = 'p';
    A->number_of_calls = -1;
    A->Interval        = cfg.interval;
    A->delay           = cfg.interval;
    A->prior           = 50;
    *(struct mqtt_cfg *)(A->data) = cfg;
    insert_to_agenda(A);
    free_agenda_mem(A);
}

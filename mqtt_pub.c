#include "blank/moje.h"
#include "mqtt_pub.h"

#ifdef _MQTT_
#include <MQTTClient.h>
#endif

void chk_time(void);
extern struct Service  *Service;
extern unsigned int     attr, at_wpis;
extern unsigned int     cursor, nocursor;
extern int              AgendaMax, lwmall;
extern struct agenda   *SysA[];
extern unsigned short   NODE_NO;

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

static void mqtt_wczytaj_konfig(struct mqtt_cfg *cfg)
{
    char line[256], key[64], val[192];
    FILE *fp = fopen(mqtt_sciezka(), "r");
    if (fp == NULL) return;
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%63[^=]=%191[^\n]", key, val) != 2) continue;
        if      (!strcmp(key, "broker"))    snprintf(cfg->broker,    sizeof(cfg->broker),    "%s", val);
        else if (!strcmp(key, "port"))      cfg->port      = atoi(val);
        else if (!strcmp(key, "topic"))     snprintf(cfg->topic,     sizeof(cfg->topic),     "%s", val);
        else if (!strcmp(key, "client_id")) snprintf(cfg->client_id, sizeof(cfg->client_id), "%s", val);
        else if (!strcmp(key, "user"))      snprintf(cfg->user,      sizeof(cfg->user),      "%s", val);
        else if (!strcmp(key, "password"))  snprintf(cfg->password,  sizeof(cfg->password),  "%s", val);
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
/* Pomocnicze: wczytaj tekst w wierszu statusu                      */
/* ================================================================ */
static void mqtt_czytaj_text(char *buf, int max_len, const char *prompt)
{
    char tmp[MQTT_BROKER_LEN];
    int  len, ch;
    strncpy(tmp, buf, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = 0;
    len = (int)strlen(tmp);
    setcursor(cursor);
    for (;;) {
        term_printf(MY_MAX, X_L0, ATTR_A,
                    " %s: %-44s  (Enter=zatwierdz  Esc=anuluj)", prompt, tmp);
        ch = getch();
        if (ch == '\n' || ch == '\r' || ch == KEY_ENTER) break;
        if (ch == 27) { len = -1; break; }
        if ((ch == KEY_BACKSPACE || ch == 127 || ch == 8) && len > 0)
            tmp[--len] = 0;
        else if (ch >= 32 && ch < 256 && len < max_len - 1)
            { tmp[len++] = (char)ch; tmp[len] = 0; }
    }
    term_printf(MY_MAX, X_L0, ATTR_A, "%-79s", " ");
    setcursor(nocursor);
    if (len > 0) { strncpy(buf, tmp, (size_t)(max_len - 1)); buf[max_len - 1] = 0; }
}

static int mqtt_czytaj_int(const char *prompt, int min_v, int max_v, int cur)
{
    char buf[12];
    int  len, ch, val;
    snprintf(buf, sizeof(buf), "%d", cur);
    len = (int)strlen(buf);
    setcursor(cursor);
    for (;;) {
        term_printf(MY_MAX, X_L0, ATTR_A,
                    " %s [%d-%d]: %-8s  (Enter=zatwierdz  Esc=anuluj)",
                    prompt, min_v, max_v, buf);
        ch = getch();
        if (ch == '\n' || ch == '\r' || ch == KEY_ENTER) break;
        if (ch == 27) { len = -1; break; }
        if ((ch == KEY_BACKSPACE || ch == 127 || ch == 8) && len > 0)
            buf[--len] = 0;
        else if (ch >= '0' && ch <= '9' && len < (int)(sizeof(buf) - 2))
            { buf[len++] = (char)ch; buf[len] = 0; }
    }
    term_printf(MY_MAX, X_L0, ATTR_A, "%-79s", " ");
    setcursor(nocursor);
    if (len <= 0) return cur;
    val = atoi(buf);
    if (val < min_v) val = min_v;
    if (val > max_v) val = max_v;
    return val;
}

/* ================================================================ */
/* Blankiet konfiguracji MQTT                                       */
/* ================================================================ */
int mqtt_blankiet(int nr_rekordu, int ob_pocz, int ob_konc,
                  int x_lewy_gorny, int y_lewy_gorny,
                  int kod_uslugi, char *adres_rek0_uslugi)
{
    struct mqtt_cfg cfg;
    char m0[80], m1[80], m2[80], m3[80], m4[80], m5[80], m6[80];
    char *menu[10];
    int  w, lw = 0;

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
    menu[7] = "  >>> Polacz / Zapisz <<<";
    menu[8] = "  >>> Rozlacz         <<<";
    menu[9] = "  Wyjdz";

    for (;;) {
        /* Wypelnij dynamiczne pola */
        snprintf(m0, 80, "  Status   : %s  (pub:%d  err:%d)",
                 cfg.connected ? "POLACZONY" : "Rozlaczony",
                 cfg.pub_count, cfg.err_count);
        snprintf(m1, 80, "  Broker   : %s", cfg.broker);
        snprintf(m2, 80, "  Port     : %d", cfg.port);
        snprintf(m3, 80, "  Topic    : %s", cfg.topic);
        snprintf(m4, 80, "  Node ID  : %d", cfg.node_id);
        snprintf(m5, 80, "  QoS      : %d", cfg.qos);
        snprintf(m6, 80, "  Interwal : %d s", cfg.interval);
        menu[0] = m0;
        menu[1] = m1;
        menu[2] = m2;
        menu[3] = m3;
        menu[4] = m4;
        menu[5] = m5;
        menu[6] = m6;

        if (lw > 9) lw = 0;
        w = okno_menu(menu, 10, lw, attr, at_wpis,
                      Y_G0 + 1, X_L0, -1, " MQTT PUBLISHER ", 1);
        lw = (w >= 0) ? w : 0;

        if (w < 0 || w == 9) return BLANKIET_UI_EXIT;

        switch (w) {
        case 0: break; /* Status — tylko podglad */
        case 1: mqtt_czytaj_text(cfg.broker,    sizeof(cfg.broker),    "Broker"); break;
        case 2: cfg.port     = mqtt_czytaj_int("Port",    1, 65535, cfg.port);    break;
        case 3: mqtt_czytaj_text(cfg.topic,     sizeof(cfg.topic),     "Topic");  break;
        case 4: cfg.node_id  = mqtt_czytaj_int("Node ID", 1, 9999,  cfg.node_id); break;
        case 5: cfg.qos      = mqtt_czytaj_int("QoS",     0, 2,     cfg.qos);    break;
        case 6: cfg.interval = mqtt_czytaj_int("Interwal [s]", 1, 3600, cfg.interval); break;

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
                            " Polaczono z %s:%d  (topic: %s/%d/...)",
                            cfg.broker, cfg.port, cfg.topic, cfg.node_id);
                GET_char();
                term_printf(MY_MAX, X_L0, ATTR_A, "%-79s", " ");
            } else {
                term_printf(MY_MAX, X_L0, ATTR_A,
                            " Blad polaczenia z %s:%d  (err: %d) <Ent>",
                            cfg.broker, cfg.port, cfg.err_count);
                GET_char();
                term_printf(MY_MAX, X_L0, ATTR_A, "%-79s", " ");
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

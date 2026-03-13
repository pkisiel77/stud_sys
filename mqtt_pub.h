#ifndef MQTT_PUB_H
#define MQTT_PUB_H

#define MQTT_BROKER_LEN 128
#define MQTT_TOPIC_LEN   64
#define MQTT_ID_LEN      32
#define MQTT_USER_LEN    32

struct mqtt_cfg {
    char broker[MQTT_BROKER_LEN]; /* hostname lub IP brokera          */
    int  port;                    /* port (domyslnie 1883)             */
    char topic[MQTT_TOPIC_LEN];   /* temat bazowy, np. "stud_sys"     */
    char client_id[MQTT_ID_LEN];  /* identyfikator klienta MQTT        */
    char user[MQTT_USER_LEN];     /* uzytkownik (opcjonalnie)          */
    char password[MQTT_USER_LEN]; /* haslo (opcjonalnie)               */
    int  qos;                     /* jakosc uslug: 0/1/2              */
    int  node_id;                 /* identyfikator wezla (NODE_NO)    */
    int  interval;                /* interwal publikacji [s]           */
    /* stan biezacy - nie jest zapisywany */
    int  connected;               /* 1 = polaczony                     */
    int  pub_count;               /* liczba opublikowanych komunikatow */
    int  err_count;               /* liczba bledow polaczenia          */
};

int   mqtt_main(void *DA);
int   mqtt_blankiet(int nr_rekordu, int ob_pocz, int ob_konc,
                    int x_lewy_gorny, int y_lewy_gorny,
                    int kod_uslugi, char *adres_rek0_uslugi);
char *dane_mqtt(int ob_pocz, int ob_konc, int *rozmiar_ob);
void  wpis_mqtt(int ob_pocz, int ob_konc, char *D,
                int rozmiar_ob, char zapis[], char *Kod_op);
int   dec_mqtt(int decyzja, int kod_decyzji, int nr_dec,
               int kod_uslugi, int np, int *nr_rekordu);
void  mqtt_auto_start(void);

#endif /* MQTT_PUB_H */

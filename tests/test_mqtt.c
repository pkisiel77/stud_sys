/* Testy jednostkowe konfiguracji MQTT Publisher.
   Plik wlacza mqtt_pub.c, zeby sprawdzic funkcje static; ten test jest
   linkowany ze wszystkimi modulami oprocz mqtt_pub.c (CMakeLists.txt). */
#include "mqtt_pub.c"
#include "test_util.h"

#include <unistd.h>

static void test_domyslne(void)
{
    struct mqtt_cfg c;
    memset(&c, 0x55, sizeof(c));
    mqtt_ustaw_domyslne(&c);
    CHECK_STR(c.broker, "localhost");
    CHECK(c.port == 1883);
    CHECK_STR(c.topic, "stud_sys");
    CHECK(c.qos == 0);
    CHECK(c.interval == 30);
    CHECK(c.node_id == (int)NODE_NO);
    CHECK(c.user[0] == 0 && c.password[0] == 0);
    CHECK(c.connected == 0 && c.pub_count == 0 && c.err_count == 0);
}

static void test_zapis_i_odczyt(void)
{
    struct mqtt_cfg a, b;
    mqtt_ustaw_domyslne(&a);
    snprintf(a.broker, sizeof(a.broker), "broker.example.org");
    a.port = 8883;
    snprintf(a.topic, sizeof(a.topic), "lab/rt");
    snprintf(a.client_id, sizeof(a.client_id), "stud_sys_7");
    snprintf(a.user, sizeof(a.user), "student");
    snprintf(a.password, sizeof(a.password), "tajne");
    a.qos = 1;
    a.node_id = 7;
    a.interval = 15;
    a.connected = 1;               /* stan biezacy: nie jest zapisywany */
    a.pub_count = 42;
    mqtt_zapisz_konfig(&a);

    mqtt_ustaw_domyslne(&b);
    mqtt_wczytaj_konfig(&b);
    CHECK_STR(b.broker, "broker.example.org");
    CHECK(b.port == 8883);
    CHECK_STR(b.topic, "lab/rt");
    CHECK_STR(b.client_id, "stud_sys_7");
    CHECK_STR(b.user, "student");
    CHECK_STR(b.password, "tajne");
    CHECK(b.qos == 1 && b.node_id == 7 && b.interval == 15);
    CHECK(b.connected == 0 && b.pub_count == 0);
}

static void test_bledny_plik(void)
{
    struct mqtt_cfg c;
    char dlugi[300];
    FILE *fp = fopen(mqtt_sciezka(), "w");
    CHECK(fp != NULL);
    if (fp == NULL) return;
    memset(dlugi, 'h', sizeof(dlugi) - 1);
    dlugi[sizeof(dlugi) - 1] = 0;
    fprintf(fp, "smieci bez znaku rownosci\n");
    fprintf(fp, "nieznany=1\n");
    fprintf(fp, "port=1884\n");
    fprintf(fp, "broker=%.190s\n", dlugi);   /* za dlugi: pomijany */
    fprintf(fp, "topic=%.70s\n", dlugi);     /* dluzszy niz MQTT_TOPIC_LEN */
    fclose(fp);

    mqtt_ustaw_domyslne(&c);
    mqtt_wczytaj_konfig(&c);
    CHECK(c.port == 1884);
    CHECK_STR(c.broker, "localhost");
    CHECK_STR(c.topic, "stud_sys");
}

static void test_brak_pliku(void)
{
    struct mqtt_cfg c;
    remove(mqtt_sciezka());
    mqtt_ustaw_domyslne(&c);
    mqtt_wczytaj_konfig(&c);
    CHECK_STR(c.broker, "localhost");
    CHECK(c.port == 1883);
}

static void test_kopiuj_pole(void)
{
    char dst[8] = "stare";
    mqtt_kopiuj_pole(dst, sizeof(dst), "nowe");
    CHECK_STR(dst, "nowe");
    mqtt_kopiuj_pole(dst, sizeof(dst), "1234567");   /* 7 znakow + NUL miesci sie */
    CHECK_STR(dst, "1234567");
    mqtt_kopiuj_pole(dst, sizeof(dst), "12345678");  /* za dlugie: bez zmian */
    CHECK_STR(dst, "1234567");
}

#ifdef _MQTT_
static void test_slug(void)
{
    char s[16];
    name_to_slug("Czujnik Sim", s, sizeof(s));
    CHECK_STR(s, "czujnik_sim");
    name_to_slug("Sprawdz.otoczenia", s, sizeof(s));
    CHECK_STR(s, "sprawdz_otoczen");   /* obciete do maxlen-1 */
    name_to_slug("#!?", s, sizeof(s));
    CHECK_STR(s, "x");
    name_to_slug(NULL, s, sizeof(s));
    CHECK_STR(s, "x");
}
#endif

int main(void)
{
    test_ustaw_home();

    test_domyslne();
    test_zapis_i_odczyt();
    test_bledny_plik();
    test_brak_pliku();
    test_kopiuj_pole();
#ifdef _MQTT_
    test_slug();
#endif

    remove(mqtt_sciezka());
    rmdir(test_home);
    return test_wynik("mqtt");
}

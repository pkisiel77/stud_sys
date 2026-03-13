#ifndef SENSOR_SIM_H
#define SENSOR_SIM_H

/* Konfiguracja symulowanego czujnika RT.
 * Dane specyficzne uslugi przechowywane w agenda->data.         */
struct sensor_sim {
    char  waveform;   /* typ przebiegu: 's'=sinus 'q'=prostokat 'w'=pila 'n'=szum */
    float amplitude;  /* amplituda [j.umowne]                                      */
    float frequency;  /* czestotliwosc [cykl/s przy Interval=1s]                  */
    float offset;     /* przesuniecie DC                                           */
    float thr_max;    /* prog alarmu gorny  -> RT_ALARM_RANGE                      */
    float thr_min;    /* prog alarmu dolny  -> RT_ALARM_RANGE                      */
};

int   sensor_sim_main(void *DA);
int   sensor_sim_blankiet(int nr_rekordu, int ob_pocz, int ob_konc,
                          int x_lewy_gorny, int y_lewy_gorny,
                          int kod_uslugi, char *adres_rek0_uslugi);
char *dane_sensor_sim(int ob_pocz, int ob_konc, int *rozmiar_ob);
void  wpis_sensor_sim(int ob_pocz, int ob_konc, char *D,
                      int rozmiar_ob, char zapis[], char *Kod_op);
int   dec_sensor_sim(int decyzja, int kod_decyzji, int nr_dec,
                     int kod_uslugi, int np, int *nr_rekordu);

#endif /* SENSOR_SIM_H */

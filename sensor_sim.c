/* sensor_sim.c — Symulowany czujnik RT
 *
 * Przyklad minimalnego modulu uslugi RT.  Generuje sygnal syntetyczny
 * (sinus / prostokat / pila / szum) i zapisuje wynik do A->rt.
 * sprawdz.c i MQTT odbieraja dane automatycznie bez znajomosci
 * tego modulu.
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "blank/moje.h"
#include "blank/sys_dekl.h"
#include "sensor_sim.h"
#include "loc.h"

/* forward declarations — funkcje zaimplementowane w core.c */
int  dane_agendy(struct agenda *A, struct agenda *An, int cykl_max);
int  pobierz_rekord_uslugi(int *nr_rekordu, int kod_uslugi, int ob_konc,
                            struct agenda **As, struct agenda **An,
                            struct agenda ***SA, char *adres_rek0, char *nazwa);
int  decyzje_run(char decyzja, struct agenda **Aserv, struct agenda **Anew,
                 int *nr_rekordu, int kod_uslugi, char *tytul);

extern struct Service *Service;
extern unsigned int attr, at_wpis;
extern int lwmall;
extern struct agenda *SysA[];

/* stan lokalny uslugi — po jednej kopii na modul (Etap 1a) */
static struct agenda *Anew = NULL, *Aserv = NULL;

/* kody decyzji lokalne dla tego modulu */
#define DEC_SIM_WAVE 30
#define DEC_SIM_AMP  31
#define DEC_SIM_FREQ 32
#define DEC_SIM_OFFS 33
#define DEC_SIM_TMAX 34
#define DEC_SIM_TMIN 35

/* ================================================================
 * RT callback — wywolywany co A->Interval sekund przez scheduler
 * ================================================================ */
int sensor_sim_main(void *DA)
{
    struct agenda    *A = (struct agenda *)DA;
    struct sensor_sim *S;
    float phase, value;

    if (A == NULL) return 0;
    S = (struct sensor_sim *)(A->data);
    if (S == NULL) return 0;

    /* generuj nastepna probke */
    switch (S->waveform) {
        case 's':   /* sinus */
            value = S->offset +
                    S->amplitude * sinf(2.0f * (float)M_PI * S->frequency * A->rt.elapsed);
            break;
        case 'q':   /* prostokat (sign sinusa) */
            value = S->offset +
                    S->amplitude * (sinf(2.0f * (float)M_PI * S->frequency * A->rt.elapsed)
                                    >= 0.0f ? 1.0f : -1.0f);
            break;
        case 'w':   /* pila narastajaca */
            phase  = S->frequency * A->rt.elapsed;
            phase -= floorf(phase);          /* 0..1 */
            value  = S->offset + S->amplitude * (2.0f * phase - 1.0f);
            break;
        default:    /* szum bialy */
            value = S->offset +
                    S->amplitude * ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
            break;
    }

    /* zapisz wynik do rt_data */
    A->rt.value = value;

    if (A->rt.elapsed <= 0.0f) {
        A->rt.val_max = value;
        A->rt.val_min = value;
    } else {
        if (value > A->rt.val_max) A->rt.val_max = value;
        if (value < A->rt.val_min) A->rt.val_min = value;
    }

    A->rt.alarm = (value > S->thr_max || value < S->thr_min)
                  ? RT_ALARM_RANGE : RT_ALARM_OK;

    A->rt.elapsed += 1.0f;
    A->state = 1;
    return A->state;
}

/* ================================================================
 * Blankiet — formularz UI
 * ================================================================ */
int sensor_sim_blankiet(int nr_rekordu, int ob_pocz, int ob_konc,
                        int x_lewy_gorny, int y_lewy_gorny,
                        int kod_uslugi, char *adres_rek0_uslugi)
{
    struct agenda    *A;
    struct sensor_sim *S;
    struct agenda   **SA;
    int    ret, ochr, nr_rek;
    int    cykl_max = 86400;   /* maks. 24h ciaglej pracy */

    static char *menu_wave[4] = {L_SIM_WAVE_SINE, L_SIM_WAVE_SQUARE, L_SIM_WAVE_SAW, L_SIM_WAVE_NOISE};
    static int   nr_wave = 0, w_min = 0, w_max = 3;
    static float f0 = 0.0f, f100 = 100.0f, f_pos = 1000.0f, f_neg = -1000.0f;

    (void)nr_rekordu; (void)ob_pocz; (void)x_lewy_gorny; (void)y_lewy_gorny;

    ret = pobierz_rekord_uslugi(&nr_rek, kod_uslugi, ob_konc,
                                &A, &Anew, &SA,
                                adres_rek0_uslugi, L_SIM_SERVICE_NAME);
    Aserv = A;
    if (A == NULL) { if (nr_rek < 0) return ret; return BLANKIET_UI_EXIT; }

    S = (struct sensor_sim *)(A->data);
    if (S == NULL) return BLANKIET_UI_EXIT;

    /* ochr=-2 -> tylko odczyt (usluga uruchomiona); ochr=4 -> edycja (nowe zlecenie) */
    ochr = (Anew == NULL) ? -2 : 4;

    if (Anew == NULL)
        ret = dana_koment(-1, 13, L_SIM_LIVE_READONLY);
    else
        ret = dana_koment(-1, -1, "+  ");

    ret = dana_koment(-1, -1, L_SIM_HEADER);
    ret = dana_koment(-1, -1, L_SIM_VALUE, (int)A->rt.value);
    { char vbuf[32]; snprintf(vbuf, sizeof(vbuf), "%10.4f", (double)A->rt.value);
      ret = dana_koment(-1, -1, L_SIM_VALUE2, vbuf); }
    ret = dana_koment(-1, -1, L_SIM_VALUE3, A->rt.value);
    ret = dana_koment(-1, -1, L_SIM_MIN_MAX, A->rt.val_min, A->rt.val_max);
    ret = dana_koment(-1, -1, L_SIM_SAMPLES,  A->rt.elapsed);
    ret = dana_koment(-1, -1, L_SIM_ALARM_LABEL,
                      A->rt.alarm == RT_ALARM_OK    ? L_SIM_ALARM_OK :
                      A->rt.alarm == RT_ALARM_RANGE ? L_SIM_ALARM_RANGE : L_SIM_ALARM_TIME);
    ret = dana_koment(-1, -1, "+ -------------------------------");

    /* typ przebiegu */
    switch (S->waveform) {
        case 's': nr_wave = 0; break;
        case 'q': nr_wave = 1; break;
        case 'w': nr_wave = 2; break;
        default:  nr_wave = 3; break;
    }
    ret = dana_int_menu(-1, -1, L_SIM_FIELD_WAVE,
                        &w_min, &w_max, &nr_wave, 12, ochr, -1, 4, menu_wave);
    switch (nr_wave) {
        case 0: S->waveform = 's'; break;
        case 1: S->waveform = 'q'; break;
        case 2: S->waveform = 'w'; break;
        default: S->waveform = 'n'; break;
    }

    ret = dana_float_dec(-1, -1, L_SIM_FIELD_AMP, &f0,    &f100,
                         &S->amplitude, 8, 3, ochr, 0, DEC_SIM_AMP);
    ret = dana_float_dec(-1, -1, L_SIM_FIELD_FREQ, &f0,    &f100,
                         &S->frequency, 8, 3, ochr, 0, DEC_SIM_FREQ);
    ret = dana_float_dec(-1, -1, L_SIM_FIELD_OFFS, &f_neg, &f_pos,
                         &S->offset,    8, 3, ochr, 0, DEC_SIM_OFFS);
    ret = dana_float_dec(-1, -1, L_SIM_FIELD_TMAX, &f_neg, &f_pos,
                         &S->thr_max,   8, 3, ochr, 0, DEC_SIM_TMAX);
    ret = dana_float_dec(-1, -1, L_SIM_FIELD_TMIN, &f_neg, &f_pos,
                         &S->thr_min,   8, 3, ochr, 0, DEC_SIM_TMIN);

    ret = dana_koment(-1, -1, "+ -------------------------------");

    return dane_agendy(A, Anew, cykl_max);
}

/* ================================================================
 * Decyzje — zarzadzanie zleceniami w agendzie
 * ================================================================ */
int dec_sensor_sim(int decyzja, int kod_decyzji, int nr_dec,
                   int kod_uslugi, int np, int *nr_rekordu)
{
    struct agenda    *A = NULL;
    struct sensor_sim *S;
    int ret = 0;

    (void)nr_dec; (void)np;

    if (kod_decyzji == DEC_NEW && decyzja == 't') {
        if (Anew == NULL) Anew = service_default(dec_sensor_sim);
        if (Anew == NULL) return kod_uslugi;
        A = Anew;
        S = (struct sensor_sim *)(A->data);
        /* domyslna konfiguracja */
        S->waveform  = 's';
        S->amplitude = 1.0f;
        S->frequency = 0.1f;   /* okres ~10s przy Interval=1s */
        S->offset    = 0.0f;
        S->thr_max   =  1.5f;
        S->thr_min   = -1.5f;
        A->Interval  = 1;      /* co 1 sekunde */
        Aserv = Anew;
        return kod_uslugi;
    } else {
        if (Anew != NULL) A = Anew;
        else              A = Aserv;
        if (A == NULL) return kod_uslugi;
    }

    if (kod_decyzji == DEC_RUN) {
        ret = decyzje_run(decyzja, &A, &Anew, nr_rekordu,
                          kod_uslugi, L_SIM_LIST_TITLE);
        if (ret == -1) return -1;
        Aserv = A;
    }
    return ret;
}

/* ================================================================
 * Persistence — stubs (dane sa czysto RT, brak zapisu na dysk)
 * ================================================================ */
char *dane_sensor_sim(int ob_pocz, int ob_konc, int *rozmiar_ob)
{
    (void)ob_pocz; (void)ob_konc; (void)rozmiar_ob;
    return NULL;
}

void wpis_sensor_sim(int ob_pocz, int ob_konc, char *D,
                     int rozmiar_ob, char zapis[], char *Kod_op)
{
    (void)ob_pocz; (void)ob_konc; (void)D;
    (void)rozmiar_ob; (void)zapis; (void)Kod_op;
}

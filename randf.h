#ifndef RANDF_H
#define RANDF_H

/* Generatory liczb losowych i zaklocen (randf.c). */
double RND01(void);   /* rozklad rownomierny <0, 1>      */
double RNDM(void);    /* rozklad rownomierny <-0.5, 0.5> */
float GAUSS(void);    /* rozklad normalny N(0,1)         */
float GAUSSu(void);
double dane_V(float Var, float Tv, float dt, double* q);
double Zakloc_V(float t, float t0, double Kv, double q, double v);
void dane_Z(float Var, float Tvd, float Tvg, int Lsin, float A[], float om[], float FI[]);
float Zakloc_Z(float t, float A[], float om[], float FI[], int Lsin);

#endif /* RANDF_H */

#include <math.h>
/* ---------------------- GENERATORY LICZ LOSOWYCH ------------------ */
static unsigned int M=0xffffffff,A=251171,IX=217013;
/* --------------------- GENERATOR LICZ LOSOWYCH <0, 1> --------------- */
double RND01(void)
 {double rnd;
   IX=(IX*A)%M;
   rnd=((double)IX)/((double)M);
   return rnd;
} /* end of RNDM() */

/* --------------------- GENERATOR LICZ LOSOWYCH <-0.5, 0.5> --------------- */
double RNDM(void)
 {double rnd;
   IX=(IX*A)%M;
   rnd=((double)IX)/((double)M)-0.5;
   return rnd;
} /* end of RNDM() */
/* ------------------- GENERATOR LICZ LOSOWYCH N(0,1) --------------- */
#define NRN 12
#define NRN2 NRN/2
#define SQR sqrt(12.)/(double)NRN
float GAUSS(void)
{int i;
 double r;
 float rn;
  r=0.;
  for(i=0;i<NRN;i++)  {r+=RND01();}
  rn=(r-(double)NRN2)*SQR;
  return(rn);
} /* end of GAUSS()  */

#define R1N 0.0117
#define XSR 0.0
#define SIG 0.989
float GAUSSu(void)
{ int i;
  double r=0.;
  static double rn=0., R1=0.7931793157975315319;
  R1=RND01();
  for(i=0;i<NRN;i++)  r += R1;
  rn=(r-(double)NRN2)*SQR-rn*R1N;
  return((rn-XSR)/SIG);
} /* end of GAUSSu()  */


/*-------------------------------------------------------------------------*/
/*                   Generatory  ZAKLOCEN                                      */
/*-------------------------------------------------------------------------*/
/*             Zwraca  chwilowa wartosc zaklocenia                           */
#define EPS 1.e-6
#define PI 3.1415
#define LSIN 10
double dane_V(float Var, float Tv, float dt, double *q)
 {double Kv;
  int i;
  for(i=0;i<50;i++) RNDM();
  *q=exp(-dt/Tv);
  Kv=(1.-(*q))*sqrt(6.*Var*Tv/dt);
  return Kv;
 }

double Zakloc_V(float t, float t0, double Kv, double q, double v)
 {double u;
  if(t<t0) return v;
  u=RNDM();
  v=q*v+Kv*u;
  return v;
 }
/**************************************************************************/
static float RFI[LSIN]={.3947,-.17395,-.98011,.44829,-.3765,0.7593,-.8813,
                        .0177,0.8017,0.2873};
void dane_Z(float Var, float Tvd, float Tvg, int Lsin, float A[], float om[], float FI[])
/*float Var,      wariancja zaklocenia
        Tvd,      okres czest. minimalnej
        Tvg;      okres czest. maxymalnej */
 {double omd,omg,Om,dom;
  int i;
   omd=2.*PI/Tvd; omg=2.*PI/Tvg; Om=omg-omd;
   dom=Om/((double)Lsin); om[0]=omd+dom/2.;
   for(i=0;i<Lsin;i++)
    {om[i]=om[0]+(double)i*(dom+RFI[i]/1000.);
     A[i]=sqrt(2./((double)Lsin)*Var);
     FI[i]=2.*PI*RFI[i]*Tvd;
    }
  return;
 }

float Zakloc_Z(float t, float A[], float om[], float FI[], int Lsin)
 {float v=0.;
  int i;
  for(i=0;i<Lsin;i++)
   v+=A[i]*sin(om[i]*t+FI[i]);
  return(v);
 } /* end of Z() */
/*-------------------------------------------------------------------------*/

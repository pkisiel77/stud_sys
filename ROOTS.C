/*--------------------------MLIB.C----9.10.92-----------------------------*/
/*   pmpy(),gmprd(),ROOT2(),ROOT3(),ROOT4(),POLRT(),wydruk(),powj(),Pow(),
     F(),RK()                                                             */
#define _DOS_
/*#define _MSOR_ */
#include <stdlib.h>
#include <stdio.h>
#include <alloc.h>
#include <conio.h>
#include <string.h>
#include <math.h>
#ifdef _MSOR_
typedef int int;
#define GET_char getch
int term_printf(int y, int x, unsigned int attr, char *format,...);
#ifdef _DOS_
#define ATTR_A 0x043f
#define MY_MAX 25
#define X_L0 1
#else
typedef short int int;
#define ATTR_A 0x043f
#define MY_MAX 24
#define X_L0 0
#endif
#endif
/*#define float double*/
#define DOKL    1.e-5
#define EPS3    1.e-22
#define EPS     1.e-20
#define ITER_MAX 100
#define L_START    4
#define CR        13
#define ESC       27
#define Stala    2.0943951
#define RE        1
#define IM        2
#define L_WEJSC   5
#define L_ZMIEN   L_WEJSC+1
#define POCZ      0
#define RANG     36
#define AKT       1
#define NOWY      2
#define STAR      3
#define MAX_DSP   6            /* maksym.liczba rownocz.wpisanych wart. y */
#define MAX_DYI   15           /* maksym.liczba wart.y obl.przez interpol */
#define MAXMIN    1440         /* liczba minut w dobie */
#define MRMW      3            /* maxym.stopien mianownika jednej transm. */
#define MAXRM L_WEJSC*MRMW     /* maxym.stopien mianownika calego modelu  */

/*---------------------- typy funkcji------------------------------------ */
void gmprd(double AA[],double B[],double R[],int n,int m,int l);
void MNOZ(double A[],double B[],double C[],int lwier_Awier_C,
          int lkol_Awier_B,int lkol_Bkol_C);
void RK(double x[],double u,double A[RANG][RANG],double B[],double ht,
        char rzad);
void ROOT2(double A[],double X_real[],double X_image[]);
void ROOT3(double A[], double X_real[],double X_image[]);
void ROOT4(double A[],int rang,double X_real[],double X_image[]);
int POLRT(double XCOF[],int MM,double ROOTR[],double ROOTI[],float tol);
double Pow(double x,int m);
double Pot(double x, double p);
void ZERR(int dim,double c[]);
/*------1.---------------------------------------------------------------*/
int pmpy(double X[],int n,double Y[],int m,double Z[])
{int i, j, l;
  if(n*m > 0)
    {l=n+m-1;  for(i=0;i<l;i++)  Z[i]=0.;
     for(i=0;i<n;i++)   {for(j=0;j<m;j++)   Z[i+j]=Z[i+j] + X[i]*Y[j];}
    }
  else l=0;
 return(l);
}
/*-----2.------------------------------------------------------------------*/
void gmprd(double AA[],double B[],double R[],int n,int m,int l)
{int ir=-1, ik, j, k, ji, ib, i;
  ik = -m;
  for(k=0;k<l;k++)
    {ik+=m;  for(j=0;j<n;j++)
               {ir++; ji=j-n; ib=ik; R[ir]=0.;
                for(i=0;i<m;i++)
                  {ji+=n;  R[ir]+=AA[ji]*B[ib]; ib++;}
               }
    }
 return;
}
/*----3.-------------------------------------------------------------------*/
void ROOT2(double A[],double X_real[],double X_image[])
 {double delta,R;
  delta=A[1]*A[1]-4.*A[2]*A[0];
  R=-A[1]/(2.*A[2]);
  if(delta>=0.) {X_real[0]=R+sqrt(delta)/(2.*A[2]);
                 X_real[1]=R-sqrt(delta)/(2.*A[2]);
                 X_image[0]=0.; X_image[1]=0.;
                }
  else {X_real[1]=R; X_real[0]=R;
        X_image[0]=-sqrt(-delta)/(2.*A[2]);
        X_image[1]=-X_image[0];
       }
  return;
 }
/*----4.-------------------------------------------------------------------*/
void ROOT3(double A[], double X_real[],double X_image[])
{int i, rang=3;
 double delta, B[2], C,D,P,Q,R, x_rob1,x_rob2,x_rob3, Fi, pp, qq, rr, ww;
 double cos_Fi, x_image;
/* ======= Rownanie: A[0]+A[1]*x+A[2]*x^2+A[3]*x^3=0 ================== */
  for(i=0;i<rang;i++)  X_image[i]=0.;
  if((fabs)(A[3]-1.) < EPS)  {P=A[2];      Q=A[1];      R=A[0];}
  else                       {P=A[2]/A[3]; Q=A[1]/A[3]; R=A[0]/A[3];}
  if(fabs(P)<1.e-10 && fabs(Q)<1.e-10)
   {rr=R; if(R < 0.) rr=-R; rr=Pot((double)rr,(double)(1./rang));
    if(R > 0.) rr=-rr;
    x_rob1=rr;  x_rob2=rr; x_rob3=rr; goto K;
   }
  B[0]=(2.*P*P*P + 27.*R - 9.*P*Q)/27.;
  B[1]=(3.*Q - P*P)/3.;  C=B[0]*B[0];  D=B[1]*B[1]*B[1];
  delta=C/4. + D/27.;
  if(delta <= EPS)
/*....................pierwiastki rzeczywiste............................*/
   {rr=(fabs)((double)delta);
    if(rr < EPS)
     {rr=-B[1]/3.;
#ifdef _MSOR_
      if(rr < 0.) {term_print(MY_MAX,X_L0,ATTR_A," Uwaga: -a/3. jest ujemne"); GET_char();}
#else
      if(rr < 0.) {printf("\n Uwaga: -a/3. jest ujemne"); getch();}
#endif
      rr=sqrt(fabs(rr));
      if(B[0] > 0.) {x_rob1=-2.*rr;  x_rob2=rr;   x_rob3=rr;}
      else          {x_rob1=2.*rr;   x_rob2=-rr;  x_rob3=-rr;}
     }
    else {rr=(C/4.)/(-D/27.);
#ifdef _MSOR_
          if(rr<0.) {term_printf(MY_MAX,X_L0,ATTR_A," Uwaga: (b*b/4)/(-a*a*a/27) jest ujemne");
                     GET_char(); rr=1.e-20;
                    }
#else
          if(rr<0.) {printf("\n Uwaga: (b*b/4)/(-a*a*a/27) jest ujemne");
                     getch();  rr=1.e-20;
                    }
#endif
          rr=sqrt(fabs(rr));
          if(B[0] > 0.)  cos_Fi=-rr;
          else           cos_Fi= rr;
          Fi=acos((double)cos_Fi);
          rr=-B[1]/3.;
          if(rr < 0.) rr=0.; /*{printf("\n Uwaga: -a/3. jest ujemne"); getch();}*/
          else rr=sqrt((double)rr);
          x_rob1=2.*rr*cos((double)(Fi/3.));
          x_rob2=2.*rr*cos((double)(Fi/3.+Stala));
          x_rob3=2.*rr*cos((double)(Fi/3.+2.*Stala));
         }
    X_image[0]=0.;     X_image[1]=0.;    X_image[2]=0.;
   }                /* koniec dla pierwiastkow rzeczywistych  */
/*................pierwiastki zespolone...................................*/
  else
    {rr=sqrt((double)(delta));
     pp=-B[0]/2.+rr; ww=pp;  if(pp<0.) ww=-pp;
     C=Pot((double)ww,(double)(1./3.));  if(pp<0.) C=-C;
     qq=-B[0]/2.-rr; ww=qq;  if(qq<0.) ww=-qq;
     D=Pot((double)ww,(double)(1./3.));  if(qq<0.) D=-D;
     x_rob1=C + D;   x_rob2=-(C + D)/2.;   x_rob3=x_rob2;
     rr=sqrt((double)3.);   x_image=(double)((C-D)/2.)*rr;
     X_image[0]=0.;     X_image[1]=x_image;   X_image[2]=-x_image;
    }
K: rr=P/3.; X_real[0]=x_rob1-rr; X_real[1]=x_rob2-rr;  X_real[2]=x_rob3-rr;
   return;
 }
/*---5.--------------------------------------------------------------------*/
void ROOT4(double A[],int rang,double X_real[],double X_image[])
{int i;
 double P, Q, R, S, a, b, c, p2, p3, y, B[4], Z_real[4], Z_image[4];
  for(i=0;i<rang;i++)  {X_image[i]=0.; X_real[i]=0;}
  if(fabs(A[4]-1.)<EPS) {P=A[3];   Q=A[2];      R=A[1];      S=A[0];}
  else                  {P=A[3]/A[4]; Q=A[2]/A[4]; R=A[1]/A[4]; S=A[0]/A[4];}
  i=0;
  if(fabs(S)<EPS)
   {i++; if(fabs(R)<EPS) {i++; if(fabs(Q)<EPS)  {i++; if(fabs(P)<EPS) i++;}}}
  switch(i)
   {case 0:
     if(fabs(P)<EPS && fabs(Q)<EPS && fabs(R)<EPS)
      {if(S<0.)
        {a=-S; a=Pot((double)a,(double)(1./rang));
         X_real[0]=a; X_real[1]=a; X_real[2]=-a; X_real[3]=-a;
         for(i=0;i<rang;i++)  X_image[i]=0.;
        }
       else
        {a=S; a=Pot((double)a,(double)(1./rang));
         X_image[0]=a; X_image[1]=a; X_image[2]=-a; X_image[3]=-a;
         for(i=0;i<rang;i++)  X_real[i]=0.;
        }
       return;
      }
     break;
    case 1:  ROOT3(&A[1],X_real,X_image); return;
    case 2:  ROOT2(&A[2],X_real,X_image);   return;
    case 3:  X_real[3]=-A[3]/A[4]; return;
    default: break;
   }
  B[0]=S*(4.*Q-P*P)/8.-R*R/8.;  B[1]=(2.*P*R-8.*S)/8.;  B[2]=-Q/2.;  B[3]=1.;
  for(i=0;i<rang;i++)   {Z_real[i]=0.;  Z_image[i]=0.;}
  ROOT3(B,Z_real,Z_image);  y=Z_real[0];
  a=8.*y + P*P-4.*Q;
  if(a < EPS3) a=EPS3;
/*    {printf("\n Dla 4-tego stopnia roboczy pierws. jest ujemny  "); exit(0);} */
  a=sqrt((double)a);
  B[0]=y+(P*y - R)/a;  B[1]=(P+a)/2.;  B[2]=1.;
  for(i=0;i<rang;i++)   {Z_real[i]=0.;  Z_image[i]=0.;}
  ROOT2(B,Z_real,Z_image);
  X_real[0]=Z_real[0];    X_real[1]=Z_real[1];
  X_image[0]=Z_image[0];  X_image[1]=Z_image[1];

  B[0]=y-(P*y - R)/a;  B[1]=(P-a)/2.;  B[2]=1.;
  for(i=0;i<rang;i++)   {Z_real[i]=0.;  Z_image[i]=0.;}
  ROOT2(B,Z_real,Z_image);
  X_real[2]=Z_real[0];    X_real[3]=Z_real[1];
  X_image[2]=Z_image[0];  X_image[3]=Z_image[1];
  return;
 }
/*----6.-------------------POLTR------------------------------------------*/
int POLRT(double XCOF[],int MM,double ROOTR[],double ROOTI[],float tol)
 {int M, IFIT, NX, NXX, ERR, N2, KJ1, L, MT, IN, ICT, I, ITEMP, pp=0;
  double XO, YO, X, Y, XPR, YPR, Pwiel, UY, V, YT, XT, Ywiel, XT2, YT2,
         SUMSQ, DX, DY, TEMP, ALPHA, FI, XD, YD, COEF[RANG], A[RANG];
   if(MM < 0) {MM=-MM; pp=-1;}
   M=MM;  if(M > RANG) return(2);
   for(L=M;L>=0;L--) {if(fabs(XCOF[L]) > 1.e-10) break;}
   M=L;  NX=M;  NXX=M;  N2=0;  KJ1=M;  IFIT=0;   ERR=0;
   switch(M)
    {case 4: ROOT4(XCOF,M,ROOTR,ROOTI); return ERR=0;
     case 3: ROOT3(XCOF,ROOTR,ROOTI); return ERR=0;
     case 2: ROOT2(XCOF,ROOTR,ROOTI); return ERR=0;
     case 1: ROOTR[0]=-XCOF[0]/XCOF[1]; return ERR=0;
     default: break;
    }
   for(L=0;L<=M;L++)  COEF[L]=XCOF[M-L];

PP:if(pp < 0)  {XO=ROOTR[N2];  YO=ROOTI[N2];}
   else        {XO=.005;    YO=.01;}
   IN=0;

TT: X=XO;  Y=0.;  X=(X+XO)/2.;  Y=(Y+YO)/2.;
    XO=-10.*Y;    YO=-10.*X;   X=XO;   Y=YO;  IN=IN+1;
EV: for(ICT=0;ICT<ITER_MAX;ICT++)
      {Pwiel=0.0; YT=0.; UY=0.; V=0.; XT=1.0; Ywiel=COEF[M];
       if(Ywiel != 0.)
         {for(I=1;I<=M;I++)
           {L=M-I; TEMP=COEF[L];  XT2=X*XT-Y*YT;  YT2=X*YT+Y*XT;
            Ywiel=Ywiel+TEMP*XT2; V=V+TEMP*YT2;
            FI=I; Pwiel=Pwiel+FI*XT*TEMP;  XT=XT2;
            UY=UY-FI*YT*TEMP;              YT=YT2;
           }
          if(fabs(Ywiel) < tol) goto T;
          SUMSQ=Pwiel*Pwiel + UY*UY;
          if(fabs(SUMSQ) >= tol)
           {DX=(V*UY-Ywiel*Pwiel)/SUMSQ;    X+=DX;
            DY=-(Ywiel*UY+V*Pwiel)/SUMSQ;   Y+=DY;
            if((fabs)(DX)+(fabs)(DY) < tol)
             {
           T: for(L=0;L<=NXX;L++)
               {MT=KJ1-L; TEMP=XCOF[MT]; XCOF[MT]=COEF[L];
                COEF[L]=TEMP;
               }
              ITEMP=M; M=NX; NX=ITEMP;
              if(IFIT == 0) {IFIT=1; ERR=0;  XPR=X;  YPR=Y;  goto EV;}
           Z: IFIT=0;
              if(ERR == 0) {if((fabs)(Y)<1.e-5*(fabs)(X))  goto BR;
                            else {ALPHA=2.*X; SUMSQ=X*X+Y*Y;
                                  M=M-2;
                                  goto BX;}
                           }
              ERR=0;
              printf("\nBrak rozw.dla pierw. %d  x=%11.3g  DX=%11.3g  w.wiel=%13.5g",
                          N2,X,DX,Ywiel);
              goto BR;
             }
         }
        else
         {X=0.0; NX--; NXX--;      /* gdy A[0] jest rowne zeru  */
         BR:Y=0.0; SUMSQ=0.0;  ALPHA=X; M--;
         BX:COEF[1]=COEF[1]+ALPHA*COEF[0];
            for(L=1;L<M;L++)
              COEF[L+1]=COEF[L+1]+ALPHA*COEF[L]-SUMSQ*COEF[L-1];
         RR:ROOTR[N2]=X;
            ROOTI[N2]=Y; N2++;
            if(SUMSQ !=0.) {Y=-Y; SUMSQ=0.0; goto RR;}
            else           {if(M > 4) goto PP;
                            else
                              {for(L=0;L<=M;L++)  A[L]=COEF[M-L];
                               if(M==4)   ROOT4(A,M,&ROOTR[N2],&ROOTI[N2]);
                               if(M==3)   ROOT3(A,&ROOTR[N2],&ROOTI[N2]);
                               return(ERR=0);}
                           }
           }
          if(IFIT !=0) goto T;
         }
      } /* end of for()  dla ICT  */
   if(IN < L_START) goto TT;
   else  return(ERR=3);
/*  return(ERR); */
 }

/*------------------------------------------------------------------------*/
void bledy(int ERR)
{int i;
  switch(ERR)
    {case 0: break;
     case 1: printf("\nM LESS THAN ONE"); break;
     case 2: printf("\nM GREATER THAN %d",RANG); break;
     case 3: printf("\nUNABLE TO DETERMINE ROOT WITH %d INTERATIONS ON %d STARTING VALUES",
                       ITER_MAX,L_START); break;
     case 4: printf("\nHIGH ORDER COEFFICIENT IS ZERO");
    }
 return;
}

/*----9.------------------------------------------------------------------*/
double Pow(double x,int m)
 {int i;
  double y;
  y=x;
  for(i=2;i<=m;i++) y*=x;
  return y;
 }
/*----9.------------------------------------------------------------------*/
double Pot(double x, double p)
 {double y;
  if(x<=0) x=1.e-20;
  y=exp(p*log(x));
  return y;
 }
/*----10.----------------------------------------------------------------*/
double F(double x[],double u,double A[],double B,char rzad)
{double y;
 char i;
  y=B*u;
  for(i=0;i<rzad;i++) y+=(A[i]*x[i]);
 return y;
}
/*-----11.----------------------------------------------------------------*/
void RK(double x[],double u,double A[RANG][RANG],double B[],double ht,char rzad)
{double yp0[RANG],y1[RANG],yp1[RANG],y2[RANG],yp2[RANG],y3[RANG],yp3;
 char i;
  for(i=0;i<rzad;i++)
   {yp0[i]=F(x,u,A[i],B[i],rzad);  y1[i]=x[i]+0.5*ht*yp0[i];}
  for(i=0;i<rzad;i++)
   {yp1[i]=F(y1,u,A[i],B[i],rzad);  y2[i]=x[i]+0.5*ht*yp1[i];}
  for(i=0;i<rzad;i++)
   {yp2[i]=F(y2,u,A[i],B[i],rzad);  y3[i]=x[i]+ht*yp2[i];}
  for(i=0;i<rzad;i++)
   {yp3=F(y3,u,A[i],B[i],rzad);
    x[i]=x[i]+ht*((yp0[i]/6.0)+(yp1[i]/3.0)+(yp2[i]/3.0)+(yp3/6.0));
   }
  return;
 }
/*------------------------------MLIB.C-------------------------------------*/


struct szuk
 {char znak;
  char pole;
  shint text;
  shint liczby;
  char wzor_t[DLN-1];
  int wzor_l;
 }Sz;

int sprawdz_text(char *wzor,char *text); /*sprawdza czy wzor == text
              z uwzglednieniem '*' we wzor, i zwraca 1 jesli tak, lub -1 jesli nie */

/**************/

#define PRZESZUKAJ_T(co)\
 for(i=0;i<MAX && studenci[i]!=NULL;i++)\
  {if( sprawdz_text(wzor,co) > 0)\
    {tb[j]=i;\
     j++;\
    }\
  }

shint *przeszukaj_t(char co,char *wzor)
 {
	
	shint *tb;
  shint i=0,j=0;

  tb=malloc(MAX*sizeof(shint));

  switch(co)
   {
    case 'i':
      PRZESZUKAJ_T( Imiona[studenci[i]->pac_imie] )
      break;
    case 'n':
      PRZESZUKAJ_T( Nazwiska[studenci[i]->pac_nazwisko] )
      break;
   }

  tb[j]=-2;
  
  return tb;
 }

/*************/
#define PRZESZUKAJ_L(co,jak)\
 for(i=0;i<MAX && studenci[i]!=NULL;i++)\
  {if(studenci[i]->co jak wzor)\
    {tb[j]=i;\
     j++;\
    }\
  }


shint *przeszukaj_l(char co,char jak,double wzor)
 {

	shint *tb;
  shint i=0,j=0;

  tb=malloc(MAX*sizeof(shint));

  switch(co)
   {
    case 'x':
      switch (jak)
       {case '=':
          PRZESZUKAJ_L(pac_nr_ew,==)
          break;
        case '>':
          PRZESZUKAJ_L(pac_nr_ew,>)
          break;
        case '<':
          PRZESZUKAJ_L(pac_nr_ew,<)
          break;
	   }
      break;
    case 's':
      switch (jak)
       {case '=':
          PRZESZUKAJ_L(pac_nr_kasy,==)
          break;
        case '>':
          PRZESZUKAJ_L(pac_nr_kasy,>)
          break;
        case '<':
          PRZESZUKAJ_L(pac_nr_kasy,<)
          break;
	   }
      break;
   }

  tb[j]=-2;
  
  return tb;
 }

/****************/

int sprawdz_text(char *wzor,char *text)
 {int i=0,j=0,M=strlen(wzor),N=strlen(text),pocz=0,kon=0;
  char tmp[20];

  strcpy(tmp,wzor);
  if(tmp[0]=='*')
   {pocz=1;
    M--;
    strcpy(tmp,tmp+1);
   }
  if(tmp[M-1]=='*')
   {kon=1;
    tmp[M-1]='\0';
    M--;
   }

  while(j<M && i<N)
   {if(text[i]!=tmp[j])
     {if(pocz==1)
       {i-=j;
        j=-1;
       }
      else {break;}
     }
    i++;
    j++;
   }
  if(j==M && i==N || kon && j==M)
   {return 1;
   }
  else {return -1;}
 }

/******************/

shint show(shint *tb)
 {
  shint i,opcja=0;
  char *texty[MAX],*temp;
  char *t_show=" Nazwisko:       Imie:           Nr ewidencyjny:";

  if(tb[0]==-1)
  {for(i=0;i<MAX;i++){tb[i]=i;}
  }
  for(i=0;i<MAX && studenci[tb[i]]!=NULL && tb[i]!=-2;i++)
   {temp=(char*)malloc(44);
    sprintf(temp,"%-20s %-15s %4d",Nazwiska[studenci[tb[i]]->pac_nazwisko],Imiona[studenci[tb[i]]->pac_imie],studenci[tb[i]]->pac_nr_ew);
    texty[i]=temp;
   }
  opcja=okno_menu(texty,i,1,attr_baz,attr0_baz,5,3,-1,t_show,1);

 return opcja;
 }
/******************/
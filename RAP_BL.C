/* ======================================================================= */
int raporty(signed char Kod_uslugi, int Nr_rekordu, int npzl,
				int kto, int r_pocz, int lrek, struct Service *S)
 {int ret, max_l_data_blank;       /*liczba danych obslugiwana przez bl */
	int y_gorny, x_lewy=0, y_danej, y_dolny, x_prawy;
	int decyzja,  kod_decyzji, nr_dec, nr_rekordu, p_konc, i;
	int old_raport, old_rekord, j, np=0;
	char czynny=1, zapis=0, poziom=0;
	signed char kod_uslugi, kolor=0, ramka=1;
/* ==================================================================== */
	kod_uslugi=Kod_uslugi;
	nr_rekordu=Nr_rekordu;
	y_gorny=Y_G0+1;  npzl=npzl;  x_lewy=X_L0;
	y_dolny=MY_MAX;  x_prawy=MX_MAX-1;
	do
	 {old_raport=kod_uslugi;  old_rekord=nr_rekordu;
		ret=blankiet(&kod_uslugi, &nr_rekordu, x_lewy, y_gorny+y_shift,
								 y_dolny, x_prawy, czynny, kolor, ramka,
								 &poziom, &zapis, &y_danej);
		if(kod_uslugi>=0)
		 {for(i=0;i<=L_SYS;i++)
			 {if(Serv[i]->kod_uslugi==kod_uslugi) {S=Serv[i]; break;}}
		 }
		Service=S;
		if(ret==UNDO) {continue;}
		freeAgendaPtr(); freeRepDataPtr(NULL);
		if(ret<=-1) {continue;}
		if(ret==Esc) {break;}
		if(ret==FONT6)
		 {decyzja=jaka_decyzja(&kod_decyzji, &nr_dec);
			old_raport=kod_uslugi;
			kod_uslugi=S->decyzje(decyzja, kod_decyzji, nr_dec, kod_uslugi,
														old_rekord, &nr_rekordu);
			if(kod_uslugi<0)
			 {int Ret=old_raport;
				Ret=okno_menu(Menu,Liczba_opcji,Ret,attr, at_wpis, 10,25,-1," LISTA PODSYSTEMOW - WYBIERZ ",1);
				if(Ret<0) {kod_uslugi=old_raport;}
				else
				 {kod_uslugi=Ret+1;
					if(kod_uslugi!=old_raport) nr_rekordu=0;
				 }
			 }
		 }
	}while(ret>0);
 return ret;
}

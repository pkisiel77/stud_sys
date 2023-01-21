public dostawca1
sl1="          "
nazwa1=repli(" ",30)
jedn1=" "
ilosc1=0
cena1=0
nowy="T"
tn="N"
sele 1
use stany index stany1
sele 2
use obroty index obroty1,obroty2
sele 3
use magazyny
max_mag=lastrec()
sele 4
use kontra
max_dos=lastrec()
declare menucena[2],menumag[max_mag],menudos[max_dos]
sele 5
use operacja
go top
zap
clear screen
@  0,  0 To 24, 79 Double

sele 4
i=1
do while .not. eof()
   menudos[i]=nazwa
   i=i+1
   skip
enddo

@ 3,9 to 11,31 double
@ 3,11 say "Wybierz odbiorce : "
dos=achoice(4,10,10,30,menudos)
go dos
dostawca1=sl
@ 3,9 clear to 11,31
@ 1,5 say "Odbiorca : " +dostawca1
sele 5
@  14,0 to 24,46 double
@  14,10 say "Wydanie towaru "
@  16,50 say "INS - dodaj towar"
@  17,50 say "DEL - usun towar"
@  18,50 say "SPACJA - popraw"
@  19,50 say "ENTER - zatwierdz"
dbedit(15,1,23,45,,"FUNK1")



restore screen from ekran1

return 




Function FUNK1()

   Save Screen To ekran2
   f= LastKey()
Do Case
   Case f = 7
      kasuj1()
   Case f = 22
      dodaj1()
   Case f = 9
      popraw1()
   Case f = 27
      return 0
   Case f = 6
      zapisz1()
      Return 0
EndCase
 

Procedure kasuj1
setcursor(1)
   a= RecNo()
   If (LastRec() == 0)
      tone(60, 3)
      Restore Screen From ekran2
   Else
      @ 10, 30 Clear To 16, 60
      @ 10, 30 To 16, 60 Double
      @ 12, 32 Say "Czy na pewno skasowac ? "
      @ 14, 35 Prompt "KASUJ"
      @ 14, 45 Prompt "NIE KASUJ"
      menu1= 2
      Menu To menu1
      If (menu1 = 1)
         Delete
      ElseIf (menu1 = 2)
      EndIf
      Pack
      close data
      Use bibl index bibl1,bibl2,bibl3
      Restore Screen From ekran2
      Return
   EndIf

********************************
Procedure dodaj1
setcursor(1)
@ 3,1 clear to 13,74
sl1="          "
nazwa1=repli(" ",30)
jedn1=" "
ilosc1=0
cena1=0
nowy="T"
tn="N"

sele 1
use stany index stany1
sele 2
use obroty index obroty1,obroty2
sele 3
use magazyny
max_mag=lastrec()
sele 4
use kontra
max_dos=lastrec()
declare menucena[2],menumag[max_mag],menudos[max_dos]
sele 5
use operacja
do while .t.
sele 1

@  5,5  say "Podaj symbol towaru : " get sl1
read
if lastkey()=27
exit
endif
sele 3
go top
i=1
  do while .not. eof()
     menumag[i]=nazwa
     skip
  i=i+1
  enddo
@ 3,49 to 11,71 double
@  3,53  say "Wybierz magazyn : "
mag=achoice(4,50,10,70,menumag)
@ 4,50 clear to 10,70

go mag
magazyn1=sl
sele 1
  locate for sl=sl1 .and. sl_magaz=magazyn1
  if found()=.T.
     nowy="N"
     nazwa1=nazwa
     jedn1=jedn
     cena1=cena_det
  else 
     @ 7,5 say "BRAK TOWARU O PODANYM SYMBOLU !!!"
     loop
  endif
@ 7,1 clear to 10,38
menucena[1]=str(cena_hurt)
menucena[2]=str(cena_det)


@  6,5  say "Nazwa towaru  : "+ nazwa1
@  7,5  say "Jednostka     : "+ jedn1
@  8,5  say "Magazyn       : "+ magazyn1
@ 3,49 to 11,71 double
@  3,53  say "Podaj cene    : "
cen=achoice(4,50,10,70, menucena)
@ 4,50 clear to 10,70

Do Case
    Case cen=1
         cena1=cena_hurt
    Case cen=2
         cena2=cena_det
EndCase


@ 9,5 say "Cena           :" + str(cena1)
do while .T.
  @ 10,5 say "Podaj ilosc : " get ilosc1
  read
  sele 1
  seek sl1
     if ilosc1>ilosc
     @ 11,5 say "NIE MA TYLE TOWARU W MAGAZYNIE !!!"
     else
     exit
     endif
enddo

sele 5
append blank
replace co with sl1
replace ile with ilosc1
replace cena with cena1
replace magazyn with magazyn1
setcursor(0)
exit
enddo





********************************
Procedure popraw1


********************************
Procedure Zapisz1
close data

sele 1
use stany index stany1
sele 2
use obroty index obroty1,obroty2
sele 3
use magazyny
sele 4
use kontra
sele 5
use operacja

do while .not. eof()
towar=co
  sele 1
  seek towar
  replace ilosc with ilosc-(5->ile)

  sele 2
    append blank
    replace data with date()  
    replace operacja with "W"
    replace komu with dostawca1
    replace co with 5->co  
    replace ile with 5->ile
    replace cena with 5->cena
    replace magazyn with 5->magazyn
  sele 5
  skip
enddo


set device to print
set printer to "potw.txt"
sele 5
go top
wart=0
@ prow()+1,1 say "POTWIERDZENIE WYDANIE TOWARU"
@ prow()+2,1 say "Odbiorca : " + 4->nazwa
@ prow()+1,1 say repli("=",60)
@ prow()+1,1 say "|Nazwa towaru      |Ilosc  |Cena   |Wartosc    |Magazyn    |"
@ prow()+1,1 say repli("=",60)
do while .not. eof()
@ prow()+1,2 say co
@ prow(),20 say "|"
@ prow(),21 say ile
@ prow(),28 say "|"
@ prow(),29 say alltrim(str(cena))
@ prow(),36 say "|"
@ prow(),37 say alltrim(str(cena*ile))
wart=wart+(cena*ile)
@ prow(),48 say "|"
@ prow(),49 say magazyn
@ prow(),60 say "|"
skip
enddo
@ prow()+1,1 say repli("=",60)
@ prow()+1,30 say "Razem wartosc : "+str(wart)

set device to screen
set printer to

readtext("potw")

return

sele 1
use obroty index obroty1,obroty2
sele 2
use kontra
max_dos=lastrec()
declare menudos[max_dos]
wart=0
i=1
do while .not. eof()
   menudos[i]=nazwa
   i=i+1
   skip
enddo
cls
@ 0,0 to 24,76 double
@ 3,9 to 11,31 double
@ 3,11 say "Wybierz kontrahenta : "
dos=achoice(4,10,10,30,menudos)
go dos
dostawca1=sl
dostawca2=nazwa

set device to print
set printer to "wydruk.txt"
sele 1
@ prow()+1,1 say "Zestawienie zbiorcze obrotow dla wybranego kontrahenta"
@ prow()+1,1 say "Kontrahent : "+dostawca2
@ prow()+2,1 say repli("Í",60)
@ prow()+1,1 say "ºData    ºOperacja  ºSl towaru ºIloscºCena      ºWartosc   º"
@ prow()+1,1 say repli("Í",60)
set filter to komu=dostawca1
go top
do while .not. eof()
@ prow()+1,2 say data

if operacja="P" 
oper="Przyjecie"
else
oper="Wydanie"
endif

@ prow(),10 say "º"+oper
@ prow(),21 say "º"+co
@ prow(),32 say "º"+alltrim(str(ile))
@ prow(),38 say "º"+alltrim(str(cena))
@ prow(),49 say "º"+alltrim(str(cena*ile))
wart=wart+(cena*ile)
skip
enddo
@ prow()+1,1 say repli("Í",60)
@ prow()+1,30 say "Razem wartosc : "+alltrim(str(wart))





set device to screen
set printer to

readtext("wydruk")
restore screen from ekran2
return

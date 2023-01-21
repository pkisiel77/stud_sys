sele 1
use obroty index obroty1,obroty2
sele 2
use stany index stany1
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
@ 3,11 say "Wybierz towar : "
dos=achoice(4,10,10,30,menudos)
go dos
towar1=sl
towar2=nazwa
stan=ilosc
stan1=0
stan2=0
set device to print
set printer to "wydruk.txt"
sele 1
@ prow()+1,1 say "Zestawienie zbiorcze obrotow dla wybranego kontrahenta"
@ prow()+1,1 say "Towar : "+towar2
@ prow()+1,1 say "Przyjecia towaru"
@ prow()+2,1 say repli("Í",60)
@ prow()+1,1 say "ºData    ºOperacja  ºSl kontr ºIloscºCena      ºWartosc   º"
@ prow()+1,1 say repli("Í",60)
set filter to co=towar1 .and. operacja="P"
go top
do while .not. eof()
@ prow()+1,2 say data

if operacja="P" 
oper="Przyjecie"
else
oper="Wydanie"
endif

@ prow(),10 say "º"+oper
@ prow(),21 say "º"+komu
@ prow(),32 say "º"+alltrim(str(ile))
@ prow(),38 say "º"+alltrim(str(cena))
@ prow(),49 say "º"+alltrim(str(cena*ile))
stan1=stan1+ile
skip
enddo
@ prow()+1,1 say repli("Í",60)
@ prow()+1,30 say "Razem przyjeto towaru : "+alltrim(str(stan1))
****************************

@ prow()+1,1 say "Wydania towaru"
@ prow()+2,1 say repli("Í",60)
@ prow()+1,1 say "ºData    ºOperacja  ºSl kontr ºIloscºCena      ºWartosc   º"
@ prow()+1,1 say repli("Í",60)
set filter to co=towar1 .and. operacja="W"
go top
do while .not. eof()
@ prow()+1,2 say data

if operacja="P" 
oper="Przyjecie"
else
oper="Wydanie"
endif

@ prow(),10 say "º"+oper
@ prow(),21 say "º"+komu
@ prow(),32 say "º"+alltrim(str(ile))
@ prow(),38 say "º"+alltrim(str(cena))
@ prow(),49 say "º"+alltrim(str(cena*ile))
stan2=stan2+ile
skip
enddo
@ prow()+1,1 say repli("Í",60)
@ prow()+1,30 say "Razem wydano towaru : "+alltrim(str(stan2))

@ prow()+1,1 say "Stan magzynu powinien wynosic (przyjecia-wydania) : "+alltrim(str(stan1-stan2))
@ prow()+1,1 say "Rzeczywisty stan wynosi : "+alltrim(str(stan))


set device to screen
set printer to

readtext("wydruk")
restore screen from ekran2
return

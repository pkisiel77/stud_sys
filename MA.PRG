set procedure to readtxt

   Set Date German
   Set Wrap On
   Clear Screen
   @  0,  0 To 24, 79 Double
   @  1,  1 Say  "         autor:     PC-UNIT, ul. Tarnopolska 9, 48-300 NYSA"
   @  2,  1 Say  "                       PROGRAM KOMPUTEROWY - MAGAZYNY"
   @  3,  1 Say  "                         Uzytkownik:   RSP WILAMOWA"

   bibl= 0
   Do While .T.
      Close Databases
      @  8,  4 To 17, 23
      @  9,  5 Prompt "Przyjecie towaru  "
      @  10,  5 Prompt "Wydanie towaru    "
      @  11,  5 Prompt "Wydruki           "
      @  12,  5 Prompt "Edycja katalogow  "
      @  13,  5 Prompt "Edycja towarow    "
      @  14,  5 Prompt "Przegl. obrotow   "
      @  15,  5 Prompt "Indeksowanie      "
      @  16,  5 Prompt "Wyjscie           "
      Menu To bibl
      Save Screen To ekran1
      Do Case
      Case bibl = 1
         do ma1
      Case bibl = 2
         do ma2
      Case bibl = 3
         do ma3
      Case bibl = 4
         do ma4
      Case bibl = 5
         ma5()
      Case bibl = 6
         ma6() 
      Case bibl = 7
         indeks()
      Case bibl = 8
         quit
      Case bibl = 0
         Quit
      EndCase
   EndDo


*********************
Procedure ma5
clear screen
close data
use stany index stany1
browse(1,1,24,79)
restore screen from ekran1
return

*********************
Procedure ma6
clear screen
close data
use obroty index obroty1,obroty2
@ 0,0 to 24,76 double
dbedit(1,1,23,75)
restore screen from ekran1
return

************************
Procedure indeks()
use stany
index on sl to stany1


use obroty
index on komu to obroty1
index on co to obroty2

   kat= 1
   Do While .T.
      Close Databases
      @  12,  15 To 15, 38
      @  13,  16 Prompt "Katalog magazynow     "
      @  14,  16 Prompt "Katalog kontrahentow  "
      Menu To mag
      Save Screen To ekran3
      Do Case
      Case mag = 1
         ma4a()
      Case mag = 2
         ma4b()
      Case mag = 0
         restore screen from ekran1
         return
      EndCase
   EndDo


procedure ma4a
cls
use magazyny
browse()

restore screen from ekran3

procedure ma4b
cls
use kontra
browse()

restore screen from ekran3



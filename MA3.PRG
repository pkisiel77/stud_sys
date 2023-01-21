
   mag= 1
   Do While .T.
      Close Databases
      @  11,  15 To 14, 47
      @  12,  16 Prompt "Zestaw. zbiorcze - kontrahenci "
      @  13,  16 Prompt "Zestaw. zbiorcze - towary      "
      Menu To mag
      Save Screen To ekran2
      Do Case
      Case mag = 1
         do ma3a
      Case mag = 2
         do ma3b
      Case mag = 0
         restore screen from ekran1
         return
      EndCase
   EndDo



# This program tests the compare modes set by sco_*
start:  li 255
        mov GRA GRB
        sco_xoreq
        cmp GRA GRB
        jmp next0
        jmp loop
next0:  li 0
        cmp GRA GRB
        jmp loop
        jmp next1
next1:  sco_andeq
        cmp GRA GRB
        jmp next2
        jmp loop
next2:  jmp start
loop:   jmp loop
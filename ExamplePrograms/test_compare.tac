# This program tests the compare modes set by sco_*
# If we end up jumping to the loop tag, we've failed the test
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
next2:  sco_gthan
        cmp GRA GRB
        jmp next3
        jmp loop
next3:  sco_oflow
        cin GRA GRA
        cmp GRA GRB
loop:   jmp loop
        jmp start
start:  li 255
        mov GRA GRB
        sco_xoreq
        cmp GRA GRB
        jmp start
loop:   jmp loop
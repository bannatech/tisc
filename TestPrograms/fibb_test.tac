start:  li 0
        mov GRA GRB
        li 255
        sp GRA
        li 1
        mov GRA GRC
        li 144
        sb GRC
loop:   add GRB GRC GRB
        push
        mov GRB GRA
        push
        mov GRC GRB
        pop
        mov GRA GRC
        pop
        sb GRC
        cmp GRB GRA
        jmp loop
end:    jmp end
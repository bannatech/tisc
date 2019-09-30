start:  li 144
        mov GRA GRC
        li 255
        sp GRA
        mov GRC GRA
        mov NUL GRB
        cin GRB GRC
loop:   add GRB GRC GRB
        push
        mov GRB GRA
        push
        mov GRC GRB
        pop
        mov GRA GRC
        pop
        sb GRC
        cmp GRA GRB
        jmp loop
end:    jmp end
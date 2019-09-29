start:  li 144 
        mov GRA GRC
        li 255
        sp GRA
        mov GRC GRA
        mov NUL GRB
        cin GRB GRC
loop:   sb GRC
        add GRB GRC GRB
        push
        mov GRB GRA
        push
        mov GRC GRB
        pop
        mov GRA GRC
        pop
        cmp GRA GRB
        jmp loop
        sb GRC
end:    jmp end
# This program computes the classic Fibbonacci sequence
###################### Clear GRA and GRB
start:  lli 0
        mov GRA GRB
###################### Set our memory pointer
        li 254
        sp GRA
###################### Set our inital state
        lli 1
        mov GRA GRC
###################### Printout current state
loop:   sb GRB
###################### Swap GRB and GRC
        sop_xor
        op GRB GRC GRC
        op GRB GRC GRB 
        op GRB GRC GRC
###################### Add GRB and GRC
        sop_add
        op GRB GRC GRB
###################### Test if we've reached the limit if GRA+GRB overflows 
        cmp GRB GRC
        jmp loop
###################### Idle around forever
        sb GRB
end:    jmp end
# fibb_test - This program computes the classic Fibbonacci sequence
###################### Set our memory pointer
        li 0xFD
        sp GRA
###################### Set our inital state
        one GRC
###################### Printout current state
loop:   sb GRB
###################### Swap GRB and GRC
        xor GRB GRC
        xor GRC GRC
        xor GRB GRC
###################### Add GRB and GRC
        op GRB GRC GRB
###################### Test if we've reached the limit if GRA+GRB overflows
        cmp GRB GRC
        jmp loop
###################### Idle around forever
        sb GRB
end:    jmp end

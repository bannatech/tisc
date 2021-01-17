# This program tests the compare modes set by sop_*
# If we end up jumping to the loop tag, we've failed the test
start:  li 255
        mov GRA GRB
        sop_xor
        cmp GRA GRB
        jmp next0
        jmp loop
next0:  li 0
        cmp GRA GRB
        jmp loop
        jmp next1
next1:  sop_and
        cmp GRA GRB
        jmp next2
        jmp loop
next2:  sop_sub
        cmp GRA GRB
        jmp next3
        jmp loop
next3:  sop_add
        cin GRA GRA
        cmp GRA GRB
loop:   jmp loop
        jmp start
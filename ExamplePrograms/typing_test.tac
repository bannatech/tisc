# This program reads the input from the 'keyboard' component and records in a
# buffer until the user presses the return key, at that point it dumps the
# buffer. The buffer will step back when the backspace key is pressed.
setup:  sco_xoreq
        li 32
        mov GRA GRC
poll:   li 252
        sp GRA
        li 128
p_loop: lb GRB
        cmp GRB GRA
        jmp p_loop 
buffer: sb GRB
        sb GRA
        li 8
        cmp GRA GRB
        jmp bcksp
        sp GRC
        sb GRB
        cin GRC GRC
        li 112
        cmp GRA GRC
        jmp setup
        li 10
        cmp GRB GRA
        jmp output
        jmp poll
bcksp:  li 32
        cmp GRA GRC
        jmp poll
        li 255
        add GRA GRC GRC
        jmp poll
output: li 32
o_loop: sp GRA
        lb GRB
        push
        li 252
        sp GRA
        sb GRB
        pop
        cin GRA GRA
        cmp GRA GRC
        jmp setup
        jmp o_loop

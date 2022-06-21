# This program is a fun test that will write the address of the jmp statement
# on line number 15 if we're running this bit of code from the internal RAM
# It can only run from the internal RAM as it doesn't write out to the Program
# Memory, only the Internal RAM which can also execute code if set up correctly
start:  nop
        nop
        nop
        nop
        nop
        nop
        pcr
        lli 5
        mov GRA GRC
        pop
        op GRA GRC GRA
        cin GRA GRB
        sp GRB
        sb GRA
        jmp start

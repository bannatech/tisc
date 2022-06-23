# weird_loop_test - exemplifies the ability to modify executing memory when the
#                   program is running from the internal RAM instead of an
#                   external ROM. The program will calculate two addresses that
#                   are used to write out to memory, overwriting the jmp address
start: jmp weird
loop:  jmp loop
# offset this bit of code
       segment 0x42
# push the return address
weird: pcr
# calculate the memory address, and the jump address we will use
       lli 5
       mov GRA GRC
# these instructions calculate the jump address
       pop
       op GRA GRC GRA
# this instruction calculates the memory address (+1 from the jump address)
       cin GRA GRB
# write out to memory now that we know our memory address + jump address
       sp GRB
       sb GRA
# after the last two instructions, this instruction will jump to itself.
       jmp loop

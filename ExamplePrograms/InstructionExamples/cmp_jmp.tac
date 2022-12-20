# sop_*, cmp A<GR> B<GR>, jmp A<label>
#
### Overview
#
# The set of sop_* commands configures the operation mode register, which the
# cmp instruction uses to generate a flag. The jmp instruction will not perform
# the jmp if the flag is set to true.
#
# If we end up jumping to the loop tag, we've failed the test
        # Set GRA and GRB to 255
start:  li 255
        mov GRA GRB
        # Test that GRA ^ GRB == 0,
        # The cmp instruction generates a flag if the result is nonzero for xor
        sop_xor
        cmp GRA GRB
        jmp next0 # This jump is executed, because the flag is false
        jmp loop
        # Set GRA to 0
next0:  li 0
        # Test that GRA ^ GRB != 0
        cmp GRA GRB
        jmp loop # This jump is not executed, because the flag is true
        jmp next1
        # Test that GRA & GRB == 0
next1:  sop_and
        cmp GRA GRB
        jmp next2 # This jump is executed, because the flag is false
        jmp loop
        # Test that GRA - GRB < 0
        # The flag is not set because the result of 0 - 255 is negative
next2:  sop_sub
        cmp GRA GRB
        jmp next3 # This jump is executed, because the flag is false
        jmp loop
        # Set GRA to 1 and test that GRA + GRB > 255
        # The flag is set because the result of 255 + 1 > 255
next3:  one GRA
        sop_add
        cmp GRA GRB
loop:   jmp loop
        jmp start
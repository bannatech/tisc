# sop_*, op
#
### Overview
#
# The set of sop_* commands configures the operation mode register, which the
# op and cmp instructions use. cmp is a mode of op which does not store a result
#
# Load some constants into the registers, GRA = 4, GRB = 2
li 0x02
mov GRA GRB
li 0x04
# Add GRA and GRB, store in GRC
sop_add
op GRA GRB GRC
# Now set the operation to subtraction
sop_sub
# Subtract GRA from GRC and store in GRA
op GRC GRA GRA
done: jmp done

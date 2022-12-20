# sp A<GR>, lb A<GR>, sb A<GR>, sps, lbs, and sbs
#
### Overview
#
# The sp, lb, and sb instructions help to point to, load, and store words in
# memory, respectively.
# sps, lbs, and lbs are instructions which use the stack for the source of the
# value instead of a general purpose register.
#
# Start out by setting our memory pointer to a random spot in memory, e.g. 23
li 23
sp GRA # Sets the memory pointer to 23
one GRA
sb GRA # Store 1 at the memory pointer
lb GRB # Read what we just wrote, but in a different regsiter
# This operation can also be done using the stack, by pushing the value, and the
# address to the stack and then using the sps and sbs instructions.
lni 1,24
sps
sbs
done: jmp done

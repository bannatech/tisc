# seg
#
### Overview
#
# The seg instruction sets the internal memory segment pointer. This allows the
# addressable memory space to be 16 bits, instead of just 8 bits.
#
# Sets our internal memory pointer to effectively 0x0123
lni 0x23,0x01
seg # Seg pops from the stack to set the segment address
sps
one GRB
# Set 0x0123 = 1
sb GRB
# Set our memory pointer to 0x0142, as the segment has not changed
li 0x42
sp GRA
# Set 0x0142 = 1
sb GRB
# Set the segment to 0x13, making the address we're pointing to 0x1342
li 0x13
push
seg
# Set 0x1342 = 1
sb GRB
done: jmp done

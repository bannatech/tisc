# ptradd A<GR>
#
### Overview
#
# ptradd performs addition with the contents of the specified register A and the
# contents of GRA. The result of the addition is used to set the memory pointer
# address, as well as writing the result to the specified register A
#
# ptradd will generate a flag for use with the jmp instruction. The flag is set
# when the addition of the specified register A and GRA produces an overflow.
#
################################################################################
#
### Example 1: Write operations based off of offsets
#
# We will write 1 (true) to a string of offsets on a base address.
# The base address is also written to.
#
# i.e.
#    *base_var = 1
#    foreach x in [5, 10, 5] {
#      base_var += x
#      *base_var = 1
#    }
#
# The last two bytes of the immediate are the first off the stack, which is
# the segment and the base address to write to.
#
# The next bytes to come off the stack are offsets that we will add to the base
# address until we reach `0xFF` or an overflow, whichever comes first.
#
# The 0xFF byte terminates our loop because adding 0xFF to any address will
# result in an overflow. Each offset is accumulative.
#
lni 0xff,0x05,0x0A,0x05,0x10,0x02 # load the segment, base address, and offsets
seg          # sets the memory segment to 0x02
pop          # pop our base address which is 0x10
zero GRC     # ensure that we aren't adding garbage
ptradd GRC   # set the memory pointer to our base address and store in GRC
one GRB      # the value to write at each point in memory (1)
loop: sb GRB # store the value at the current address
pop          # pop the next offset to add to the copy of our address in GRC
ptradd GRC   # flag is set in this instruction if GRA+GRC produces an overflow
jmp loop     # if the flag is unset, continue looping.
#
## Example 2: Elaborated write operations with a base address and offsets
#
# In Example 1, the value we wrote was a simple (1).
# In this example, we are writing two byte words into memory at a base address,
# and then at series of accumulative offsets.
#
# i.e.
#   type OffsetValue {
#     offset uint8
#     value  uint16
#   }
#   *base_addr = 0xA3B4
#   writes = [
#       OffsetValue{5,  0xA2B2},
#       OffsetValue{10, 0xA1B1},
#       OffsetValue{5,  0xA0B0},
#   ]
#   foreach i in writes {
#     base_addr += i.offset
#     *base_addr = i.value
#   }
# )
#
lni 0xff,0xA0,0xB0,0x05,0xA1,0xB1,0x0A,0xA2,0xB2,0x05,0xA3,0xB3,0xA0,0x01
seg
pop
zero GRC
ptradd GRC
mnb_store
mnb_incr
loop2: mnb 2
pop
ptradd GRC
jmp loop2
done: jmp done

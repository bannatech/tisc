# mnb_*, mnb A<1-15>
#
### Overview
#
# mnb 'move N bytes' is a way to move multiple words at once to and from the stack.
#
# The number of words is passed into arguement A and is the number of words to pop
# or push to/from the stack to a memory address pointed to by the memory pointer.
#
# The mode of mnb is set through the mnb_* instructions. The flow of values is set
# through the mnb_load and mnb_store instructions which pop from and push to the
# stack from the memory pointer, respectively.
#
# Incrementing the memory pointer for each byte read or written is set by mnb_incr.
#
# Decrementing the memory pointer is set by mnb_decr.
#
# The default is mnb_noincr, which does not increment the memory pointer. This is
# most useful for writing to memory mapped addresses, where you want to load more
# than one byte into the externally mapped address.
#
### Efficient Copy
#
# This sequence is in the reverse order it will appear in memory, once written
# this is due to the first byte in the sequence being popped last, effectively
# reversing the order. If you are representing values in little endian, then the
# values will be big endian for lni instructions.
lni 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11
# Load a string of 16 bytes and write to address 0x40
li 0x40
# Set the memory pointer by adding GRA+GRB (0x40+0), and store in GRB
ptradd GRB
# We're writing, and we're incrementing our memory pointer after each write.
mnb_store
mnb_incr
# Moves 16 bytes
mnb 16
# This is the offset that will point to the last byte
li 15
# GRB has the original pointer 0x40, we add 15 to get to 0x4F, the last byte
ptradd GRB
# Now we load in reverse, so that the endianness in memory is preserved
mnb_load
mnb_decr
mnb 16
# Write what we read into address 0x50
li 0x50
sp GRA
mnb_store
mnb_incr
mnb 16
done: jmp done

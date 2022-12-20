# The AU "Arithmetic Unit" is a component which can do arithmetic on 32 bit ints
# 
### Addition
# load the A Value to stack, little endian
lni 0x01,0x00,0x00,0x00
# load the B Value to stack, little endian
lni 0xFF,0xFF,0xFF,0x7F
# load the arithmetic operation to the stack (addition)
li 0x01
push
# call the function that loads the values + operation from the stack, and reads
pcr
jmp write_au
# write out the stack to memory
li 0x00
sp GRA
mnb_store
mnb_incr
mnb 4

### Subtraction
# load the A Value to stack, little endian
lni 0x00,0x00,0x00,0x00
# load the B Value to stack, little endian
lni 0x01,0x00,0x00,0x00
# load the arithmetic operation to the stack (subtraction)
li 0x02
push
# call the function that loads the values + operation from the stack, and reads
pcr
jmp write_au
# write out the stack to memory
li 0x10
sp GRA
mnb_store
mnb_incr
mnb 4

### Multiplication
# load the A Value to stack, little endian
lni 0x0A,0x00,0x00,0x00
# load the B Value to stack, little endian
lni 0x00,0x00,0x0A,0x00
# load the arithmetic operation to the stack (multiplication)
li 0x03
push
# call the function that loads the values + operation from the stack, and reads
pcr
jmp write_au
# write out the stack to memory
li 0x20
sp GRA
mnb_store
mnb_incr
mnb 4

### Division
# load the A Value to stack, little endian
lni 0x00,0x00,0x0A,0x00
# load the B Value to stack, little endian
lni 0x0A,0x00,0x00,0x00
# load the arithmetic operation to the stack (division)
li 0x04
push
pcr
jmp write_au
li 0x30
sp GRA
mnb_store
mnb_incr
mnb 4
catch: jmp catch

write_au: li 0xfe
          sp GRA
          pop
          mnb_store
          mnb_noincr
          mnb 9
          mnb_load
          mnb 4
          goto
# Example guide for TISC Assembly Code -> "tac"

### Structure

Each assembly line must start with either a comment, 

	#Some words

An Instruction:

	[opcode] [arguements]

Or a label, followed with an instruction,

	[label]: [opcode] [arguements]

Please refer to other examples as reference.

### Arguement Legend

 * `A,B,C` specifies the positional arguements as types

 * `<GR>` specifies the arguement type as a General Register
 	* The following types are accepted:
 		* `GRA` General Purpose Register A
 		* `GRB` General Purpose Register B
 		* `GRC` General Purpose Register C

 * `<label>` specifies the arguement as a label

## 0 Arguement Instructions

Do nothing and reset comparsion operation

	nop

 Push GRC to the stack

	push

Pop the stack to GRC

	pop

Program Counter Read - get the program counters current address

	pcr

Set Comparsion Operation - compare for `A > B`

	sco_gthan

Set Comparison Operation - compare for `A + B > 255`

	sco_oflow

Set Comparison Operation - compare for `A & B == 0`

	sco_andeq

Set Comparison Operation - compare for `A ^ B == 0`

	sco_xoreq

Increment memory pointer (set with the `sp` instruction) by `1`

	ptrinc

## 1 Arguement Instructions

Load immediate to GRC. This is a macro for successive lli and lui instructions

	li A<0-255>

Load Lower Immediate to GRC, Least Significant 4 bits of a byte 'nibble'

	lli A<0-15>

Load Upper Immediate to GRC, Most Significant 4 bits of a byte 'nibble'
> Note: This operation ORs the contents of GRC. May be of use. (hint: sign)

	lui A<0-15>

Jumps to particular label. This jump will be conditional if following a cmp

	jmp A<label>

Loads a byte from memory into the specified register. Uses the memory pointer

	lb A<GR>

Stores a byte from the specified register into memory. Uses the memory pointer

	sb A<GR>

Set the memory pointer to specify a specific address in RAM from the specified register

	sp A<GR>

# 2 Arguement Instructions

Increment the specified register `A` and save in register `B` 

	cin A<GR> B<GR>

Move the conents of `A` to `B`

	mov A<GR> B<GR>

Compare `A` and `B` and generate a flag based on the current compare operation

	cmp A<GR> B<GR>

# 3 Arguement Instructions

Logical OR the contents of `A` and `B`, store the result in `C`

	or A<GR> B<GR> C<GR>

Logical NAND the contents of `A` and `B`, store the result in `C`

	nand A<GR> B<GR> C<GR>

Add the contents of `A` and `B`, store the result in `C`

	add A<GR> B<GR> C<GR>

# Example guide for TISC Assembly Code -> "tac"

TODO: Implement some kind of variables / defines

### Structure

Each assembly line must start with either a comment:

	#Some words

An Instruction:

	[opcode] [arguments]

Or a label, followed with an instruction:

	[label]: [opcode] [arguments]

Please refer to the examples as reference.

### Argument Legend

 * `A<type>, B<type> , C<type>` specifies the positional arguments as types

 * `<GR>` specifies the argument type as a General Register
 	* The following types are accepted:
 		* `GRA` General Register A = 0b01
 		* `GRB` General Register B = 0b10
 		* `GRC` General Register C = 0b11
 		* `NIL` No register    NIL = 0b00

 * `<label>` specifies the argument as a label

 * `<1-16>` `<0-255>` specifies an integer value
 	* The following formats are accepted:
 		* Base 10
 		* Base 16: Prefixed with `0x`
 		* Base 2: Prefixed with `0b`

 * `<bytes>` An array of integers within `<0-255>` e.g. `0xff,255,0b11111111`

 * `<string>` An ascii string, supports escape characters: `\0`, `\n`, `\r`

## 0 Argument Instructions

Push `GRA` to the stack

	push

Pop the stack to `GRA`

	pop

Peek the stack to `GRA`

	peek

Pushes a byte from memory into stack. Uses the memory pointer

	lbs

Pops from the stack into memory. Uses the memory pointer

	sbs

Pops from the stack to use as the memory pointer

	sps

Set ADD Operation for `cmp` and `op` instructions.

`op` will perform `C = A + B`.
`cmp` will flag if `A + B` produces an overflow

	sop_add

Set SUBTRACT Operation for `cmp` and `op` instructions.

`op` will perform `C = A - B`.
`cmp` will flag if the `(A - B) >= 0`.

	sop_sub

Set AND Operation for `cmp` and `op` instructions. 
`op` will perform `C = A & B`.
`cmp` will flag if `(A & B) != 0`.

	sop_and

Set XOR Operation for `cmp` and `op` instructions. `cmp` will flag if the result
is nonzero. `op` will perform bitwise XOR.

	sop_xor

Set PCNT Operation for `cmp` and `op` instructions.

`op` will perform `popcount(A | B)` .
`cmp` will flag if `popcount(A) > popcount(B)`.

	sop_pcnt

Set CIN Operation for `cmp` and `op` instructions.

`op` will perform `A + B + 1`.
`cmp` will flag if there is an overflow.

	sop_cin

Set Left Shift operation for `cmp` and `op` instructions. 

`op` will perform `A << B`.
`cmp` will flag if there is overflow.

	sop_lsh

Set Right Shift operation for `cmp` and `op` instructions.

`op` will perform `A >> B`.
`cmp` will flag if there is underflow.

	sop_rsh

Program Counter Return Address - pushes program counter return address to stack

	pcr

Segment - set memory segment address from stack, and pop stack

	seg

Unconditionally branch to the address in `GRA`

	goto

Do not increment the memory pointer when peforming an mnb instruction.

	mnb_noincr

Increment the memory pointer when peforming an mnb instruction.

> Example: `mnb 4` will increment the memory pointer by 4

	mnb_incr

Decrement the memory pointer when performing an mnb instruction.

> Example: `mnb 4` will decrement the memory pointer by 4

	mnb_decr

Pushes on to the stack from the memory pointer when performing an mnb instruction.

	mnb_load

Pops from the stack to the memory pointer when performing an mnb instruction.

	mnb_store

## 1 Argument Instructions

Meta-instruction that is identical to `li` that gets the assigned address of
the label to use as an immediate

	getlabel A<label>

Meta-instruction that chains together `lni` and `li` instructions that load an
ascii string onto the stack, popping the stack will present the string in order

	string A<string>

Meta-instruction that chains together `lni` and `li` instructions that load an
ascii string onto the stack, popping the stack will present the string in
reverse order

	string_r A<string>

Load Immediate to `GRA`

	li A<0-255>

Load `N` Bytes to `STACK`. `len(<bytes>)` must be > 1

> Example: `lni 0x1,0x2,0x3,4,5,6`

	lni A<bytes>

Jumps to particular label unless the flag is set by the instruction executed
prior to the `jmp` instruction.

	jmp A<label>

Loads a byte from memory into the specified register. Uses the memory pointer

	lb A<GR>

Stores a byte from the specified register into memory. Uses the memory pointer

	sb A<GR>

Set the memory pointer from the specified register

	sp A<GR>

Move a number of bytes, the direction and behavior set by `mnb_*` instructions

	mnb A<1-16>

Set specified register `A` to 1

	one A<GR>

Zero specified register `A`

	zero A<GR>

Perform `A = A + GRA` and use the result to set memory pointer

	ptradd A<GR>

# 2 Argument Instructions

Increment the specified register `A` and store in register `B`, flag if overflow

	incr A<GR> B<GR>

Decrement the specified register `A` and store in register `B`, flag if postive

	decr A<GR> B<GR>

Logical AND the contents of `A` and `B`, store the result in `B`, flag if nonzero

	and A<GR> B<GR>

Logical XOR the contents of `A` and `B`, store the result in `B`, flag if nonzero

	xor A<GR> B<GR>

Move the contents of `A` to `B`

	mov A<GR> B<GR>

Perform the operation set with `sop_*`, discarding output, to generate a flag

	cmp A<GR> B<GR>

# 3 Argument Instructions

Logical OR the contents of `A` and `B`, store the result in `C`, flag if nonzero

	or A<GR> B<GR> C<GR>

Logical NAND the contents of `A` and `B`, store the result in `C`, flag if nonzero

	nand A<GR> B<GR> C<GR>

Perform the operation set with `sop_*`, with `A` and `B`, store the result in `C`

	op A<GR> B<GR> C<GR>

# Assembler Instructions

> These instructions are meta-instructions that configure a parameter within the
assembler

Set the program address of the next instruction to the specified address in memory

	space A<0-255>

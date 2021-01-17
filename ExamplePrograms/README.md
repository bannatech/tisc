# Example guide for TISC Assembly Code -> "tac"

TODO: Implement some kind of variables / defines

### Structure

Each assembly line must start with either a comment, 

	#Some words

An Instruction:

	[opcode] [arguments]

Or a label, followed with an instruction,

	[label]: [opcode] [arguments]

Please refer to other examples as reference.

### Argument Legend

 * `A,B,C` specifies the positional arguments as types

 * `<GR>` specifies the argument type as a General Register
 	* The following types are accepted:
 		* `GRA` General Purpose Register A
 		* `GRB` General Purpose Register B
 		* `GRC` General Purpose Register C
 		* `NIL` No register referenced

 * `<label>` specifies the argument as a label

## 0 Argument Instructions

Do nothing and reset the set operation to ADD

	nop

Push GRA to the stack

	push

Pop the stack to GRA

	pop

Program Counter Read - get the program counters current address

	pcr

Set ADD Operation for `cmp` and `op` instructions. `cmp` will flag if there is 
an overflow. `op` will perform addition.

	sop_add

Set SUBTRACT Operation for `cmp` and `op` instructions. `cmp` will flag if the 
result has a positive sign. `op` will perform subtraction.

	sop_sub

Set AND Operation for `cmp` and `op` instructions. `cmp` will flag if the result
is nonzero. `op` will perform bitwise AND.

	sop_and

Set XOR Operation for `cmp` and `op` instructions. `cmp` will flag if the result
is nonzero. `op` will perform bitwise XOR.

	sop_xor

Set XNOR Operation for `cmp` and `op` instructions. `cmp` will flag if the 
result is nonzero. `op` will perform bitwise XNOR.

	sop_xnor

Set CIN Operation for `cmp` and `op` instructions. `cmp` will flag if there is
an overflow. `op` will perform with the carry in set.

	sop_cin

Set Left Shift operation for `cmp` and `op` instructions. `cmp` will flag if
there is overflow. `op` will perform A << B.

	sop_lsh

Set Right Shift operation for `cmp` and `op` instructions. `cmp` will flag if
there is underflow. `op` will perform A >> B.

	sop_rsh

Increment memory pointer (set with the `sp` instruction) by `1`

	ptrinc

## 1 Argument Instructions

Load immediate to `GRC`. This is a macro for successive lli and lui instructions

	li A<0-255>

Load Lower Immediate to `GRC`, Least Significant 4 bits of a byte

	lli A<0-15>

Load Upper Immediate to `GRC`, Most Significant 4 bits of a byte
> Note: This operation ORs the contents of `GRC`. (you may find this useful)

	lui A<0-15>

Jumps to particular label unless the flag is set by the `cmp` instruction
executed prior to the `jmp` instruction. 

	jmp A<label>

Loads a byte from memory into the specified register. Uses the memory pointer

	lb A<GR>

Stores a byte from the specified register into memory. Uses the memory pointer

	sb A<GR>

Set the memory pointer to specify a specific address in RAM from the specified register

	sp A<GR>

# 2 Argument Instructions

Increment the specified register `A` and save in register `B` 

	cin A<GR> B<GR>

Move the contents of `A` to `B`

	mov A<GR> B<GR>

Compare `A` and `B` and generate a flag based on the current compare operation

	cmp A<GR> B<GR>

# 3 Argument Instructions

Logical OR the contents of `A` and `B`, store the result in `C`

	or A<GR> B<GR> C<GR>

Logical NAND the contents of `A` and `B`, store the result in `C`

	nand A<GR> B<GR> C<GR>

Perform the operation configured with the `sco_*` opcode, with `A` and `B`, store the result in `C`

	op A<GR> B<GR> C<GR>

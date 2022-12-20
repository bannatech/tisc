# fn_call_beyond_segments - Demonstrates how to call and return between segments
#                           of memory that expands the program memory size limit
#                           to 0xFFFF
#
# The program counter of the TISC is only able to address the 8-bit space: this
# allows for an architecture that uses an absolute one byte unit as the atomic
# size for processing instruction metadata. This program explores expanding the
# executable instruction space to a 16-bit space.
#
# The method in which we are able to expand the executable instruction space is
# through the use of external instruction memory with a writable register mapped
# to a 8-bit RAM space.
#
#-------------------------------------------------0x00 SEGMENT START------------
#-------------------------------------------------0x00 SEGMENT INDEX START------
# Segment index, responsible for "routing" calls to other segments
# Expects:
#     GRA = jump address in new segment
#     GRB = new segment address
index@00: lni 0xFF,0x00
          seg
          sps    # the segment address location is 0xFF on MMIO
          lbs    # push our current segment to the stack
          sb GRB # set the segment address we should be in
#                  the next instruction will be at the same PC address but in
#                  the segment we set. e.g. 0x0004 -> 0xFF04 if GRB = 0xFF
#
# Since this is in segment 0x00, we must test if we are just starting (jump pointer is 0x00)
          and GRA GRA  # test if the jump pointer is null
          jmp begin@00 # jump to the actual beginning of the program
          goto         # jump to the pointer if it isn't 0
#-------------------------------------------------0x00 SEGMENT INDEX END--------
## The function calling convention
#
# Function calls across segments are implemented as such:
# First, any function arguements are placed on the stack (there are none here)
begin@00: mnb_incr
          getlabel test@01 # the target function is pushed to the stack
          push
          li 0x1 # The segment address is assigned to GRB
          mov GRA GRB
          getlabel matrix@01 # The segment matrix address is assigned to GRA
          pcr # The return address is pushed to stack
          jmp index@00 # We jump to the segment index to call the function
# Immediately after the jump, the function will return with the segment address
# on the top of the stack. Any return values are after the segment address.
          pop # pop our previous segment address
loop@00:  jmp loop@00
#-------------------------------------------------0x00 SEGMENT END--------------
#-------------------------------------------------0x01 SEGMENT START------------
#-------------------------------------------------0x01 SEGMENT INDEX START------
space 0x0100
index@01: lni 0xFF,0x00
          seg
          sps
          lbs
          sb GRB # set the segment address we should be in
          goto
#-------------------------------------------------0x01 MATRIX START-------------
# This section handles returning to the callee by pulling the return segment and
# return address off of the stack, and then jumps to the target address with a
# local function call
#
# The limitation is that a function call can not call back into the callee's
# segment -> All function calls must call into a unique segment.
#
#  A -> B -> C is allowed, but:
#  A -> B -> A is not allowed.
#
# This could be resolved by implementing a software stack, but this would
# significantly increase complexity and memory use.
#
# We expect the stack to have:
# 0: Return segment address
# 1: Return address
# 2: Target function address (in this space)
matrix@01: li 0x3 # Our pointer in RAM for the return segment and address
           sp GRA
           mnb_store
           mnb 2   # pop + store the return segment, address
           pop     # pop the target function to GRA
           goto    # goto will jump to the target function in GRA
return@01: li 0x3  # Our pointer in RAM for the return segment and address
                   # we will return from the function here
           sp GRA
           lb GRB  # load the return segment
           incr GRA GRA
           sp GRA
           lb GRA  # load the return address
           jmp index@01 # go route us!
#-------------------------------------------------0x01 SEGMENT INDEX END--------
# set this bit of code in a random spot of memory within the 0x01 segment
space 0x014F
         # function call 1
test@01: getlabel deee@02 # get the address of our target function in 0x02
         push
         li 0x2
         mov GRA GRB
         getlabel matrix@02
         pcr
         jmp index@01
         pop
         # function call 2
         getlabel aade@02 # get the address of our target function in 0x02
         push
         li 0x02
         mov GRA GRB
         getlabel matrix@02
         pcr
         jmp index@01
         pop
         jmp return@01
#-------------------------------------------------0x01 SEGMENT END--------------
#-------------------------------------------------0x02 SEGMENT START------------
#-------------------------------------------------0x02 SEGMENT INDEX START------
space 0x0200
index@02: lni 0xFF,0x00
          seg
          sps
          lbs
          sb GRB # set the segment address we should be in
          goto
#-------------------------------------------------0x02 MATRIX START-------------
matrix@02: li 0x5  # Our pointer in RAM for the return segment and address
           sp GRA
           mnb_store
           mnb 2   # pop + store the return segment, address
           pop     # pop our target function to GRA
           goto    # goto will jump to the target function in GRA
return@02: li 0x5  # Our pointer in RAm for the return segment and address
                   # we will return from the function here
           sp GRA
           lb GRB  # load the return segment
           incr GRA GRA
           sp GRA
           lb GRA  # load the return address
           jmp index@02 # go route us!
#-------------------------------------------------0x02 SEGMENT INDEX END--------
# set this bit of code in a random spot of memory within the 0x02 segment
space 0x022F
deee@02: lni 0xDE,0xFD
         sps
         sbs
         jmp return@02
aade@02: lni 0xAD,0xFD
         sps
         sbs
         jmp return@02
#-------------------------------------------------0x02 SEGMENT END--------------

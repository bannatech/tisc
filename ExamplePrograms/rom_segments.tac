# rom_segments - This file demonstrates how to call and return between segments
#                of memory that expands the program memory size limit to 0xFFFF
#                with external instruction memory
#
# The program counter of the TISC is only able to address the 8-bit space: and
# that is not a lot of memory. This program explores expanding the executable 
# instruction space to a 16-bit space.
#
# The method in which we are able to expand the executable instruction space is
# through the use of external instruction memory. This instruction memory serves
# program words in a 16-bit address space, which has the upper byte mapped to a
# writable memory mapped register.

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
# Since this is in segment 0x00, we must test if we are just starting
          sop_and
          cmp GRA GRA  # test if the jump pointer is null
          jmp begin@00 # jump to the actual beginning of the program
          goto         # jump to the pointer if it isn't 0
#-------------------------------------------------0x00 SEGMENT INDEX END--------
#
begin@00: li 0x1 # segment address
          mov GRA GRB
          getlabel test@01 # get the address of our target function in 0x01
          pcr
          jmp index@00
          pop
loop@00:  jmp loop@00
#-------------------------------------------------0x00 SEGMENT END--------------
#-------------------------------------------------0x01 SEGMENT START------------
#-------------------------------------------------0x01 SEGMENT INDEX START------
space 0x0100
index@01: lni 0xFF,0x00
          seg
          sps
          lbs
          sb GRB
          goto
#-------------------------------------------------0x01 SEGMENT INDEX END--------
# set this bit of code in a random spot of memory within the 0x01 segment
space 0x019F
test@01: pop # pop the segment address
      mov GRA GRB
      pop # pop the return address
      jmp index@01
#-------------------------------------------------0x01 SEGMENT END--------------

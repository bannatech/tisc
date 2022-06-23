# test_segment - This file demonstrates how to call and return between segments
#                of memory that expands the program memory size limit to 0xFFFF
#
#-------------------------------------------------0x00 SEGMENT START------------
#-------------------------------------------------0x00 SEGMENT INDEX START------
# Segment index, responsible for "routing" calls to other segments
# Expects:
#     GRA = jump address in new segment
#     GRB = new segment address
index@00: nand NIL NIL GRC
          sp GRC # the segment address location is 0xFF on MMIO
          lbs    # push our current segment to the stack
          sb GRB # set the segment address we should be in
#                  the next instruction will be at the same PC address but in 
#                  the segment we set. e.g. 0x0004 -> 0xFF04 if GRB = 0xFF
#
# Since this is in segment 0x00, we must test if we are just starting
          sop_and
          cmp GRA GRA  # test if the jump pointer is 
          jmp begin@00 # jump to the actual beginning of the program
          goto         # jump to the pointer if it isn't 0
#-------------------------------------------------0x00 SEGMENT INDEX END--------
# 
begin@00: lli 0x1 # segment address
          mov GRA GRB
          getlabel test@01 # get the address of our target function in 0x01
          pcr
          jmp index@00
loop@00:  jmp loop@00
#-------------------------------------------------0x00 SEGMENT END--------------
#-------------------------------------------------0x01 SEGMENT START------------
#-------------------------------------------------0x01 SEGMENT INDEX START------
segment 0x0100
index@01: nand NIL NIL GRC
          sp GRC
          lbs
          sb GRB
          goto
#-------------------------------------------------0x01 SEGMENT INDEX END--------
# set this bit of code in a random spot of memory within the 0x01 segment
segment 0x019F
test@01: pop # pop the segment address
      mov GRA GRB
      pop # pop the return address
      jmp index@01
#-------------------------------------------------0x01 SEGMENT END--------------

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
begin@00: mnb_incr
          mnb_store
          lni 0x01,0x01,0x10
          sps
          mnb 2
          getlabel init
          push
          li 3
          mov GRA GRB
          getlabel matrix@03
          pcr
          jmp index@00
          pop
# terminal_test - Interactive terminal program allowing you to type commands to
#                 call specific methods.
#                 Uses the ExampleConfigurationROM circuit
###################### Print the welcome string
        string Welcome!\n\0
        pcr
        jmp print
        jmp term_char
###################### Set up our buffer pointer to live in GRC
poll_setup: lni 32,0x00
            seg
            pop
            mov GRA GRC
###################### Set up our polling loop
poll:   sop_xor
        lni 128,252 # Load the bitmask for available data + the polling address
        sps         # Set the polling address
        pop         # Pop the bitmask for available data into GRA
###################### Poll until data is available
p_loop: lb GRB
        cmp GRB GRA
        jmp p_loop
###################### Handle buffering the data and echo
###################### Clears the byte out of the buffer in the keyboard
        sb GRA
###################### Did we read a backspace character? If so, handle that
        li 8 # ascii BS
        cmp GRA GRB
        jmp bcksp
###################### Echo the byte
        sb GRB
###################### Write the byte in the buffer in RAM
        sp GRC
        sb GRB
###################### Increment the buffer pointer
        incr GRC GRC
###################### If we have reached the end of the buffer, restart
        li 110 # max buffer size = 78, 32 + 78 = 110
        cmp GRA GRC
        jmp term_char
###################### If the user pressed the enter key, output the buffer
        li 10 # ascii LF
        cmp GRB GRA
        jmp cmpr
        jmp poll
###################### Handle backspaces
bcksp:  li 32 # buffer pointer
###################### Are we at the start of the buffer? If so, we can return
        cmp GRA GRC
        jmp poll
###################### Echo the byte
        sb GRB
###################### Decrement the buffer pointer and start polling again
        li 1
        sop_sub
        op GRC GRA GRC
        jmp poll
############ Print routine that expects a null-terminated string on the stack
print:      pop
            mov GRA GRB
            sop_and
            li 252 # MMIO TTY address
            sp GRA
print_loop: pop
            cmp GRA GRA    # test for a null byte
            jmp print_done # if we get a null byte, return
            sb GRA
            jmp print_loop
print_done: mov GRB GRA
            goto
##### first off, store our buffer end pointer in RAM
cmpr: li 32 # buffer pointer
      push
      # compute the buffer size
      sop_sub
      op GRC GRA GRA
      push
      li 0x01
      sp GRA
      mnb_store
      mnb 2
      # read if we are going to copy the buffer from the "set" command
      li 0x10
      sp GRA
      lb GRA
      or GRA GRA
      jmp copy
      getlabel do_comp@01
      push
      li 0x1
      mov GRA GRB
      getlabel matrix@01
      pcr
      jmp index@00
      pop
      li 0x11
      sp GRA
      lb GRA
      sop_and
      cmp GRA GRA
      jmp poll_setup
###################### Print the terminal character '$ '
term_char:  string $ \0
            pcr
            jmp print
            jmp poll_setup
########### copy routine
copy:       li 0x01
            sp GRA
            lb GRC
            incr GRA GRA
            sp GRA
            lb GRB
            sp GRB
            one GRB
            mnb_load
            sop_sub
            mov GRC GRA
copy_l:     cmp GRA GRB
            jmp copy_w
            mnb 1
            op GRA GRB GRA
            jmp copy_l
copy_w:     lni 0x3F,0x01
            seg
            sps
            mov GRC GRA
            push
            mnb_store
            mnb 1
copy_w_l:   cmp GRA GRB
            jmp copy_d
            mnb 1
            op GRA GRB GRA
            jmp copy_w_l
copy_d:     lni 0x1,0x1,0x10,0x00
            seg
            sps
            mnb 2
            jmp term_char
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
matrix@01: li 0x3  # Our pointer in RAM for the return segment and address
           sp GRA
           mnb_store
           mnb 2
           pop     # pop the target function to GRA
           goto    # goto will jump to the target function in GRA
return@01: li 0x3 # we will return from the function here
           sp GRA  # The pointer in RAM for the return segment and address
           lb GRB  # load the return segment
           incr GRA GRA
           sp GRA
           lb GRA  # load the return address
           jmp index@01 # go route us!
###################### Compare the buffer to our commands
###################### Specify the string we will be comparing, and the length.
###################### This is in reverse order, as the bytes will be pushed to
###################### the stack. The last byte is the first byte out.
###################### The last byte is the length of the string.
do_comp@01: sop_add
############ Respond to 'testing123' with 'horray!'
#         "testing123\n\0", length 11
n0:   lni 0x1D,0x01
      seg
      sps
      mnb_load
      mnb 13
      pcr
      jmp callcmp
      pop
      sop_and
############ If the strings match, print our response, else go handle input
      cmp GRA GRA
      jmp n1
############ load the null-terminated string
############ "horray!\n\0"
      lni 0x2A,0x01
      seg
      sps
      mnb_load
      mnb 9
      pcr
      jmp print@01
      jmp return@01
############# Respond to 'ping' with 'pong'
##         "ping\n\0", length 5
n1:   lni 0x10,0x01
      seg
      sps
      mnb_load
      mnb 7
      pcr
      jmp callcmp
      pop
      sop_and
############ If the strings match, print our response, else continue to the next
      cmp GRA GRA
      jmp n2
############ load the null-terminated string to print
############ "pong\n\0"
      lni 0x17,0x01
      seg
      sps
      mnb_load
      mnb 6
      pcr
      jmp print@01
      jmp return@01
############ Respond to 'clear' by clearing the TTY
#      "clear\n", length 6
n2:    string clear\n\0
       li 6 #length
       push
       pcr
       jmp callcmp
       pop
       sop_and
       cmp GRA GRA
       jmp n3
########## clear the TTY
       lni 12,252 # Load acii FF and our TTY MMIO address
       sps
       sbs
       jmp return@01
############ Respond to 'set' by copying the next entered string into memory
n3:    string set\n\0
       li 4 #length
       push
       pcr
       jmp callcmp
       pop
       sop_and
       cmp GRA GRA
       jmp n4
######### set the copy flag to true, don't do setup
       lni 0x00,0x11,0x00,0x10,0x00
       seg
       sps
       sbs
       sps
       sbs
       jmp return@01
############ Respond to 'print' by printing the string stored in memory by "set"
n4:    string print\n\0
       li 6 #length
       push
       pcr
       jmp callcmp
       pop
       sop_and
       cmp GRA GRA
       jmp return@01
######## echo our copy buf
       lni 0x0,0x3F,0x01
       seg
       sps
       mnb_load
       mnb 1
       pop
       incr GRA GRA
echo_r: decr GRA GRA
        jmp echo_d
        mnb 1
        jmp echo_r
echo_d: pcr
       jmp print@01
       jmp return@01
callcmp: lni 0xF,0x0
         seg
         sps
         pop
         sb GRA
         getlabel cmpr_f@02
         push
         li 0x2
         mov GRA GRB
         getlabel matrix@01
         pcr
         jmp index@01
         pop
         li 0xF
         sp GRA
         lb GRA
         goto
print@01:      lni 252,0x0
               seg
               sps
               pop
               mov GRA GRB
               sop_and
print_loop@01: pop
               cmp GRA GRA    # test for a null byte
               jmp print_done@01 # if we get a null byte, return
               sb GRA
               jmp print_loop@01
print_done@01: mov GRB GRA
               goto
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
matrix@02: li 0x5 # Our pointer in RAM for the return segment and address
           sp GRA
           mnb_store
           mnb 2
           pop     # pop our target function to GRA
           goto    # goto will jump to the target function in GRA
return@02: li 0x5 # Our pointer in RAm for the return segment and address
                   # we will return from the function here
           sp GRA
           lb GRB  # load the return segment
           incr GRA GRA
           sp GRA
           lb GRA  # load the return address
           jmp index@02 # go route us!
#-------------------------------------------------0x02 SEGMENT INDEX END--------
######### compare function, compares the null-terminated string on the stack to
#         the contents of the buffer and returns a boolean in GRB.
# Expects:
#   Stack: size of compare string, null-terminated compare string
#   Memory:
#     0x1 - buffer length
#     0x2 - buffer pointer
cmpr_f@02: li 0x01 # buffer length / pointer
         sp GRA
######## Get the buffer length + start address
         lb GRC
         incr GRA GRA
         sp GRA
         lb GRB
         sp GRB
######## If the length of our string does not match our buffer,
######## just exit immediately.
         pop
         sop_xor
         cmp GRC GRA
         jmp c_cmpr
         jmp c_fail
c_cmpr: mov GRB GRC
c_loop: pop
######## Test if we've reached the end of our comparision buffer.
######## If we've reached the end and haven't branched out, then
######## it is a match!
        cmp GRA NIL # test for a null byte
        jmp c_match
        lb GRB
######## Increment our buffer pointer
        incr GRC GRC
        sp GRC
######## If the bytes match, continue looping
        cmp GRB GRA
        jmp c_loop
# Return false
c_fail: pop         # Flush the stack until we get the null byte
        cmp GRA NIL # test for a null byte
        jmp c_r     # if null, return
        jmp c_fail  # else continue flushing
# Return true
c_match: li 0x01
c_r:    push
        jmp return@02
#-------------------------------------------------0x02 SEGMENT END--------------
#-------------------------------------------------0x03 SEGMENT START------------
#-------------------------------------------------0x03 SEGMENT INDEX START------
space 0x0300
index@03: lni 0xFF,0x00
          seg
          sps
          lbs
          sb GRB # set the segment address we should be in
          goto
#-------------------------------------------------0x03 MATRIX START-------------
matrix@03: li 0x8  # Our pointer in RAM for the return segment and address
           sp GRA
           mnb_store
           mnb 2
           pop     # pop our target function to GRA
           goto    # goto will jump to the target function in GRA
return@03: li 0x8  # Our pointer in RAM for the return segment and address
                   # we will return from the function here
           sp GRA
           lb GRB  # load the return segment
           incr GRA GRA
           sp GRA
           lb GRA  # load the return address
           jmp index@03 # go route us!
########## static strings
init:      mnb_incr
           mnb_store
           lni 0x05,0x10,0x01 # string length, address, segment
           seg
           sps
           string_r ping\n\0
           mnb 7

           li 0x17 # address
           sp GRA
           string_r pong\n\0
           mnb 6 #

           lni 0x0B,0x1D # string length, address
           sps
           string_r testing123\n\0
           mnb 13

           string_r horray!\n\0
           li 0x2A
           sp GRA
           mnb 9

           li 0
           seg
trap@03:   jmp return@03
#-------------------------------------------------0x03 SEGMENT INDEX END--------


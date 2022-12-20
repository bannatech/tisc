# terminal_test - Interactive terminal program allowing you to type commands to
#                 call specific methods.
#                 Uses the ExampleConfigurationROM circuit
###################### Print the welcome string
        lni 0x0,0xa,0x21,0x65,0x6d,0x6f,0x63,0x6c,0x65,0x57
        pcr
        jmp print
###################### Print the terminal character '$ '
setup:  lni 0x0,0x20,0x24
        pcr
        jmp print
###################### Set up our buffer pointer to live in GRC
        li 32 # buffer pointer
        mov GRA GRC
###################### Set up our polling loop
poll:   sop_xor
        lni 128,252 # Load the bitmask for available data + the polling address
        sps         # Set the polling address
        pop         # Get the bitmask for available data
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
        jmp setup
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
###################### Compare the buffer to our commands
###################### Specify the string we will be comparing, and the length.
###################### This is in reverse order, as the bytes will be pushed to
###################### the stack. The last byte is the first byte out.
###################### The last byte is the length of the string.
##### first off, store our buffer end pointer in RAM
cmpr: li 32 # buffer pointer
      push
      sop_sub
      op GRC GRA GRC
      li 0x01
      sp GRA
      sb GRC
      incr GRA GRA
      sp GRA
      sbs
############ Respond to 'ping' with 'pong'
#         "ping\n\0", length 5
      lni 0x0,0x0a,0x67,0x6e,0x69,0x70,5
      pcr
      jmp cmpr_f
      sop_and
############ If the strings match, print our response, else continue to the next
      cmp GRB GRB
      jmp n0
############ load the null-terminated string to print
############ "pong\n\0"
      lni 0x00,0x0a,0x67,0x6e,0x6f,0x70
      pcr
      jmp print
      jmp setup
############ Respond to 'testing123' with 'we are alive'
#         "testing123\n\0", length 11
n0:   lni 0x0,0x0a,0x33,0x32,0x31,0x67,0x6e,0x69,0x74,0x73,0x65,0x74,11
      pcr
      jmp cmpr_f
      sop_and
############ If the strings match, print our response, else go handle input
      cmp GRB GRB
      jmp n1
############ load the null-terminated string
############ "we are alive\n\0"
      lni 0x0,0xa,0x65,0x76,0x69,0x6c,0x61,0x20,0x65,0x72,0x61,0x20,0x65,0x77
      pcr
      jmp print
      jmp setup
############ Respond to 'foo' with 'bar'
#      "foo\n", length 4
n1:    lni 0x0,0x0a,0x6f,0x6f,0x66,4
       pcr
       jmp cmpr_f
       sop_and
############ If the strings match, print our response, else go handle input
       cmp GRB GRB
       jmp n2
############ load the null-terminated string
############ "bar\n\0"
       lni 0x0,0xa,0x72,0x61,0x62
       pcr
       jmp print
       jmp setup
############ Respond to 'clear' by clearing the TTY
#      "clear\n", length 6
n2:    lni 0x0,0xa,0x72,0x61,0x65,0x6c,0x63,6
       pcr
       jmp cmpr_f
       sop_and
       cmp GRB GRB
       jmp setup
########## clear the TTY
       lni 12,252 # Load acii FF and our TTY MMIO address
       sps
       sbs
       jmp setup
######### compare function, compares the null-terminated string on the stack to
#         the contents of the buffer and returns a boolean in GRB.
# Expects:
#   Stack: return address, size of compare string, null-terminated compare string
#   Memory:
#     0x0 - used for the return pointer address
#     0x1 - buffer length
#     0x2 - buffer pointer
cmpr_f:  li 0x0 # return pointer memory address
         sp GRA
######## Store the return pointer
         sbs
         li 0x01 # buffer length / pointer
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
c_r:    mov GRA GRB
        li 0x0
        sp GRA
        lb GRA
        goto
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

# This program reads the input from the 'keyboard' component and records in a
# buffer until the user presses the return key, at that point it compares the
# buffer with 3 unique commands with unique responses
###################### Set up our buffer pointer to live in GRC
setup:  li 32
        mov GRA GRC
###################### Set up our polling address
poll:   sop_xor
        li 252
        sp GRA
###################### Load the bitmask for available data
        li 128
###################### Poll until data is available
p_loop: lb GRB
        cmp GRB GRA
        jmp p_loop
###################### Handle buffering the data and echo
###################### Echo the byte
        sb GRB
###################### Clears the byte out of the buffer in the keyboard
        sb GRA
###################### Did we read a backspace character? If so, handle that
        lli 8
        cmp GRA GRB
        jmp bcksp
###################### Write the byte in the buffer in RAM
        sp GRC
        sb GRB
###################### Increment the buffer pointer
        cin GRC GRC
###################### If we have reached the end of the buffer, restart
        li 112
        cmp GRA GRC
        jmp setup
###################### If the user pressed the enter key, output the buffer
        lli 10
        cmp GRB GRA
        jmp cmpr
        jmp poll
###################### Handle backspaces
bcksp:  li 32
###################### Are we at the start of the buffer? If so, we can return 
        cmp GRA GRC
        jmp poll
###################### Decrement the buffer pointer
        lli 1
        sop_sub
        op GRC GRA GRC
        jmp poll
###################### Compare the buffer to our commands
###################### Specify the string we will be comparing, and the length.
###################### This is in reverse order, as the bytes will be pushed to
###################### the stack. The last byte is the first byte out.
###################### The last byte is the length of the string.
############ Respond to 'ping' with 'pong'
##### first off, store our buffer end pointer
cmpr: lli 0x01
      sp GRA
      sb GRC
#         "ping", length 4
      lni 0x67,0x6e,0x69,0x70,4
      gtr
      push
      jmp cmpr_f
      sop_and
############ If the strings match, print our response, else continue to the next
      cmp GRB GRB
      jmp n0
############ load the null-terminated string to print
############ "pong\n\0"
      lni 0x00,0x0a,0x67,0x6e,0x6f,0x70
      gtr
      push
      jmp print
      jmp setup
############ Respond to 'testing123' with 'we are alive'
n0:   lli 0x01
      sp GRA
      lb GRC
#         "testing123", length 10
      lni 0x33,0x32,0x31,0x67,0x6e,0x69,0x74,0x73,0x65,0x74,10
      gtr
      push
      jmp cmpr_f
      sop_and
############ If the strings match, print our response, else go handle input
      cmp GRB GRB
      jmp n1
############ load the null-terminated string
############ "we are alive\n\0"
      lni 0x0,0xa,0x65,0x76,0x69,0x6c,0x61,0x20,0x65,0x72,0x61,0x20,0x65,0x77
      gtr
      push
      jmp print
      jmp setup
############ Respond to 'foo' with 'bar'
n1:    lli 0x01
       sp GRA
       lb GRC
#         "foo", length 3
       lni 0x6f,0x6f,0x66,3
       gtr
       push
       jmp cmpr_f
       sop_and
############ If the strings match, print our response, else go handle input
       cmp GRB GRB
       jmp setup
############ load the null-terminated string
############ "bar\n\0"
       lni 0x0,0xa,0x72,0x61,0x62
       gtr
       push
       jmp print
       jmp setup
######### compare function
cmpr_f:  lli 0x0
         sp GRA
         pop
         sb GRA
         li 32
         sp GRA
######## Compute the length of our stored buffer
         sop_sub
         op GRC GRA GRC
######## If the length of our string is greater than the buffer,
######## just exit immediately.
         pop
         cmp GRC GRA
         jmp c_fail
c_loop: sop_sub
######## Test if we've reached the end of our comparision buffer.
        lli 1
        op GRC GRA GRC
        cmp GRC GRA
######## If we've reached the end and haven't branched out, then
######## it is a match!
        jmp c_match
######## Pop the comparision string and get the buffer byte
        pop
        lb GRB
######## Increment our buffer pointer
        ptrinc
######## If the bytes match, continue looping
        sop_xor
        cmp GRB GRA
        jmp c_loop
######## If the bytes don't match, we quit
c_fail: lli 0x00
        mov GRA GRB
        sp GRA
        lb GRA
        goto
c_match: lli 0x01
         move GRA GRB
         lli 0x00
         sp GRA
         lb GRA
         goto
print:      pop
            mov GRA GRB
            sop_and
            li 252
            sp GRA
print_loop: pop
            cmp GRA GRA
            jmp print_done
            sb GRA
            jmp print_loop
print_done: mov GRB GRA
            goto

# This program reads the input from the 'keyboard' component and records in a
# buffer until the user presses the return key, at that point it compares the
# buffer with "ping", if the buffer == "ping" then it responds with "pong"
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
cmpr:   li 32
        sp GRA
###################### Compute the length of our stored buffer
        sop_sub
        op GRC GRA GRC
###################### Specify the string we will be comparing, and the length.
###################### This is in reverse order, as the bytes will be pushed to
###################### the stack. The last byte is the first byte out.
###################### The last byte is the length of the string.
#           "ping"
        lni 0x67,0x6e,0x69,0x70,0x04
###################### If the length of our string is greater than the buffer,
###################### just exit immediately.
        pop
        cmp GRC GRA
        jmp setup
c_loop: sop_sub
###################### Test if we've reached the end of our comparision buffer.
        lli 1
        op GRC GRA GRC
        cmp GRC GRA
###################### If we've reached the end and haven't branched out, then
###################### it is a match!
        jmp ping
###################### Pop the comparision string and get the buffer byte
        pop
        lb GRB
###################### Increment our buffer pointer
        ptrinc
###################### If the bytes match, continue looping
        sop_xor
        cmp GRB GRA
        jmp c_loop
###################### If the bytes don't match, we quit
        jmp setup
###################### Respond to 'ping' with 'pong'
###################### First, load the null-terminated string
#           "pong\n\0"
ping:   lni 0x00,0x0a,0x67,0x6e,0x6f,0x70
###################### To test if a byte is null, we compare the byte to itself
###################### with a bitwise AND.
        sop_and
###################### Set our output location
        li 252
        sp GRA
###################### Print our response string until we've reached the null
p_l:    pop
        sb GRA
        cmp GRA GRA
        jmp setup
        jmp p_l

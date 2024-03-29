# typing_test.tac - test program that reads from the keyboard component, echoes
#                   input and records the buffer in RAM. Once the user returns,
#                   it plays back the buffer. Handles backspaces and typing
#                   beyond the buffer limit.
#                   Uses the ExampleConfigurationROM circuit
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
        li 8
        cmp GRA GRB
        jmp bcksp
###################### Write the byte in the buffer in RAM
        sp GRC
        sb GRB
###################### Increment the buffer pointer
        incr GRC GRC
###################### If we have reached the end of the buffer, restart
        li 112
        cmp GRA GRC
        jmp setup
###################### If the user pressed the enter key, output the buffer
        li 10
        cmp GRB GRA
        jmp output
        jmp poll
###################### Handle backspaces
bcksp:  li 32
###################### Are we at the start of the buffer? If so, we can return 
        cmp GRA GRC
        jmp poll
###################### Decrement the buffer pointer
        li 1
        sop_sub
        op GRC GRA GRC
        jmp poll
###################### Echo out all the things in the buffer to the TTY
output: li 32
o_loop: sp GRA
        lb GRB
        push
        li 252
        sp GRA
        sb GRB
        pop
        incr GRA GRA
        cmp GRA GRC
        jmp setup
        jmp o_loop

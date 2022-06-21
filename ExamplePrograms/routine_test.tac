# routine call test
# load a string on the stack
            lni 0x0,0xa,0x21,0x65,0x6e,0x6f,0x20,0x67,0x6e,0x69,0x72,0x74,0x73
# get the return address and push to stack
            gtr
            push
# jump to the print function
            jmp print
# the print function returns here, and we print a different string
            lni 0x0,0xa,0x21,0x67,0x6e,0x69,0x72,0x74,0x73,0x20,0x64,0x6e,0x6f,0x63,0x65,0x73
            gtr
            push
            jmp print
complete:   jmp complete
# Print function!
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

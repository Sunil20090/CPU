
DCLR $first_number
DCLR $second_number
DCLR $message[10]
DCLR $counter
DCLR $result
DCLR $answer[3]


convert_char:
    load %r2 '0'
    clr
    add %r1
    add %r2
    mov $result
END


mult:
    add %r1

    inc $counter
    comp %r2 $counter
    caln mult

    mov $result
    end


take_inputs:    
    aor %r1 $first_number
    syscall 0x01

    aor %r1 $second_number
    syscall 0x01
END


_main:
    call take_inputs

    load %r1 $first_number
    call convert_number
    load %r1 $result
    store %r1 $first_number

    load %r1 $second_number
    call convert_number
    load %r1 $result
    store %r1 $second_number

    
    load %r1 $first_number
    load %r2 $second_number
    clr
    call mult


    load %R1 $result
    store %R1 $answer
    load %R1 $result
    load %R2 0xf0
    clr
    add %R1
    add %R2
    mov $answer[1]
    LOAD %R1 0x04
    RTS %R1 $answer[1]
    LOAD %R1 0x10
    LOAD %R2 $answer[1]
    CLR
    CALL mult
    
    LOAD %R1 $result
    STORE %R1 $answer[2]

END
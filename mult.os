
DCLR $first_number[2]
DCLR $second_number[2]
DCLR $message[10]
DCLR $counter
DCLR $result


convert_number:
    LOAD %R2 '0'
    CLR
    ADD %R1 0x00
    SUB %R2 0x00
    MOV $result
END


mult:
    LOAD %R1 $first_number

    LOAD %R2 $second_number

    ADD %R1 0x00

    INC $counter
    COMP %R2 $counter
    CALN mult

    MOV $result
END


take_2_number:
    LOAD %R2 0x02
    SYSCALL 0x05
END


take_inputs:    
    AOR %R1 $first_number
    CALL take_2_number

    CALL print_s1

    AOR %R1 $second_number
    CALL take_2_number
END


_main:
    CALL take_input

    LOAD %R1 0x10
    STORE %R1 $first_number

    LOAD %R1 0x05
    STORE %R1 $second_number
    CALL mult
END
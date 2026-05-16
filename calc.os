DCLR $first_number[3]
DCLR $b
DCLR $second_number[6]
DCLR $RESULT

take_number:

    LOAD %R2 0x03
    SYSCALL 0x04
END


print:
    LOAD %R2 0x03
    SYSCALL 0x03
END


take_inputs:

    AOR %R1 $first_number
    CALL take_number

    AOR %R1 $second_number
    CALL take_number
END


print_numbers:

    AOR %R1 $first_number
    CALL print

    AOR %R1 $second_number
    CALL print
END


print_space:

    LOAD %R1 '_'
    STORE %R1 $b
    AOR %R1 $b
    SYSCALL 0x01
END


convert_number:

    AOR %R1 $first_number
    LOAD %R2 '0'

    CLR
    ADD %R1 0x00
    SUB %R2 0x00

    MOV $RESULT

    AOR %R1 $RESULT

    SYSCALL 0x01
END


_main:

    CALL take_inputs

    CALL convert_number

    CALL print_numbers

END


DCLR $first_number[2]
DCLR $b
DCLR $second_number[2]
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
    LOAD %R2 '0'
    CLR
    ADD %R1 0x00
    SUB %R2 0x00

    MOV $RESULT
END


_main:

    CALL take_inputs

    LOAD %R1 $first_number[0]
    CALL convert_number
    LOAD %R1 $RESULT
    STORE %R1 $first_number[1]

    LOAD %R1 $second_number[0]
    CALL convert_number
    LOAD %R1 $RESULT
    STORE %R1 $second_number[1]


    LOAD %R1 $first_number[1]
    LOAD %R2 $second_number[1]

    CLR
    ADD %R1
    SUB %R2

    LOAD %R1 '0'
    ADD %R1

    MOV $b

    AOR %R1 $b
    SYSCALL 0x01

END


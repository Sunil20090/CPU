
DCLR $first_number
DCLR $second_number
DCLR $message[10]
DCLR $counter
DCLR $result
DCLR $answer[3]


convert_number:
    LOAD %R2 '0'
    CLR
    ADD %R1 0x00
    SUB %R2 0x00
    MOV $result
END

convert_char:
    LOAD %R2 '0'
    CLR
    ADD %R1 0x00
    ADD %R2 0x00
    MOV $result
END


mult:
    ADD %R1

    INC $counter
    COMP %R2 $counter
    CALN mult

    MOV $result
END


take_inputs:    
    AOR %R1 $first_number
    SYSCALL 0x01

    AOR %R1 $second_number
    SYSCALL 0x01
END


_main:
    CALL take_inputs

    LOAD %R1 $first_number
    CALL convert_number
    LOAD %R1 $result
    STORE %R1 $first_number

    LOAD %R1 $second_number
    CALL convert_number
    LOAD %R1 $result
    STORE %R1 $second_number

    
    LOAD %R1 $first_number
    LOAD %R2 $second_number
    CLR
    CALL mult


    LOAD %R1 $result
    STORE %R1 $answer
    LOAD %R1 $result
    LOAD %R2 0xf0
    CLR
    ADD %R1
    AND %R2
    MOV $answer[1]
    LOAD %R1 0x04
    RTS %R1 $answer[1]
    LOAD %R1 0x10
    LOAD %R2 $answer[1]
    CLR
    CALL mult
    
    LOAD %R1 $result
    STORE %R1 $answer[2]

END
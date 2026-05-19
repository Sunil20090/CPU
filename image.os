
DCLR $buffer[11]
DCLR $size
DCLR $call_counter
DCLR $save_to_address

init:
    LOAD %R1 0xb
    STORE %R1 $size
END


take_inputs:
    LOAD %R1 $call_counter
    AOR %R2 $buffer
    CLR
    ADD %R1
    ADD %R2

    MOV $save_to_address

    LOAD %R1 $save_to_address

    SYSCALL 0x02

    LOAD %R1 $size
    INC $call_counter    
    COMP %R1 $call_counter
    CALN take_inputs
    
END


print:
    AOR %R1 $buffer
    LOAD %R2 $size
    SYSCALL 0x03
END

_main:
    CALL init

    CALL take_inputs

    CALL print

END
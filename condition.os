
DCLR $choice
DCLR $a_value
DCLR $b_value


print_a:
    LOAD %R1 'A'
    STORE %R1 $a_value
    AOR %R1 $a_value
    SYSCALL 0x01
END


print_b:
    LOAD %R1 'B'
    STORE %R1 $b_value

    AOR %R1 $b_value
    SYSCALL 0x01
END


take_input:
    AOR %R1 $choice
    SYSCALL 0x02
END


_main:
    CALL take_input

    LOAD %R2 '1'
    COMP %R2 $choice
    CALZ print_a

    LOAD %R2 '2'
    COMP %R2 $choice
    CALZ print_b

END
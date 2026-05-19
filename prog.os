_main:
    DCLR $a
    LOAD %R1 0x09
    STORE %R1 $a

    DCLR $b
    LOAD %R1 0x04
    STORE %R1 $b

    LOAD %R1 $a
    LOAD %R2 $b

    CLR
    ADD %R1
    SUB %R2
    LOAD %R1 '0'
    ADD %R1

    DCLR $c
    MOV $c

    AOR %R1 $c
    SYSCALL 0x01

END
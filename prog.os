save_hello:
STR1 0x68
LDR1 0x88

STR1 0x65
LDR1 0x89

STR1 0x6c
LDR1 0x8A

STR1 0x6c
LDR1 0x8B

STR1 0x6f
LDR1 0x8C
END



increament_86:
STR1 0x01
STR2 [0x86]
CLR
ADR1
ADR2
MOVR [0x86]
END

print_next:
CALL increament_86
STR1 [0x86]
LDR1 [0x78]
PRTC [0x78]
END


_main:
CALL save_hello
STR1 0x87
LDR1 0x86
CALL print_next
CALL print_next
CALL print_next
CALL print_next
CALL print_next
END


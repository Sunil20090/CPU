

print_F:
PRTC 'F'


END

_main:
DCLR $a1
DCLR $a3
STR1 [0x90]
LDR1 $a1
STR1 $a3
LDR2 [0x88]

CALL print_F


END
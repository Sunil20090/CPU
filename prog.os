
DCLR $a
DCLR $a1
DCLR $a2
DCLR $a3
DCLR $counter

DCLR $next_printable

;this is sample comment

save_name:
LDR1 'S'
LDR2 'O'

STR1 $a1
STR2 $a2

LDR1 'N'
LDR2 'U'

STR1 $a3
STR2 $a

END


print_name:
PRTC $next_printable
INC $next_printable

DEC $counter
LDR1 $counter
CLR
ADR1
CALZ print_name

END

_main:
PRTC 'A'
PRTC 0x0A
PRTC 'a'

CALL save_name

END
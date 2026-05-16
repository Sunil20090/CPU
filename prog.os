
DCLR $a
DCLR $b

DCLR $next_printable

;this is sample comment


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
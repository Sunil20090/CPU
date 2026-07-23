dclr $data
dclr $data_second

dclr $power_button[4]

dclr $vol_plus_button[4]

dclr $vol_minus_button[4]

dclr $down_button[4]
dclr $up_button[4]

define $ACTION_NEC_SEND 0x03

memory $up_button[0x0f,0x08,0x04,0x02]
memory $power_button[0x9f,0xf8,0x44,0x22]
memory $vol_plus_button[0x1f,0x38,0x3,0x1e]
memory $vol_minus_button[0x4f,0x08,0x43,0x32]

nec_send:
    syscall $ACTION_NEC_SEND
    end

power{POWER}:
    aor %r1 $power_button
    call nec_send
    end

vol_minus{VOLUME[-]}:
    aor %r1 $vol_minus_button
    call nec_send
    end

vol_plus{VOLUME[+]}:
    aor %r1 $vol_plus_button
    call nec_send
    end

up{UP_ARROW}:
    aor %r1 $up_button
    call nec_send
    end

_main:
    aor %r1 $data
    call power
    end






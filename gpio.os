dclr $data
dclr $data_second

dclr $power_button[4]

dclr $vol_plus_button[4]

dclr $vol_minus_button[4]

define $ACTION_NEC_SEND 0x03

toggle_gpio{api}:
    load %r2 0x03
    store %r2 $data_second
    clr
    add %r2 0x01
    end

init_buttons:
    ;power
    load %r1 0x20
    store %r1 $power_button[0]
    
    load %r1 0xFD
    store %r1 $power_button[1]

    load %r1 0x56
    store %r1 $power_button[2]

    load %r1 0x43
    store %r1 $power_button[3]
    
    load %r1 0x21
    store %r1 $vol_plus_button[0]
    
    load %r1 0xFE
    store %r1 $vol_plus_button[1]

    load %r1 0x26
    store %r1 $vol_plus_button[2]

    load %r1 0x33
    store %r1 $vol_plus_button[3]

    load %r1 0x11
    store %r1 $vol_minus_button[0]
    
    load %r1 0xed
    store %r1 $vol_minus_button[1]

    load %r1 0x22
    store %r1 $vol_minus_button[2]

    load %r1 0xfa
    store %r1 $vol_minus_button[3]

    end

nec_send:
    syscall $ACTION_NEC_SEND
    end

power{api}:
    aor %r1 $power_button
    call nec_send
    end

vol_minus{api}:
    aor %r1 $vol_minus_button
    call nec_send
    end

vol_plus{api}:
    aor %r1 $vol_plus_button
    call nec_send
    end

_main:
    call init_buttons

    end



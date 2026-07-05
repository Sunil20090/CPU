
dclr $data

dclr $power_button[4]
dclr $start_address

define $ACTION_TOGGLE 0x1

define $ACTION_DELAY 0x02
define $ACTION_NEC_SEND 0x03


init_power_button:
    ;0x20DF10EF
    load %r1 0x20
    store %r1 $power_button[0]

    load %r1 0xDF
    store %r1 $power_button[1]

    load %r1 0x10
    store %r1 $power_button[2]

    load %r1 0xEF
    store %r1 $power_button[3]
    end

toggle_gpio:
    load %r1 $ACTION_TOGGLE
    syscall
    end

delay_1s:
    load %r1 $ACTION_DELAY
    syscall
    end



nec_send:
    load %r1 $ACTION_NEC_SEND
    load %r2 $start_address
    syscall
    end





_main:
    call init_power_button

    aor %r1 $power_button
    store %r1 $start_address
    call nec_send

    end
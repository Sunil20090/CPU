
dclr $data

dclr $power_button[4]
dclr $start_address

define $ACTION_TOGGLE 0x1
define $ACTION_DELAY 0x02
define $ACTION_NEC_SEND 0x03

init_power_button:
    load %r1 0x20
    store %r1 $power_button[0]

    load %r1 0xDF
    store %r1 $power_button[1]

    load %r1 0x10
    store %r1 $power_button[2]

    load %r1 0xEF
    store %r1 $power_button[3]
    
    end

toggle_gpio{api}:
    load %r1 $ACTION_TOGGLE
    syscall
    end

delay_1s:
    load %r1 $ACTION_DELAY
    syscall
    end

nec_send{api}:
    load %r1 
    load %r2 $start_address
    call toggle_gpio
    syscall $ACTION_NEC_SEND
    end

_main:
    end
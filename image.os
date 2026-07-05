dclr $buffer[10]
dclr $counter
dclr $index
dclr $input_size


reset_counter:
    load %r1 0x00
    store %r1 $counter
    end

;this function is very usefull
reset_register:
    load %r1 0x00
    load %r2 0x00
    load %r3 0x00
    load %r4 0x00
    end

reset_index:
    load %r1 0x00
    store %r1 $index
    end

reset:
    call reset_counter
    call reset_index
    call reset_register
    
    end


print_value:
    aor %r1 $buffer
    clr
    add %r1
    add $counter
    mov $index

    load %r1 $index
    syscall

    inc $counter
    dec %r2
    comp %r2
    caln print_value
    call reset
    end

save_string:
    aor %r3 $buffer
    clr
    add $counter
    add %r3
    mov $index

    ;taking input
    load %r1 $index
    syscall 0x01

    ;loop control logic
    inc $counter
    dec %r2
    comp %r2
    caln save_string
    call reset
    end


print_value_5:
    load %r2 $input_size
    call print_value
    end

init:
    load %r1 0x10
    store %r1 $input_size
    end

_main:
    call init
    load %r2 $input_size
    call save_string
    call print_value_5
    end
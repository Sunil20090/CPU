define $SYSCALL_COMMAND 0x01

dclr $command_buffer[4]

dclr $command_toggle[4]
dclr $command_port_c[4]
dclr $command_name[4]
dclr $command_alphabet[4]

memory $command_buffer["SON"]
memory $command_toggle["TEP"]
memory $command_port_c["por"]
memory $command_name["nam"]

dclr $source_address
dclr $destination_address
dclr $copy_loop_counter


print_a:
    aor %r1 $command_alphabet
    syscall $SYSCALL_COMMAND
    end


interupt_handler:
    comp %r1 0x01
    calz print_a

    end

trigger_command:
    aor %r1 $command_buffer
    syscall $SYSCALL_COMMAND
    end

reset_all:
    clr $copy_loop_counter
    clr $source_address
    clr $destination_address
    load %r1 0x00
    load %r2 0x00
    load %r3 0x00
    load %r4 0x00
    end

copy_data:
    lfad %r1 $source_address
    stad %r1 $destination_address
    inc $source_address
    inc $destination_address
    inc $copy_loop_counter
    comp %r2 $copy_loop_counter
    caln copy_data
    call reset_all
    end


store_toggle_command:
    aor %r1 $command_port_c
    store %r1 $source_address
    aor %r1 $command_buffer
    store %r1 $destination_address
    load %r2 0x04
    call copy_data
    end


store_name_command:
    aor %r1 $command_name
    store %r1 $source_address
    aor %r1 $command_buffer
    store %r1 $destination_address
    load %r2 0x08
    call copy_data
    end

gpio_toggle_pin{_toggle}:
    call store_toggle_command
    call trigger_command
    end


name_pin{name_game}:
    call store_name_command
    call trigger_command
    end

_main:
    call gpio_toggle_pin
    call name_pin
    end
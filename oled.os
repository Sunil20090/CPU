dclr $destination[5]
dclr $address
dclr $command_buffer[5]

;syscalls data
define $SYSCALL_WRITE_STRING 0x05


memory $address[0x3c]
memory $command_buffer["0x3c"]


copy_destination:

    load %r1 $command_buffer
    store %r1 $destination

    load %r1 $command_buffer[1]
    store %r1 $destination[1]

    load %r1 $command_buffer[2]
    store %r1 $destination[2]

    load %r1 $command_buffer[3]
    store %r1 $destination[3]

    end


write_string:
    syscall $SYSCALL_WRITE_STRING
    end

fun_write_char:
    load %r1 0x8
    load %r2 0x10
    aor %r3 $address
    end

_main:
    call copy_destination
    end
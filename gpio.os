define $SYSCALL_COMMAND 0x09

dclr $command_buffer[4]

memory $command_buffer[0xf0, 0xd1, 0xb3, 0xa4]

gpio_init{gpio_test}:
    aor %r1 $command_buffer
    syscall $SYSCALL_COMMAND
    end

_main:
    call gpio_init
    end
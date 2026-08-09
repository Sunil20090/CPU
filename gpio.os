define $SYSCALL_RUN_COMMAND 0x03
define $SYSCALL_DELAY_1_SEC 0x06
define $SYSCALL_RUN_COMMAND_WITH_RETURN 0x05
define $SYSCALL_RUN_COMMAND_DATA_CAPTURE 0x04

dclr $command_gpio_init[4]
dclr $command_gpio_write_high[4]
dclr $command_gpio_write_low[4]

dclr $command_uart_init[4]
dclr $command_uart_recieve[4]
dclr $command_uart_transmit[4]

dclr $data_to_transmit[10]

memory $data_to_transmit["_Hello_OS"]


memory $command_gpio_init[0x10,0x3D,0x10,0x00]
memory $command_gpio_write_high[0x11, 0x3D, 0x00, 0x00]
memory $command_gpio_write_low[0x11, 0x3D,0x10,0x00]

memory $command_uart_init[0x20, 0x00, 0x20, 0x00]
memory $command_uart_recieve[0x22, 0x00, 0x20, 0x00]
memory $command_uart_transmit[0x21, 0x00, 0x20, 0x00]

dclr $blink_counter
memory $blink_counter[0x0A]

gpio_init{GPIO_INIT}:
    aor $command_gpio_init
    syscall $SYSCALL_RUN_COMMAND
    end

gpio_write_high{GPIO_HIGH}:
    aor $command_gpio_write_high
    syscall $SYSCALL_RUN_COMMAND
    end

gpio_write_low{GPIO_LOW}:
    aor $command_gpio_write_low
    syscall $SYSCALL_RUN_COMMAND
    end

fun_uart_init:
    aor $command_uart_init
    syscall $SYSCALL_RUN_COMMAND
    end

fun_uart_recieve:
    aor $command_uart_recieve
    syscall $SYSCALL_RUN_COMMAND
    end


delay_loop_100ms:
    dec %r2
    comp %r2 0x00
    syscall $SYSCALL_DELAY_1_SEC
    caln delay_loop_100ms
    load %r2 0x00
    end

delay_1s:
    load %r2 0x01
    call delay_loop_100ms
    end

blink_loop:
    call gpio_write_low
    call delay_1s
    call gpio_write_high
    call delay_1s
    
    dec $blink_counter
    load %r2 $blink_counter
    comp %r2 0x00
    caln blink_loop
    load %r2 0x00
    end
 
fun_uart_transmit:
    load %r2 0x08
    store %r2 $data_to_transmit

    aor %r1 $command_uart_transmit
    syscall $SYSCALL_RUN_COMMAND
    end

 fun_init_io:
    call gpio_init
    call fun_uart_init
    end



_main:
    call fun_init_io

    call blink_loop

    end
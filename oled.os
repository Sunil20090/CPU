dclr $address[4]
dclr $destination[6]


;syscalls data
define $SYSCALL_WRITE_STRING 0x05


memory $address["Hel"]
memory $destination["Suninl"]


write_string:
    syscall $SYSCALL_WRITE_STRING
    end

fun_write_char:
    load %r1 0x8
    load %r2 0x10
    aor %r3 $address
    end

_main:
    call write_string
    end
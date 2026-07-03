dclr $choice
dclr $a_value
dclr $b_value

print_a:
    load %r1 'A'
    store %r1 $a_value
    aor %r1 $a_value
    syscall 0x00
end


print_b:
    load %r1 'B'
    store %r1 $b_value
    aor %r1 $b_value
    syscall 0x00
end

take_input:
    aor %r1 $choice
    syscall 0x01
end

_main:
    call take_input

    load %r2 '1'
    comp %r2 $choice
    calz print_a

    load %r2 '2'
    comp %r2 $choice
    calz print_b

end
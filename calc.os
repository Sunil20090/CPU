dclr $first_number
dclr $second_number
dclr $type
dclr $message[20]
dclr $result


convert_number:
    load %r2 '0'
    clr
    add %r1
    sub %r2
    mov $result
    end

print_quest:
    load %r1 'E'
    store %r1 $message[0]
    load %r1 'n'
    store %r1 $message[1]
    load %r1 't'
    store %r1 $message[2]
    load %r1 'e'
    store %r1 $message[3]
    load %r1 'r'
    store %r1 $message[4]
    load %r1 '-'
    store %r1 $message[5]

    aor %r1 $message
    load %r2 0x06
    syscall 0x04
    end

show_result:
    aor %r1 $result
    syscall
    end

add_number:
    load %r1 $first_number
    
    clr
    add %r1
    add $second_number
    load %r2 '0'
    add %r2
    mov $result
    call show_result
    end

subtract_number:
    load %r1 $first_number
    
    clr
    add %r1
    sub $second_number
    load %r2 '0'
    add %r2
    mov $result
    call show_result
    end

take_inputs:

    call print_quest
    aor %r1 $first_number
    syscall 0x01

    ;converting from char to int
    load %r1 $first_number
    call convert_number
    load %r1 $result
    store %r1 $first_number


    call print_quest
    aor %r1 $second_number
    syscall 0x01


    ;converting from char to int
    load %r1 $second_number
    call convert_number
    load %r1 $result
    store %r1 $second_number
    end

start_addition:
    call take_inputs
    call add_number
    end

take_type:
    aor %r1 $type
    syscall 0x01
    end


end_of_program:
    load %r1 'E'
    store %r1 $message[0]
    load %r1 'n'
    store %r1 $message[1]
    load %r1 'd'
    store %r1 $message[2]
    load %r1 '-'
    store %r1 $message[3]
    load %r1 'p'
    store %r1 $message[4]
    load %r1 'r'
    store %r1 $message[5]
    load %r1 'o'
    store %r1 $message[6]
    load %r1 'g'
    store %r1 $message[7]

    aor %r1 $message
    load %r2 0x08
    syscall 0x04
    end


start_subtract:
    call take_inputs
    call subtract_number
    end


show_type_message:
    load %r1 '['
    store %r1 $message[0]
    load %r1 '+'
    store %r1 $message[1]
    load %r1 '/'
    store %r1 $message[2]
    load %r1 '-'
    store %r1 $message[3]
    load %r1 '/'
    store %r1 $message[4]
    load %r1 '*'
    store %r1 $message[5]
    load %r1 ']'
    store %r1 $message[6]

    aor %r1 $message
    load %r2 0x08
    syscall 0x04
    end


multiply_loop:
    add $second_number
    dec %r1
    comp %r1 0x00
    caln multiply_loop
    load %r2 '0'
    add %r2
    mov $result
    call show_result
    end

multiply:
    load %r1 $first_number
    clr
    call multiply_loop
    end

start_multiply:
    call take_inputs
    call multiply
    end

reset_register:
    load %r1 0x00
    load %r2 0x00
    load %r3 0x00
    load %r4 0x00
    end

start_calc:

    call show_type_message
    call take_type

    load %r1 '+'
    comp %r1 $type
    calz start_addition

    load %r1 '-'
    comp %r1 $type
    calz start_subtract

    load %r1 'm'
    comp %r1 $type
    calz start_multiply

    call end_of_program
    call start_calc
    end

_main:

    call start_calc

    end
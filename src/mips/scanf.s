_f_scanf_S0__sig_1:
    ADDIU $sp, $sp, -8
    SW $ra, 4($sp)
    SW $fp, 0($sp)
    MOVE $fp, $sp

    addu $a0, $a0, $sp   # Adjust $a0 if needed

    lw $t0, 0($a0)       # Load format string address
    addiu $a0, $a0, -4   # $t2 will point to addresses where inputs are to be stored
    move $t2, $a0

next_char_scanf:
    lb $t3, 0($t0)       # Load current char
    beqz $t3, done_scanf

    li $t4, '%'
    bne $t3, $t4, skip_nonformat_scanf

    addiu $t0, $t0, 1
    lb $t3, 0($t0)

    beq $t3, 'd', read_int_scanf
    beq $t3, 's', read_string_scanf
    beq $t3, 'c', read_char_scanf
    beq $t3, 'f', read_float_scanf

skip_nonformat_scanf:
    addiu $t0, $t0, 1
    j next_char_scanf

read_int_scanf:
    li $v0, 5          # syscall 5: read int
    syscall
    lw $t1, 0($t2)     # Load address where to store int
    sw $v0, 0($t1)     # Store input value

    addiu $t2, $t2, -4
    addiu $t0, $t0, 1
    j next_char_scanf

read_string_scanf:
    li $v0, 8          # syscall 8: read string
    lw $a0, 0($t2)     # Load address where to store string
    li $a1, 100        # Max length of input (adjust if needed)
    syscall

    addiu $t2, $t2, -4
    addiu $t0, $t0, 1
    j next_char_scanf

read_char_scanf:
    li $v0, 12         # syscall 12: read char
    syscall
    lw $t1, 0($t2)     # Load address where to store char
    sb $v0, 0($t1)     # Store input byte

    addiu $t2, $t2, -4
    addiu $t0, $t0, 1
    j next_char_scanf

read_float_scanf:
    li $v0, 6          # syscall 6: read float
    syscall
    lw $t1, 0($t2)     # Load address where to store float
    swc1 $f0, 0($t1)   # Store float into memory

    addiu $t2, $t2, -4
    addiu $t0, $t0, 1
    j next_char_scanf

done_scanf:
    LW $fp, 0($sp)
    LW $ra, 4($sp)
    ADDIU $sp, $sp, 8
    JR $ra
    
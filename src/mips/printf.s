.data
	fmt:    .asciiz "Hello %s, your score is %d and grade is %c and GPA is %f\n"
	name:   .asciiz "Alice"
	.align 2   # Align to 4-byte boundary

.text
.global _start
_start:
	jal main
	li $v0, 10
	syscall

main:
    la $t0, fmt
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    la $t0, name
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    li $t0, 95
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    li $t0, 'A'
    addiu $sp, $sp, -1
    sb $t0, 0($sp)
						   # Push float 3.75
    li.s $f0, 3.75         # load float literal
    mfc1 $t0, $f0          # move float bits to integer register
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    move $a0, $sp          # $a0 -> 'A'
	addiu $a0, $a0, 13
    jal printf
	nop

    addiu $sp, $sp, 17

    li $v0, 10
    syscall


_f_printf_S0__sig_1:
    ADDIU $sp, $sp, -8
    SW $ra, 4($sp)
    SW $fp, 0($sp)
    MOVE $fp, $sp

	addu $a0, $a0, $sp

    lw $t0, 0($a0)
	
	addiu $a0, $a0, -4
    move $t2, $a0

next_char_printf:
    lb $t3, 0($t0)
    beqz $t3, done_printf

    li $t4, '%'
    beq $t3, $t4, handle_format_printf

    li $v0, 11
    move $a0, $t3
    syscall

    addiu $t0, $t0, 1
    j next_char_printf

handle_format_printf:
    addiu $t0, $t0, 1
    lb $t3, 0($t0)

    beq $t3, 'd', print_int_printf
    beq $t3, 's', print_string_printf
    beq $t3, 'c', print_char_printf
	beq $t3, 'f', print_float_printf

    li $v0, 11
    li $a0, '%'
    syscall
    move $a0, $t3
    syscall

    addiu $t0, $t0, 1
    j next_char_printf

print_int_printf:
    lw $a0, 0($t2)
    li $v0, 1
    syscall

    addiu $t2, $t2, -4    # move to next argument (backward)
    addiu $t0, $t0, 1
    j next_char_printf

print_string_printf:
    lw $a0, 0($t2)
    li $v0, 4
    syscall

    addiu $t2, $t2, -4
    addiu $t0, $t0, 1
    j next_char_printf

print_char_printf:
    lb $a0, 0($t2)
    li $v0, 11
    syscall

    addiu $t2, $t2, -4
    addiu $t0, $t0, 1
    j next_char_printf
	
print_float_printf:
    lw $t1, 0($t2)         # Load float bits into $t1
    mtc1 $t1, $f12         # Move bits into floating-point register $f12
    li $v0, 2              # syscall 2: print float
    syscall

    addiu $t2, $t2, -4
    addiu $t0, $t0, 1
    j next_char_printf

done_printf:
	LW $fp, 0($sp)
    LW $ra, 4($sp)
    ADDIU $sp, $sp, 8
    JR $ra
    jr $ra

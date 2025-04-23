.data
	fmt:    .asciiz "Hello %s, %s your score is %d/%d and grade is %c\n"
	name:   .asciiz "Alice"
	.align 2   # Align to 4-byte boundary

.text
.global _start
_start:
	jal main
	li $v0, 10
	syscall		# Use syscall 10 to stop simulation


# Usage of printf in main

main:
    li $t0, 'A'
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

	li $t0, 100
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    li $t0, 95
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    la $t0, name
    addiu $sp, $sp, -4
    sw $t0, 0($sp)
	
	la $t0, name
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    la $a0, fmt         # format string
    move $a1, $sp       # pointer to first argument
    jal printf

	addiu $sp, $sp, 12  # Clean up stack after printf
    li $v0, 10
    syscall             # exit


# Begin printf implementation

printf:
    # $a0 = format string
    # $a1 = pointer to arguments on stack

    move $t0, $a0       # t0 = format string pointer
    move $t1, $a1       # t1 = stack argument pointer

next_char_printf:
    lb $t2, 0($t0)      # load current char
    beqz $t2, done_printf      # end of string

    li $t3, '%'
    beq $t2, $t3, handle_format_printf

    # Print normal char
    li $v0, 11
    move $a0, $t2
    syscall

    addiu $t0, $t0, 1
    j next_char_printf

handle_format_printf:
    addiu $t0, $t0, 1       # move to format specifier
    lb $t2, 0($t0)

    beq $t2, 'd', print_int_printf
    beq $t2, 's', print_string_printf
    beq $t2, 'c', print_char_printf

    # Unknown specifier: print it literally
    li $v0, 11
    li $a0, '%'
    syscall
    move $a0, $t2
    syscall

    addiu $t0, $t0, 1
    j next_char_printf

print_int_printf:
    lw $a0, 0($t1)
    li $v0, 1
    syscall

    addiu $t1, $t1, 4       # move to next argument
    addiu $t0, $t0, 1
    j next_char_printf

print_string_printf:
    lw $a0, 0($t1)
    li $v0, 4
    syscall

    addiu $t1, $t1, 4
    addiu $t0, $t0, 1
    j next_char_printf

print_char_printf:
    lw $a0, 0($t1)
    li $v0, 11
    syscall

    addiu $t1, $t1, 4
    addiu $t0, $t0, 1
    j next_char_printf

done_printf:
    jr $ra

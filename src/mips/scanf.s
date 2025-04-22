# Use .set noreorder to prevent the assembler from filling branch
# delay slots, if you want to fill delay slots manually.
# .set noreorder
.data
	printf_str:		.asciiz "Hey %s, your score is %d and grade is %c\n"
	name:			.asciiz "Alice"
	scanf_str:		.asciiz "%s %d %c"
	str_var:		.space 100
	int_var:		.word 0
	char_var:		.space 1
					.align 2   # Align to 4-byte boundary

.text
.global _start
_start:
	jal main
	li $v0, 10
	syscall		# Use syscall 10 to stop simulation

main:
	
	la $t0, char_var
    addiu $sp, $sp, -4
    sw $t0, 0($sp)
	
	la $t0, int_var
    addiu $sp, $sp, -4
    sw $t0, 0($sp)
	
	la $t0, str_var
    addiu $sp, $sp, -4
    sw $t0, 0($sp)
	
    la $a0, scanf_str
    move $a1, $sp
    jal scanf
	
	addiu $sp, $sp, 12
	
	lb $t0, char_var     # Load char
    li $t1, 0            # Clear temp
    addiu $sp, $sp, -4
    sw $t1, 0($sp)       # Clear 4 bytes
    sb $t0, 0($sp)       # Store byte into lowest byte of that word
	
	la $t1, int_var
	lw $t0, 0($t1)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)
	
	la $t0, str_var
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

	la $a0, printf_str
    move $a1, $sp
    jal printf
	
	addiu $sp, $sp, 12
	
	li $v0, 10
    syscall             # exit

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


scanf:
    # $a0 = format string
    # $a1 = pointer to stack args (destination addresses)

    move $t0, $a0       # Format string pointer
    move $t1, $a1       # Arg destination pointer

next_char_scanf:
    lb $t2, 0($t0)
    beqz $t2, done_scanf    # End of format string

    li $t3, '%'
    beq $t2, $t3, handle_format_scanf

    # Skip normal characters like whitespace
    addiu $t0, $t0, 1
    j next_char_scanf

handle_format_scanf:
    addiu $t0, $t0, 1       # move to specifier
    lb $t2, 0($t0)

    beq $t2, 'd', read_int_scanf
    beq $t2, 's', read_string_scanf
    beq $t2, 'c', read_char_scanf

    # Unknown specifier: skip
    addiu $t0, $t0, 1
    j next_char_scanf

read_int_scanf:
    li $v0, 5           # syscall: read_int_scanf
    syscall
    lw $t4, 0($t1)      # address to store value
    sw $v0, 0($t4)

    addiu $t1, $t1, 4
    addiu $t0, $t0, 1
    j next_char_scanf

read_string_scanf:
    lw $a0, 0($t1)      # buffer address
    li $a1, 100         # max input length
    li $v0, 8           # syscall: read_string_scanf
    syscall

    addiu $t1, $t1, 4
    addiu $t0, $t0, 1
    j next_char_scanf

read_char_scanf:
    li $v0, 12          # syscall: read_char_scanf
    syscall

check_whitespace_scanf:
    # Check if the character is a whitespace or newline
    li $t2, 32          # ASCII value for ' '
    beq $v0, $t2, read_again_scanf  # If it's a space, read again

    li $t2, 10          # ASCII value for '\n'
    beq $v0, $t2, read_again_scanf  # If it's a newline, read again

    # If it's not a whitespace or newline, store it
    lw $t4, 0($t1)      # Load address where the char will be stored
    sb $v0, 0($t4)      # Store character

    addiu $t1, $t1, 4   # Move to next argument location on stack
    addiu $t0, $t0, 1   # Move to the next format specifier in the format string
    j next_char_scanf

read_again_scanf:
    # Read another character
    li $v0, 12          # syscall: read_char_scanf
    syscall
    j check_whitespace_scanf  # Check again for whitespace or newline

done_scanf:
    jr $ra
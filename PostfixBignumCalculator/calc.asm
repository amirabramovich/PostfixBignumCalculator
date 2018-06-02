global addition
global subtraction
global multiply
global division

%define A1 rdi
%define A2 rsi
%define result rdx
%define A1_digits rcx
%define A2_digits r8
%define loops r9
%define A1_digit r11b
%define A2_digit r12b
%define last_digit r13
%define carry r14b
%define carry_helper r15b

section .data

extern check_result
var1 DQ 0


section .text	
        
addition:
        push rbp          
        mov rbp, rsp      
        mov rax, 0 ;no fp
        mov carry, 0 ;init carry
        mov rax, A2_digits ; num of loops
        jmp add_init
    
add_init:
        add result, A1_digits ;go to last char
        inc result ;result is larger by 1 than A1
        mov byte[result], 0 ;for end of string
        add A1, A1_digits ;go to last char
        add A2, A2_digits ;go to last char
        jmp digits_addition
        
digits_addition:
	cmp rax, 0 ;check if end of digits minimum num of loops
	je finish_addition
	dec rax ;loop--
        dec result ;go to last digit wasnt calculated
        dec A1 ;go to last digit wasnt calculated
        dec A2 ;go to last digit wasnt calculated
        mov A1_digit, [A1] ;place digit in A1_digit
        mov A2_digit, [A2] ;place digit in A2_digit
        sub A1_digit, 48  ;convert digit from ascii
        sub A2_digit, 48  ;convert digit from ascii
        add A1_digit, carry ;add the carry to A1_digit
        mov carry, 0 ;re-use the carry
        add A1_digit, A2_digit ;make addition of 2 numbers(digits)
        mov carry_helper, A1_digit ;copy result to carry_helper
        sub carry_helper, 9 ;check if addition>10
        jg carry_function ;if addition>10 jump to carry
        jmp continue_add ;else continue addition.
        
continue_add:
        add A1_digit, 48 ;convert digit back to ascii
	mov [result], A1_digit ;place the result of the addition
        jmp digits_addition
        
carry_function:
        sub A1_digit, 10 ;update A1_digit=A1_digit-10
        mov carry, 1 ;update carry the carry
        jmp continue_add
        
finish_addition:  ;copy last digits
        sub A1_digits, A2_digits
	jg copy_digits
	jmp done_addition
	
done_addition:
        dec result;go to first digit(from left) in result
        add carry, 48
        mov byte[result], carry ;if dont have carry msb is 0, if have msb is 1
        mov rsp, rbp
        pop rbp
        ret
        
copy_digits:
        cmp A1_digits, 0 ;check if end of digits
        je done_addition
        dec result
        dec A1
        mov A1_digit, [A1]
        sub A1_digit, 48 ;to digit from ascii
        add A1_digit, carry
        mov carry, 0
        mov carry_helper, A1_digit ;copy result to carry_helper
        sub carry_helper, 9 ;check if addition>10
        jg add_carry
        add A1_digit, 48
        mov [result], A1_digit
        dec A1_digits
        jmp copy_digits
            
add_carry:
        sub A1_digit, 10 ;update A1_digit=A1_digit-10
        mov carry, 1 ;update carry the carry
        add A1_digit, 48
        mov [result], A1_digit
        dec A1_digits
        jmp copy_digits
        
;--------------------------------------------------------

subtraction:
        push rbp          
        mov rbp, rsp      
        mov rax, 0 ;no fp
        mov carry, 10 ;init carry
        mov rax, A2_digits ; num of loops
        jmp sub_init
        
sub_init:
        add result, A1_digits ;go to last char
        inc result ;result is larger by 1 than A1
        mov byte[result], 0 ;for end of string
        add A1, A1_digits ;go to last char
        add A2, A2_digits ;go to last char
        
digits_subtraction:
	cmp rax, 0 ;check if end of digits (min num of loops)
	je finish_subtraction
	dec rax
	dec result ;go to last digit
        dec A1 ;go to last digit
        dec A2 ;go to last digit
        mov A1_digit, [A1] ;place digit in A1_digit
        mov A2_digit, [A2] ;place digit in A2_digit
        sub A1_digit, 48  ;convert digit from ascii
        sub A2_digit, 48  ;convert digit from ascii
        add A1_digit, carry ;add the carry to A1_digit
        mov carry, 9 ;re-use the carry
        sub A1_digit, A2_digit ;make subtraction of 2 numbers(digits)
        mov carry_helper, A1_digit ;copy result to carry_helper
        sub carry_helper, 9 ;check if subtraction>10
        jg subcarry ;if subtraction>10 jump to carry
        jmp continue_sub ;else continue subtraction.
        
continue_sub:
        add A1_digit, 48 ;convert digit back to ascii
	mov [result], A1_digit ;place the result of the addition
        jmp digits_subtraction
        
subcarry:
        sub A1_digit, 10 ;update A1_digit=A1_digit-10
        mov carry, 10 ;update carry the carry
        jmp continue_sub
        
finish_subtraction:
        dec result
        cmp A2_digits, A1_digits
        jge done_subtraction
        dec A1
        mov A1_digit, [A1] ;place digit in A1_digit
        sub A1_digit, 48  ;convert digit from ascii
        add A1_digit, carry ;add the carry to A1_digit
        mov carry, 9 ;re-use the carry
        mov carry_helper, A1_digit ;copy result to carry_helper
        sub carry_helper, 9 ;check if subtraction>10
        jg finish_subcarry ;if subtraction>10 jump to carry
        jmp continue_subt ;else continue subtraction.
        
finish_subcarry:
        sub A1_digit, 10 ;update A1_digit=A1_digit-10
        mov carry, 10 ;update carry the carry
        jmp continue_subt
        
continue_subt:
        add A1_digit, 48 ;convert digit back to ascii
	mov [result], A1_digit ;place the result of the addition
	dec A1_digits
        jmp finish_subtraction

done_subtraction:
        mov byte[result], 48
        mov rsp, rbp
        pop rbp
        ret
        
;--------------------------------------------------------

multiply:
        push rbp          
        mov rbp, rsp      
        mov rax, 0 ;no fp
        mov carry, 0 ;init carry
        mov loops, A2_digits;loops = A2 num of digits, outer loop
        mov [var1], A1_digits ; num of main loops = A1 num of digits, inner loop
        mov r15, A1_digits
        mov r12, 1
        add result, A1_digits ;go to last char
        add result, A2_digits ;go to last char
        mov byte[result], 0 ;for end of string
        mov r10, result ;for saving the result
        add A1, A1_digits ;go to last char
        add A2, A2_digits ;go to last char
        mov last_digit, A1
        dec A2 ;go to last digit wasnt calculated
        jmp row_multiply
        
row_multiply:
        cmp loops, 0 ;check if finished multiply all rows
        je finish_multiply_init
	cmp qword[var1], 0 ;check if finished multiply a row
	je finish_row
	dec qword[var1] ;innerloop--
        dec result ;go to last digit wasnt calculated
        dec A1 ;go to last digit wasnt calculated
        mov al, [A1] ;place digit in al
        mov bl, [A2] ;place digit in bl
        sub al, 48  ;convert digit from ascii
        sub bl, 48  ;convert digit from ascii
        mul bl ;al=al*bl
        add al, [result] ;adds to al the result from previous rows (outer loop)
        add al, carry ;adds to al the carry from current row (inner loop)
        mov carry, 0 ;reset carry
        cmp al, 9 ;if al>10
        jg carry_mul_func
        mov [result], al ;place the result of the addition
        jmp row_multiply
        
carry_mul_func:
        mov ch, 10
        mov ah, 0
        div ch ;update al=al/10 -> ah has the reminder
        mov carry, al ;update the carry
        mov al, ah ;now al is the reminder
        mov [result], al ;place the result of the addition
        jmp row_multiply
        
finish_row:
        cmp carry, 0
        jg insert_carry
        dec loops ;loop--
        dec A2 ;go to last digit wasnt calculated
        mov result, r10
        sub result, r12 ;go to last digit in result
        inc r12
        mov A1, last_digit
        mov [var1], r15 ; num of main loops = A1 num of digits, inner loop
        jmp row_multiply
        
insert_carry:
        dec result
        mov [result], carry
        mov carry, 0
        jmp finish_row
        
finish_multiply_init: 
        mov result, r10 ;r10 in the end of result
        mov loops, A2_digits
        add loops, r15
        jmp finish_multiply
        
finish_multiply:
        cmp loops, 0 ;if result reach to begining
        je done_multiply
        dec loops
        dec result
        mov al, [result]
        add al, carry
        mov carry, 0
        mov carry_helper, al ;copy result to carry_helper
        sub carry_helper, 9 ;check if addition>10
        jg carry_mul ;if addition>10 jump to carry
        jmp continue_finish_mul
        
carry_mul:
        mov ch, 10
        mov ah, 0
        div ch ;update al=al/10 -> ah has the reminder
        mov carry, al ;update the carry
        mov al, ah ;now al is the reminder
        jmp continue_finish_mul
        
continue_finish_mul:
        add al, 48 ;convert digit back to ascii
	mov [result], al ;place the result of the addition
        jmp finish_multiply
        
done_multiply:
        mov rsp, rbp
        pop rbp
        ret
        
;--------------------------------------------------------

    
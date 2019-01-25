; 8080 assembler code
.hexfile F1.hex
.binfile F1.com
; try "hex" for downloading in hex format
.download bin
.objcopy gobjcopy
.postbuild echo "OK!"
;.nodump

; OS call list
PRINT_B		equ 1
PRINT_MEM	equ 2
READ_B		equ 3
READ_MEM	equ 4
PRINT_STR	equ 5
READ_STR	equ 6
GET_RND     equ 7
TExit       equ 8
TJoin       equ 9
TYield      equ 10
TCreate     equ 11

; Position for stack pointer
stack   equ 0F000h

org 000H
jmp begin

; Start of our Operating System
GTU_OS:	PUSH B
push D
push H
push psw
nop	; This is where we run our OS in C++, see the CPU8080::isSystemCall()
; function for the detail.
pop psw
pop H
pop D
pop B
register
; ---------------------------------------------------------------
; YOU SHOULD NOT CHANGE ANYTHING ABOVE THIS LINE

begin:
	LXI SP,stack 	; always initialize the stack pointer
	MVI D, 50		; BURDA kaldın
	MVI B, 1		; printing register
loop:
		MVI A, PRINT_B 	; print the b register
		call GTU_OS		; call the os
		INR B			; increment to next number
		DCR D			; decrement the counter
		JNZ loop		; while

		MVI A,TExit
		call GTU_OS
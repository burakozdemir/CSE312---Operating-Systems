; 8080 assembler code
.hexfile 1.hex
.binfile 1.com
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

org 1000H

F1:                 dw 'F1.bin',00AH,00H
F2:                 dw 'F2.bin',00AH,00H
F3:                 dw 'F3.bin',00AH,00H
F4:                 dw 'F4.bin',00AH,00H
F5:                 dw 'F5.bin',00AH,00H

; Position for stack pointer
stack   equ 0E000h

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
ret
; ---------------------------------------------------------------
; YOU SHOULD NOT CHANGE ANYTHING ABOVE THIS LINE


begin:
	LXI SP,stack 	; always initialize the stack pointer
	
	LXI B,F1
	MVI A,TCreate
	call GTU_OS
	
	LXI B,F3
	MVI A,TCreate
	call GTU_OS
	
	MVI B,1
	MVI A,TJoin
	call GTU_OS
	
	MVI B,2
	MVI A,TJoin
	call GTU_OS

	hlt

 

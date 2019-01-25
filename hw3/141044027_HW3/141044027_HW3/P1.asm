   ; 8080 assembler code
        .hexfile P1.hex
        .binfile P1.com
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
Texit		equ 8
TCreate 	equ 11
TJoin 		equ 9
TYield		equ 10
FileCreate equ 12
FileClose equ 13
FileOpen equ 14
FileRead equ 15
FileWrite equ 16
FileSeek equ 17
DirRead	equ 18
	; Position for stack pointer
stack   equ 0F000h

	org 000H
	jmp begin

	; Start of our Operating System
GTU_OS:	PUSH D
	push D
	push H
	push psw
	nop	; This is where we run our OS in C++, see the CPU8080::isSystemCall()
		; function for the detail.
	pop psw
	pop h
	pop d
	pop D
	ret
	; ---------------------------------------------------------------
	; YOU SHOULD NOT CHANGE ANYTHING ABOVE THIS LINE        
	
newline: 					dw 00AH,00H
numbers:                    dw '01234567891011121314151617181920212223242526272829303132333435363738394041424344454647484950', 00H


begin:
	LXI SP,stack 	; always initialize the stack pointer
	MVI A, READ_STR													
	call GTU_OS														
	PUSH B													  		; 
	MVI A, FileCreate												
	call GTU_OS														; 
	MVI A, 0 														
	POP B                                                           
	MVI A, FileOpen													;
	call GTU_OS														
	MOV E, B
	LXI B, numbers
	MVI A, FileWrite												
	call GTU_OS 													
	MOV B, E 														
	MVI A, FileClose                                                
	call GTU_OS														;
	hlt 															; end program

	
	
	
	
	
	
	
	

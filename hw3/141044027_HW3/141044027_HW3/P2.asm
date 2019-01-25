        ; 8080 assembler code
        .hexfile p2.hex
        .binfile p2.com
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
GET_RND		equ 7
TExit		equ 8
TJoin		equ 9
TYield		equ 10
TCreate		equ	11
FileCreate equ 12
FileClose equ 13
FileOpen equ 14
FileRead equ 15
FileWrite equ 16
FileSeek  equ 17
DirRead	 equ 18

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


newline: 					dw 00AH,00H 									; null terminated newline string
enter_file_name:			dw 'Enter file name to open file : ',00H 


begin:
	LXI SP,stack 													
	LXI B, enter_file_name											
	MVI A, PRINT_STR												
	call GTU_OS														
	MVI A, READ_STR													
	call GTU_OS														
	MVI A, FileOpen													
	call GTU_OS                                                     
	MOV E, B 														
	MVI A, 0 														
	
    MVI A, FileRead 												
	call GTU_OS 													
	hlt 															; end program
	

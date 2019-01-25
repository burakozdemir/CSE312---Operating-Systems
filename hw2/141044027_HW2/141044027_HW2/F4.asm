 
 
 
 ; 8080 assembler code
        .hexfile Sort.hex
        .binfile Sort.com
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

	; Position for stack pointer
stack   equ 0F000H

	org 0000H
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
	; this file sorts numbers with bubble sort
	

SWAP:
	push A
	MOV A,D
	STAX B
	pop A
	DCX B
	DCX B
	STAX B
	INX B
	INX B
	ret

	ORG 8000H
;53H, 2AH, 12H, 34H, 08H
START:	DW 00H


	ORG 0700H
size:	DB 50 ; number of items to compare
out: DB 50
in: DB 49

	ORG 00d0H
begin:
	LXI SP,stack 	; always initialize the stack pointer



	LXI D,START
	MVI H,50
generateRand:
	
	MVI A,GET_RND
	call GTU_OS
	MOV A,B
	STAX D
	INX D
	INX D
	DCR H 	    
	JNZ generateRand


SortStart:

	LDA out ;
	MOV H,A ;


outloop:
	
	LDA in 		; A<- j 
	MOV E,A     ; E<- A
	
	LXI B,START ; BC<- Start adress
	

innerloop:
	LDAX B      ; A<- BC[i]
	MOV D,A     ; D<- A

	INX B       ; BC++
	INX B
	LDAX B 		; A<-BC[j]

	CMP D       ; A-D(flag 0 if A greater)
	CC SWAP     ;
	DCR E 	    ;
	JNZ innerloop   ;
	DCR H       	;
	JNZ outloop  	;


	LXI B,START
	MVI D,50
PRINT_LOOP: 
	MVI A, PRINT_MEM
	call GTU_OS

	INX B ; go next address
	INX B ; ""

	DCR D
	JNZ PRINT_LOOP ; 


	hlt		; end program
	

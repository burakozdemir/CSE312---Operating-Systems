    	; 8080 assembler code
    	.hexfile test.hex
    	.binfile test.com
    	; try "hex" for downloading in hex format
    	.download bin  
    	.objcopy gobjcopy
    	.postbuild echo "OK!"
    	;.nodump

    ; OS call list
PRINT_B   	 equ 1
PRINT_MEM    equ 2
READ_B   	 equ 3
READ_MEM    equ 4
PRINT_STR    equ 5
READ_STR    equ 6
GET_RND     equ 7

    ; Position for stack pointer
stack   equ 0F000h

    org 000H
    jmp begin

    ; Start of our Operating System
GTU_OS:    PUSH D
    push D
    push H
    push psw
    nop    ; This is where we run our OS in C++, see the CPU8080::isSystemCall()
   	 ; function for the detail.
    pop psw
    pop h
    pop d
    pop D
    ret
    ; ---------------------------------------------------------------
    ; YOU SHOULD NOT CHANGE ANYTHING ABOVE THIS LINE   	 

    ;This program prints a null terminated string to the screen

testprintbmsg:      dw 'Testing Print_b b has address value start with 0 > ',00AH,00H     ; null terminated string
testprintmemmsg:    dw 'Testing printmem will print > T but in decimal so it is 84 > ',00AH,00H    ;
testReadBmsg:       dw 'Testing Read_b',00AH,00H    ;
testreadmemmsg:     dw 'Testing Read_mem',00AH,00H ;
testprintstrmsg:    dw 'Testing Print_str',00AH,00H;
testReadstrmsg:     dw 'Testing Read_str',00AH,00H ;
testRandom:         dw 'Testing GET_RND',00AH,00H;
OK:                 dw 'OK',00AH,00H
input:              dw  00

begin:
    LXI SP,stack        ; always initialize the stack pointer
    LXI B,testprintstrmsg ; put msg to regB
    MVI A,PRINT_STR       ; put print msg
    call GTU_OS         ; call OS
    LXI B,OK            ; OK msg
    call GTU_OS         ; call OS
    
    
    LXI B, testprintmemmsg ; put msg to console
    call GTU_OS         ; call OS
    MVI A,PRINT_MEM     ; put print mem opcode
    call GTU_OS         ; call OS
    LXI B,OK            ; put ok msg
    MVI A,PRINT_STR     ; put opcode print string
    call GTU_OS         ; call OS
    
    LXI B, testprintbmsg   ; put msg 
    MVI A,PRINT_STR     ; put opcode
    call GTU_OS         ; call OS
    MVI A,PRINT_B       ; put print_b 
    call GTU_OS         ; call OS
    LXI B,OK            ; put ok msg
    MVI A,PRINT_STR     ; put opcode print string
    call GTU_OS         ; call OS
    
    LXI B, testReadBmsg    ; put b to test msg
    MVI A,PRINT_STR     ; put opcode
    call GTU_OS         ; call OS
    MVI A, READ_B          ; put opcode read b 
    call GTU_OS         ; call OS
    MVI A, PRINT_B         ; print the readed
    call GTU_OS         ; call OS
    LXI B,OK            ; put ok msg
    MVI A,PRINT_STR     ; put opcode print string
    call GTU_OS         ; call OS
    
    LXI B,testReadstrmsg  ; put read str test msg
    MVI A,PRINT_STR     ; put opcode
    call GTU_OS         ; call OS
    LXI B, input        ; ready a buffer
    MVI A,READ_STR        ; put opcode read str 
    call GTU_OS         ; call OS
    MVI A,PRINT_STR       ; print the readed
    call GTU_OS         ; call OS
    LXI B,OK            ; put ok msg
    MVI A,PRINT_STR     ; put opcode print string
    call GTU_OS         ; call OS
    
    LXI B,testreadmemmsg  ; put b to test msg
    MVI A,PRINT_STR     ; put opcode
    call GTU_OS         ; call OS
    LXI B,input		   ; read in input
    MVI A,READ_MEM        ; put opcode read mem 
    call GTU_OS         ; call OS
    LXI B, input    	; print in input 
    MVI A,PRINT_MEM       ; print the readed
    call GTU_OS         ; call OS
    LXI B,OK            ; put ok msg
    MVI A,PRINT_STR     ; put opcode print string
    call GTU_OS         ; call OS
    
    LXI B, testRandom    ; put b to test msg
    MVI A, PRINT_STR     ; put opcode
    call  GTU_OS         ; call OS
    MVI A, GET_RND          ; put opcode read b 
    call GTU_OS         ; call OS
    MVI A, PRINT_B         ; print the readed
    call GTU_OS         ; call OS
    LXI B,OK            ; put ok msg
    MVI A,PRINT_STR     ; put opcode print string
    call GTU_OS    
    
    hlt			;halt program
    

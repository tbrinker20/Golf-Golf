; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 
FP RN 11 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
; R0=0,    then output "0"
; R0=3,    then output "3"
; R0=89,   then output "89"
; R0=123,  then output "123"
; R0=9999, then output "9999"
; R0=4294967295, then output "4294967295"
LCD_OutDec
Count      EQU  0
Num    EQU  4
	;callee save
	PUSH{R4-R9,FP,LR}
	;2) allocation
	SUB SP, #8
	MOV FP, SP
	
	;checking if num is 0
	CMP R0, #0
	BEQ display0
	
	;3) access
	STR R0, [FP, #Num]
	MOV R4, #0
	STR R4, [FP, #Count]
	MOV R5, #10 
	

Odloop	
	LDR R4,[FP, #Num]
	CMP R4, #0
	BEQ nextOD
	UDIV R6,R4,R5
	MUL R7,R5,R6
	SUB R8,R4,R7
	PUSH{R8}
	STR R6,[FP, #Num]
	LDR R6,[FP, #Count]
	ADD R6,#1
	STR R6,[FP, #Count]
	B Odloop

display0
	ADD R0,#0x30
	BL ST7735_OutChar
	B doneOD
nextOD
	LDR R4,[FP, #Count] 
	CMP R4, #0
	BEQ doneOD
	POP{R0}
	ADD R0,#0x30
	BL ST7735_OutChar
	SUB R4, #1
	STR R4, [FP, #Count]
	B nextOD	
	
doneOD
	;deallocation(getting past local vars in memory so u can pop registers vals that were pushed, not the local vars contents
	ADD SP,#8
	POP{R4-R9,FP,LR}
	BX LR

;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000"
;       R0=3,    then output "0.003"
;       R0=89,   then output "0.089"
;       R0=123,  then output "0.123"
;       R0=9999, then output "9.999"
;       R0>9999, then output "*.***"
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix

     BX   LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file

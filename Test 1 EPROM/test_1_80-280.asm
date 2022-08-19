; TEST 1
;    Test program for John Bell Z80 Computer 80-280.
;    Output a 16-bit count on PIO ports A and B.
;    Reverse engineered from EPROM on board.
;
; TEST 1 EPROM
;
; 15:32:41.785 -> Read 2716 EPROM.
; 15:32:41.785 -> 0000: 3E0FD302D3032100007DD3007CD30123
; 15:32:43.004 -> 0010: 18F7FFFFFFFFFFFFFFFFFFFFFFFFFFFF
; ...
;
; Sig Nin, July 27, 2022
;
	.ORG 	0000h
;
MODEOUT .EQU 00001111B
ADATA   .EQU 0
BDATA   .EQU 1
ACTRL   .EQU 2
BCTRL   .EQU 3
;
; Set PIO Ports A and B to output mode
     LD  A,MODEOUT     ; Mode control word: output mode - 00..1111
     OUT (ACTRL),A     ; Set port A mode
     OUT (BCTRL),A     ; Set port B mode
; Output 16-bit count on ports: low order on A, high order on B
     LD  HL,0000H      ; Set count to zero
L1   LD  A,L           ; Low order byte ..
     OUT (ADATA),A     ; .. on port A
     LD  A,H           ; High order byte ..
     OUT (BDATA),A     ; .. on port B
     INC  HL           ; Increment count
     JR   L1           ; Repeat
;
     .END

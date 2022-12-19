;
; CONSTANT ROM BASIC ROUTINE
;
ROM_MSGOUT:     EQU     52EDH
;
; CONSTANT ROM BASIC WORK AREA
;
DRIVE_COUNT:    EQU     0EDC7H
;
; CONSTANT IPL DATA
;
IPL_REBASE_TOP: EQU     9000H
;
                ORG     0C000H
_PROG_TOP:
START_IPL:
                LD      HL,_PROG_TOP
                LD      DE,IPL_REBASE_TOP
                LD      BC,0100H
                LDIR
                LD      HL,IPL_MSG0 - ( _PROG_TOP - IPL_REBASE_TOP )
                CALL    ROM_MSGOUT
                XOR     A
                LD      (DRIVE_COUNT),A
                SCF
                RET
;
IPL_MSG0:       DB      "Hello World!", 0DH, 0AH, 0DH, 0AH, 0
_PROG_END:      
                DS      256 - ( _PROG_END - _PROG_TOP ), 0FFH
                END



;
; Interface to XMBurner
;
; Copyright (C) 2017 Sandor Zsuga (Jubatian)
;
;  This program is free software: you can redistribute it and/or modify
;  it under the terms of the GNU General Public License as published by
;  the Free Software Foundation, either version 3 of the License, or
;  (at your option) any later version.
;
;  This program is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License
;  along with this program.  If not, see <http://www.gnu.org/licenses/>.
;
;
; Interfacing with XMBurner, application side components.
;



.section .data

; Error report

xmbu_error_report:
	.space 3



.section .text


;
; XMBurner fault handler. Since this is a diagnostic application, this returns
; normally to the caller, so it may attempt to display diagnostic info. Or it
; may totally fail.
;
.global xmb_fault
xmb_fault:
	sts   xmbu_error_report + 1, r24
	sts   xmbu_error_report + 2, r25
	ldi   r24,     0xFF
	sts   xmbu_error_report + 0, r24
	jmp   xmb_glob_tail



;
; Clears error report
;
.global xmbu_error_clear
xmbu_error_clear:
	ldi   r24,     0x00
	sts   xmbu_error_report + 0, r24
	ret



;
; Returns whether XMBurner detected something (nonzero if so)
;
.global xmbu_iserror
xmbu_iserror:
	lds   r24,     xmbu_error_report + 0
	sts   0x003A,  r24
	cpi   r24,     0x00
	breq  .+2
	ldi   r24,     0x01
	ldi   r25,     0x00
	ret



;
; Returns error code as reported by XMBurner
;
.global xmbu_geterror
xmbu_geterror:
	lds   r24,     xmbu_error_report + 1
	lds   r25,     xmbu_error_report + 2
	ret



;
; Binary size for CRC
;
.global xmb_bsize
xmb_bsize:

	.byte (BINSIZE      ) & 0xFF
	.byte (BINSIZE >>  8) & 0xFF
	.byte (BINSIZE >> 16) & 0xFF
	.byte (BINSIZE >> 24) & 0xFF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; File:		io.asm
;; 
;; Copyright 2020 Scott Maday
;; You should have received a copy of the GNU General Public License along with this program. 
;; If not, see https://www.gnu.org/licenses/gpl-2.0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


GLOBAL	outb
outb:	; rdi=[port], rsi=[value]
	mov	rdx, rdi	; port 
	mov rax, rsi	; value
	out dx, al
	ret

GLOBAL	inb
inb:	; rdi=[port], al=return(value)
	mov	rdx, rdi	; port 
	in	al, dx
	ret
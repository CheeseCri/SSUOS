[BITS 16]

START:   
mov		ax, 0xb800
mov		es, ax
mov		ax, 0x00
mov		bx, 0
mov		cx, 80*25*2


CLS:
mov		[es:bx], ax
add		bx, 1
loop 	CLS

mov		ah, 0x09
mov 		al, 'H'
mov		[es:0000], ax
mov		al, 'e'
mov     [es:0002], ax
mov		al, 'l'
mov		[es:0004], ax
mov		al, 'l'		
mov		[es:0006], ax
mov		al, 'o'
mov		[es:0008], ax
mov		al, ','
mov		[es:0010], ax
mov		al, ' '
mov		[es:0012], ax
mov		al, 'K'
mov		[es:0014], ax
mov		al, 'e'
mov		[es:0016], ax
mov		al, 'o'
mov		[es:0018], ax
mov		al, 'n'
mov		[es:0020], ax
mov		al, 't'
mov		[es:0022], ax
mov		al, 'e'
mov		[es:0024], ax
mov		al, 'c'
mov		[es:0026], ax
mov		al, 'k'
mov		[es:0028], ax
mov		al, 39
mov		[es:0030], ax
mov		al, 's'
mov		[es:0032], ax
mov		al, ' '
mov		[es:0034], ax
mov		al, 'W'
mov		[es:0036], ax
mov		al, 'o'
mov		[es:0038], ax
mov		al, 'r'
mov		[es:0040], ax
mov		al, 'l'
mov		[es:0042], ax
mov		al, 'd'
mov		[es:0044], ax




org	0x7c00   

[BITS 16]

START:   
		jmp		PAINT ;PAINT로 점프

PAINT:
	mov	ax, 0xb800
	mov	es, ax
	mov	ax, 0x00
	mov	bx, 0
	mov	cx, 80*25*2
        
CLS:
	mov	[es:bx], ax
	add	bx, 1
	loop	CLS
DF:
        mov     bx, 0
        mov     si, 0

SSUOS1:
        mov     dl, byte[ssuos_1+si]
        mov     byte[es:bx], dl
        add     bx, 1
        add     si, 1
        mov     byte[es:bx], 0x07
        add     bx,1
        cmp     dl,0
        jne     SSUOS1

        mov     dl, '0'
        mov     byte[es:2], dl
        mov     byte[es:3], 0x07

        mov     bx, 160
        mov     si, 0

SSUOS2:
        mov     dl, byte[ssuos_2+si]
        mov     byte[es:bx], dl
        add     bx, 1
        add     si, 1
        mov     byte[es:bx], 0x07
        add     bx,1

        cmp     dl,0
        jne     SSUOS2

        mov     bx, 320
        mov     si, 0

SSUOS3:
        mov     dl, byte[ssuos_3+si]
        mov     byte[es:bx], dl
        add     bx, 1
        add     si, 1
        mov     byte[es:bx], 0x07
        add     bx, 1

        cmp     dl,0
        jne     SSUOS3


        mov     si, 0

        jmp     SELECT


        

SELECT:
        mov     ah, 0x00
        int     16h

        cmp     ah, 0x48
        je      UP

        cmp     ah, 0x50
        je      DOWN

        cmp     ah, 0x1c
        je      BOOT1_LOAD

DOWN:
        cmp     si, 2
        je     SELECT
        add     si, 1

        jmp     DRAW_SELECT

UP:
        cmp     si, 0
        je     SELECT
        sub     si, 1

        jmp     DRAW_SELECT

DRAW_SELECT:

        mov     bx, si
        mov     ax, 160
        mul     bx

        mov     bx, ax

        mov     dl, ' '
        mov     byte[es:2], dl
        mov     byte[es:3], 0x07

        mov     dl, ' '
        mov     byte[es:162], dl
        mov     byte[es:163], 0x07

        mov     dl, ' '
        mov     byte[es:322], dl
        mov     byte[es:323], 0x07

        mov     dl, '0'
        mov     byte[es:bx+2], dl
        mov     byte[es:bx+3], 0x07

        mov     bx, 0
        
        jmp     SELECT 


BOOT1_LOAD:
	mov     ax, 0x0900 
        mov     es, ax
        mov     bx, 0x0

        mov     ah, 2	
        mov     al, 0x4		
        mov     ch, 0	
        mov     cl, 2	
        mov     dh, 0		
        mov     dl, 0x80

        int     0x13	
        jc      BOOT1_LOAD

        cmp     si, 0
        je      KERNEL_LOAD

        cmp     si, 1
        je      KERNEL_LOAD1

        cmp     si, 2
        je      KERNEL_LOAD2

KERNEL_LOAD:
	mov     ax, 0x1000      ;loaded at es:bx	
        mov     es, ax	        ;0x1000:0000 = 0x1000
        mov     bx, 0x0		

        mov     ah, 2		;reading command
        mov     al, 0x3f	;secter numbers to read
        mov     ch, 0		;cylender num
        mov     cl, 0x6	        ;secter num
        mov     dh, 0          ;head 0num
        mov     dl, 0x80        ;drive num, 0x00 = A:, 0x80 = C:

        int     0x13            ;launch command
        jc      KERNEL_LOAD     ;retry when fail

        jmp             0x0900:0x0000


KERNEL_LOAD1:
        mov     ax, 0x1000      ;loaded at es:bx        
        mov     es, ax          ;0x1000:0000 = 0x1000
        mov     bx, 0x0         

        mov     ah, 2           ;reading command
        mov     al, 0x3f        ;secter numbers to read
        mov     ch, 9           ;cylender num
        mov     cl, 0x2F        ;secter num
        mov     dh, 14          ;head 0num
        mov     dl, 0x80        ;drive num, 0x00 = A:, 0x80 = C:

        int     0x13            ;launch command
        jc      KERNEL_LOAD     ;retry when fail

        jmp             0x0900:0x0000


KERNEL_LOAD2:
        mov     ax, 0x1000      ;loaded at es:bx        
        mov     es, ax          ;0x1000:0000 = 0x1000
        mov     bx, 0x0         

        mov     ah, 2           ;reading command
        mov     al, 0x3f        ;secter numbers to read
        mov     ch, 14           ;cylender num
        mov     cl, 0x7        ;secter num
        mov     dh, 14          ;head 0num
        mov     dl, 0x80        ;drive num, 0x00 = A:, 0x80 = C:

        int     0x13            ;launch command
        jc      KERNEL_LOAD     ;retry when fail

jmp		0x0900:0x0000

select db "[O]",0
ssuos_1 db "[ ] SSUOS_1",0
ssuos_2 db "[ ] SSUOS_2",0
ssuos_3 db "[ ] SSUOS_3",0
ssuos_4 db "[ ] SSUOS_4",0
partition_num : resw 1

times   446-($-$$) db 0x00

PTE:
partition1 db 0x80, 0x00, 0x00, 0x00, 0x83, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x3f, 0x0, 0x00, 0x00
partition2 db 0x80, 0x00, 0x00, 0x00, 0x83, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x3f, 0x0, 0x00, 0x00
partition3 db 0x80, 0x00, 0x00, 0x00, 0x83, 0x00, 0x00, 0x00, 0x98, 0x3a, 0x00, 0x00, 0x3f, 0x0, 0x00, 0x00
partition4 db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
times 	510-($-$$) db 0x00
dw	0xaa55

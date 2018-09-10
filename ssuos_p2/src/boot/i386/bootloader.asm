org	0x7c00   ;메모리의 몇 번지에서 실행해야 하는지 알려주는 선언문

[BITS 16]        ;이 프로그램이 16비트 단위로 데이터를 처리하는 프로그램임을 알림

START:   
		jmp		PAINT ;PAINT로 점프

PAINT:
	mov	ax, 0xb800 ;비디오 메모리 주소 저장
	mov	es, ax     ;비디오 메모리 주소 es에 저장
	mov	ax, 0x00   ;값들 초기화
	mov	bx, 0
	mov	cx, 80*25*2
        
CLS:                       ;화면 초기화 파트
	mov	[es:bx], ax
	add	bx, 1
	loop	CLS
DF:
        mov     bx, 0      ;bx값과 si값을 0으로 초기화 해줌.
        mov     si, 0

SSUOS1:
        mov     dl, byte[ssuos_1+si] ;데이터 섹션에 저장된 ssuos_1를 바이트 단위로 가져와 dl에 저장
        mov     byte[es:bx], dl      ;si는 인덱스의 역할을 수행, dl을 비디오메모리에 바이트 단위로 기입.
        add     bx, 1                ;bx, si의 값을 1 씩 증가 시킴으로써 비디오 메모리의 다음 단계에            
        add     si, 1                ;글자를 입력할 준비를 마침.
        mov     byte[es:bx], 0x07    ;비디오 메모리에 글자 양식을 입력
        add     bx,1                 ;bx에 1 증가.
        cmp     dl,0                 ;데이터를 모두 읽었을 경우 dl은 읽을 값이 없기에 0이 됨.
        jne     SSUOS1               ;0이 되지 않았을 경우 다시 SSUOS1로 돌아감.

        mov     dl, '0'              ;처음 선택 인터페이스를 표시할때 SSUOS1에 0이 위치하도록 함.
        mov     byte[es:2], dl
        mov     byte[es:3], 0x07

        mov     bx, 160              ;다음 행에 출력하기 위에 80*2의 값을 bx에 입력.
        mov     si, 0                ;인덱스의 역할을 하는 si의 값을 초기화 

SSUOS2:
        mov     dl, byte[ssuos_2+si] ;위의 내용을 SSUOS2와 3에 적용
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

        jmp     SELECT              ;3줄 모두 출력 한 후에 SELECT로 이동함.


        

SELECT:
        mov     ah, 0x00        ;ah에 00값을 넣고 인터럽트 16h를 실행
        int     16h             ;키 입력값을 받아 ah에 저장하게 됨.

        cmp     ah, 0x48        ;키 입력값이 0x48, 화살표 위 키와 같을 경우
        je      UP              ;UP으로 점프

        cmp     ah, 0x50        ;키 입력값이 0x50, 화살표 아래 키와 같을 경우
        je      DOWN            ;DOWN으로 점프

        cmp     ah, 0x1c        ;키 입력값이 0x1c, 엔터키와 같을 경우
        je      BOOT1_LOAD      ;BOOT1_LOAD로 점프

DOWN:                           ;DOWN으로 오면 현재 위치를 si 증가시킴으로서 추후 불러올 커널을 지정함.
        cmp     si, 2           ;현재 위치가 맨 밑일 경우 다시 SELECT로 이동하여 예외처리 함
        je     SELECT
        add     si, 1

        jmp     DRAW_SELECT     ;si 값에 따른 현재 상태를 그리기 위해 DRAW_SELECT로 점프

UP:
        cmp     si, 0           ;DOWN과 원리는 동일하나 si를 1 감소시키고 맨위에 있을 경우 예외처리를 함
        je     SELECT
        sub     si, 1

        jmp     DRAW_SELECT

DRAW_SELECT:                    ;선택한 커널에 0을 그리는 부분

        mov     bx, si          ;인덱스 값 si를 bx에 저장
        mov     ax, 160         ;ax에는 한줄에 80*2이므로 160을 저장
        mul     bx              ;bx와 ax를 곱한 값을 ax에 저장

        mov     bx, ax          ;곱해진 값을 bx에 다시 저장

        mov     dl, ' '         ;1, 2, 3번째 줄에 있는 0을 모두 빈칸으로 대체
        mov     byte[es:2], dl
        mov     byte[es:3], 0x07

        mov     dl, ' '
        mov     byte[es:162], dl
        mov     byte[es:163], 0x07

        mov     dl, ' '
        mov     byte[es:322], dl
        mov     byte[es:323], 0x07

        mov     dl, '0'         ;선택한 위치에 0을 출력하게됨.
        mov     byte[es:bx+2], dl
        mov     byte[es:bx+3], 0x07

        mov     bx, 0
        
        jmp     SELECT          ;다 그리고 난 후에는 SELECT로 다시 돌아가게됨.


BOOT1_LOAD:                     ;BOOT1 
	mov     ax, 0x0900      ;복사 목적지 주소 값 지정 es:bx, 0x0900
        mov     es, ax
        mov     bx, 0x0

        mov     ah, 2	        ;INT 13h AH = 02h : 드라이브에서 섹터를 읽는 명령.
        mov     al, 0x4		;4번 섹터 읽음.
        mov     ch, 0	        ;0번 실린더
        mov     cl, 2	        ;2번째 섹터부터 읽을 것
        mov     dh, 0		;0번 헤드
        mov     dl, 0x80        ;80번 드라이브.

        int     0x13	        ;INT 13H 실행
        jc      BOOT1_LOAD      ;에러 날 경우 다시 읽기 시도.

        cmp     si, 0           ;si, 인덱스 값과 비교해 어느 커널을 읽을지 결정.
        je      KERNEL_LOAD

        cmp     si, 1
        je      KERNEL_LOAD1

        cmp     si, 2
        je      KERNEL_LOAD2

KERNEL_LOAD:
	mov     ax, 0x1000      ;복사 목적지 주소 값 지정, 0x1000	
        mov     es, ax	        
        mov     bx, 0x0		

        mov     ah, 2		;인터럽트 13h의 읽기 명령
        mov     al, 0x3f	;읽을 섹터 지정
        mov     ch, 0		;실린더 번호
        mov     cl, 0x6	        ;섹터 번호
        mov     dh, 0           ;헤드 번호
        mov     dl, 0x80        ;드라이브 번호

        int     0x13            ;INT 13H 실행
        jc      KERNEL_LOAD     ;에러 날 경우 다시 읽기 시도

        jmp             0x0900:0x0000


KERNEL_LOAD1:                   ;커널 별로 주소 값을 다르게 해서 지정해놓음.
        mov     ax, 0x1000      
        mov     es, ax          
        mov     bx, 0x0         

        mov     ah, 2           
        mov     al, 0x3f        
        mov     ch, 9           
        mov     cl, 0x2F        
        mov     dh, 14          
        mov     dl, 0x80        

        int     0x13            
        jc      KERNEL_LOAD     

        jmp             0x0900:0x0000


KERNEL_LOAD2:
        mov     ax, 0x1000              
        mov     es, ax          
        mov     bx, 0x0         

        mov     ah, 2           
        mov     al, 0x3f        
        mov     ch, 14           
        mov     cl, 0x7        
        mov     dh, 14          
        mov     dl, 0x80       

        int     0x13            
        jc      KERNEL_LOAD    

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

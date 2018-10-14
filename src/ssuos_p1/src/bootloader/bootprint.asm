[BITS 16]

START:   
mov		ax, 0xb800	;비디오 메모리 지정.
mov		es, ax		;es에 비디오 메모리 저장
mov		ax, 0x00 	;ax 0으로 초기화
mov		bx, 0 		;bx 0으로 초기화
mov		cx, 80*25*2 ;화면 크기가 80*25이므로 80 * 25에 글자 용량이 2byte이기에 *2를 붙임.


CLS:
mov		[es:bx], ax ;es:bx 위치에 ax의 값, 0을 넣음
add		bx, 1		;그 이후 bx의 값을 1 증가 시킴
loop 	CLS			;bx의 값이 80 * 25 * 2가 될때까지 루프

mov		ah, 0x09	;ax의 왼쪽 8비트 레지스터에 문자 양식 저장.
mov 	al, 'H'		;ax의 오른쪽 8비트 레지스터에 문자 아스키 코드 저장.
mov		[es:0000], ax ;ax 레지스터를 비디오 메모리 주소에 넣음으로서 글자를 출력함.
mov		al, 'e'		  ;이후 이 과정을 반복하여 "Hello, Keonteck's World"를 화면에 출력함.
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




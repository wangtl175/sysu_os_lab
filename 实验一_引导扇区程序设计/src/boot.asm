Dn_Rt equ 1
Up_Rt equ 2
Up_Lt equ 3
Dn_Lt equ 4
delay equ 50000
ddelay equ 5000
;org 0x7c00 ;不是因为有org指令所以程序就保存在内存的0x7c00
mov ax,cs
mov ss,ax
mov ds,ax
mov ax,0xb800
mov es,ax;显存
mov si,mmsg+0x7c00
xor bx,bx
print:
	mov al,[si]
	mov ah,0x0f
	add si,1
	cmp al,0x00
	jz start
	mov [es:bx],ax
	inc bx
	inc bx
	jmp print

start:
	mov si,msg+0x7c00
loop1:
	jmp show
next1:
	mov cx,delay
	mov al,Dn_Rt
	cmp al,[si+3];direction
	jz D_R
	mov al,Dn_Lt
	cmp al,[si+3]
	jz D_L
	mov al,Up_Rt
	cmp al,[si+3]
	jz U_R
	;U_L就不用判断了
	mov al,[si]
	dec al
	cmp al,0
	jnz next9
	mov al,Dn_Lt
	mov [si+3],al
	mov al,2
next9:
	mov [si],al
	mov al,[si+1]
	dec al
	cmp al,-1
	jnz next10
	mov al,Up_Rt
	mov [si+3],al
	mov al,1
next10:
	mov [si+1],al


next2:
	mov ax,ddelay
next11:
	dec ax
	jnz next11
	dec cx
	jnz next2
	jmp loop1
	 
D_R:
	mov al,[si]
	inc al;x+1
	cmp al,25;判断是否到底
	jnz next3
	mov al,Up_Rt;到底就反弹成右上
	mov [si+3],al
	mov al,23
next3:
	mov [si],al
	mov al,[si+1]
	inc al;y+1
	cmp al,80;判断是否到达右边界
	jnz next4
	mov al,Dn_Lt;到了有边界就反弹成左上
	mov [si+3],al
	mov al,78
next4:
	mov [si+1],al
	jmp next2

D_L:
	mov al,[si]
	inc al
	cmp al,25
	jnz next5
	mov al,Up_Lt
	mov [si+3],al
	mov al,23
next5:
	mov [si],al
	mov al,[si+1]
	dec al
	cmp al,-1
	jnz next6
	mov al,Dn_Rt
	mov [si+3],al
	mov al,1
next6:
	mov [si+1],al
	jmp next2

U_R:
	mov al,[si]
	dec al
	cmp al,0
	jnz next7
	mov al,Dn_Rt
	mov [si+3],al
	mov al,2
next7:
	mov [si],al
	mov al,[si+1]
	inc al
	cmp al,80
	jnz next8
	mov al,Up_Lt
	mov [si+3],al
	mov al,78
next8:
	mov [si+1],al
	jmp next2

show:
	mov al,[si];x
	xor ah,ah
	mov bx,80;x*80
	mul bx
	mov bl,[si+1];y
	xor bh,bh
	add ax,bx
	shl ax,1;(x*80+y)*2
	mov bx,ax
	mov ah,[si+4];color
	and ah,0x0f
	or ah,0x08
	mov al,[si+2]
	mov [es:bx],ax
	inc ah
	mov [si+4],ah
	jmp next1

mmsg:
	db '18340168 wangtianlong'
	db 0x00
msg:
	db 1;position x si
	db 0;position y si+1
	db 'A';char si+2
	db Dn_Lt;direction si+3
	db 0;color si+4
	times 510-($-$$) db 0x00
	db 0x55,0xaa
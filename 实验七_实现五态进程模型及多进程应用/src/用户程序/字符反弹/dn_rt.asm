;在右下角反弹

Dn_Rt equ 1
Up_Rt equ 2
Up_Lt equ 3
Dn_Lt equ 4
delay equ 50000
ddelay equ 5000

displaytimes equ 100
org 0x100

push ax
push bx
push cx
push ds
push es
push si

mov ax,cs
mov ss,ax
mov ds,ax
mov ax,0xb800
mov es,ax;显存
xor bx,bx

start:
	mov si,msg
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
	cmp al,12
	jnz next9
	mov al,Dn_Lt
	mov [si+3],al
	mov al,14
next9:
	mov [si],al
	mov al,[si+1]
	dec al
	cmp al,39
	jnz next10
	mov al,Up_Rt
	mov [si+3],al
	mov al,41
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
	cmp al,39
	jnz next6
	mov al,Dn_Rt
	mov [si+3],al
	mov al,41
next6:
	mov [si+1],al
	jmp next2

U_R:
	mov al,[si]
	dec al
	cmp al,12
	jnz next7
	mov al,Dn_Rt
	mov [si+3],al
	mov al,14
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
	mov al,[si+5]
	cmp al,displaytimes
	jz return
	inc al
	mov [si+5],al


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

return:
	pop si
	pop es
	pop ds
	pop cx
	pop bx
	pop ax

	ret
msg:
	db 13;position x si
	db 40;position y si+1
	db 'A';char si+2
	db Dn_Lt;direction si+3
	db 0;color si+4
	db 0
	times 512-($-$$) db 0x00
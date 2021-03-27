.8086
public _loadblock
public _jump
public _LoadOuch
public _LoadWhell
_TEXT segment byte public 'CODE'
DGROUP group _TEXT,_DATA,_BSS
assume cs:_TEXT,ds:DGROUP,ss:DGROUP

whellDelay equ 5;风火轮延迟

_loadblock proc near;有4个参数，es:bx，一个要加载的块号从1开始编号loadblock，还有一个要加载多少块(es,bx,id,num)

	push bp
	mov bp,sp

	push ax
	push bx
	push cx
	push dx
	push es

	mov ax,[bp+4]
	mov es,ax
	mov bx,[bp+6]
	mov ah,2
	mov al,[bp+10]
	mov ch,0
	mov cl,[bp+8]
	mov dl,0
	mov dh,0
	int 13h

	pop es
	pop dx
	pop cx
	pop bx
	pop ax
	pop bp


	ret
_loadblock endp

_jump proc near;两个参数,要跳到的cs:ip
;在这里就把用户程序的ss=cs，sp=0000h给初始化好，并且把这里的ss,sp压到用户程序的栈里，然后再压cs,ip，用户用retf就可以返回到这里了
	push bp
	mov bp,sp
	
	push ax
	push bx
	push cx

	push es

	mov bx,[bp+4];cs
	mov ax,[bp+6];ip

	;把PSP写到cs:0处
	mov es,bx
	lea si,PSPBegin
	mov di,0
	;movsb 把ds:si->es:di
	lea cx,PSPEnd
	sub cx,si;循环次数
	rep movsb;循环写



	;在这里就把用户程序的堆栈给设好，把这里的sp压栈，再把这里的cs，ip压栈，然后把要跳转的cs，ip压栈
	mov cx,sp
	;切换到用户栈
	mov ss,bx
	mov sp,0

	push cx;压sp
	push cs;压这里的cs
	lea cx,retp
	push cx;压返回的ip
	
	xor cx,cx
	push cx;压0x0000

	push bx;要跳转的cs
	push ax;要跳转的ip
	retf;跳转


retp:
	;把内核栈切换回来
	pop bx;sp
	mov ax,cs
	mov ss,ax
	mov sp,bx;切换完成

	pop es

	pop cx
	pop bx
	pop ax

	pop bp

	ret

PSPBegin:
	retf
PSPEnd:nop


_jump endp


_LoadWhell proc near
	
	push ax
	push es


	CLI
	xor ax,ax
	mov es,ax
	lea ax,whell

	mov [es:20h],ax
	mov ax,cs
	mov [es:22h],ax
	mov es,ax
	STI

	pop es
	pop ax

	ret
whell:
	
	push ds
	push es
	push ax
	push bx

	mov ax,cs
	mov ds,ax

	lea bx,count
	mov al,[bx]
	dec al
	mov [bx],al
	cmp al,0
	jnz exit
	mov al,whellDelay
	mov [bx],al

	lea bx,show

	mov al,[bx]
	xor ah,ah
	inc ax
	cmp ax,4
	jnz next1
	mov ax,1
next1:
	mov [bx],al
	add bx,ax

	mov ax,0b800h
	mov es,ax

	mov al,[bx]
	mov ah,07h
	mov bx,3998
	mov es:[bx],ax

exit:
	
	mov al,20h
	out 20h,al
	out 0a0h,al

	pop bx
	pop ax
	pop es
	pop ds

	iret
_LoadWhell endp


_LoadOuch proc near
	push ax
	push bx
	push es

	;保存原来的int 9
	xor ax,ax
	mov es,ax
	mov ax,es:[24h]
	push ax
	mov ax,es:[26h]
	push ax


	lea bx,int9
	pop ax
	mov [bx+2],ax
	pop ax
	mov [bx],ax


	mov bx,cs
	lea ax,OUCH

	CLI
	mov es:[24h],ax
	mov es:[26h],bx
	STI

	pop es
	pop bx
	pop ax

	ret

OUCH:
	
	push ds
	push es
	push ax
	push bx
	push cx
	push si

	mov ax,cs
	mov ds,ax	
	
	mov ax,0b800h
	mov es,ax

	lea si,ouchMsgLength
	mov cx,[si]
	lea si,ouchMsg
	mov bx,3800
	mov ah,07h

again1:;打印字符
	mov al,[si]
	mov es:[bx],ax
	inc si
	inc bx
	inc bx
	loop again1

	mov cx,50000
delay2:
	push cx
	mov cx,3000
delay1:
	loop delay1
	pop cx
	loop delay2

	lea si,ouchMsgLength
	mov cx,[si]
	mov bx,3800
	mov ah,07h
	mov al,' '
again2:;清空字符
	mov es:[bx],ax
	inc bx
	inc bx
	loop again2


	;模仿int的工作方式，调用原来的int 9
	pushf

	pushf
	pop ax
	and ah,11111100b
	push ax
	popf
	
	mov ax,cs
	push ax
	lea ax,ouchRetp
	push ax
	
	lea bx,int9
	mov ax,[bx+2]
	push ax
	mov ax,[bx]
	push ax
	retf


ouchRetp:


;	mov al,20h
;	out 20h,al
;	out 0a0h,al

	pop si
	pop cx
	pop bx
	pop ax
	pop es
	pop ds

	iret
_LoadOuch endp

_TEXT ends

_DATA segment word public 'DATA'
count label byte
	db whellDelay
show label byte
	db 1;char + 1 to 3
	db '|'
	db '/'
	db '\'
ouchMsg label byte
	db "OUCH! OUCH!"
ouchMsgLength label byte
	dw $-ouchMsg
int9 label byte
	dw 0,0;原来int 9的ip，cs
_DATA ends

_BSS segment word public 'BSS'
_BSS ends
end
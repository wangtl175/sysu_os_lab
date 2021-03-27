.8086
public _printc
public _readkeypress
public _cleanscreen

_TEXT segment byte public 'CODE'
DGROUP group _TEXT,_DATA,_BSS
assume cs:_TEXT,ds:DGROUP,ss:DGROUP
_printc proc near;一个参数c,用int 10h把字符c输出,21h需要dos支持
	push bp
	mov bp,sp
	push ax
	push ds
	push cx
	push bx

	mov ax,cs
	mov ds,ax

	mov bh,0
	mov ah,3
	int 10h;读当前光标位置

	mov ax,1301h
	mov bx,0007h
	add bp,4
	mov cx,1
	int 10h

	pop bx
	pop cx
	pop ds
	pop ax
	pop bp
	ret
_printc endp

_readkeypress proc near;有一个返回值，从键盘读一个按键
	mov ah,0
	int 16h
	xor ah,ah
	ret
_readkeypress endp

_cleanscreen proc near
	push bx
	push cx
	push dx
	push ax
	
	mov ah,6
	mov al,0
	mov ch,0
	mov cl,0
	mov dh,24
	mov dl,79
	mov bh,7
	int 10h;清屏

	mov bh,0
	mov dx,0
	mov ah,02h
	int 10h;把光标设置到0，0

	pop ax
	pop dx
	pop cx
	pop bx
	ret
_cleanscreen endp

_TEXT ends

_DATA segment word public 'DATA'
_DATA ends

_BSS segment word public 'BSS'
_BSS ends
end
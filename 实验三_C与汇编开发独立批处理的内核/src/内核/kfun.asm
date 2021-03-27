.8086
public _loadblock
public _jump
_TEXT segment byte public 'CODE'
DGROUP group _TEXT,_DATA,_BSS
assume cs:_TEXT,ds:DGROUP,ss:DGROUP

_loadblock proc near;������������es:bx��һ��Ҫ���صĿ�Ŵ�1��ʼ���loadblock(es,bx,id)
;�����س�����900:100,Ҫ�Ӵ�0x0800�л���0x0000�����Ҫ���ص�0x0000:0x9100�Ļ�
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
	mov al,1
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

_jump proc near;��������,Ҫ������cs:ip
	push bp
	mov bp,sp
	
	mov ax,[bp+4];cs
	push ax
	mov ax,[bp+6];ip
	push ax
	retf

	pop ax;����cs��ip
	pop ax

	ret

_jump endp

_TEXT ends

_DATA segment word public 'DATA'
_DATA ends

_BSS segment word public 'BSS'
_BSS ends
end
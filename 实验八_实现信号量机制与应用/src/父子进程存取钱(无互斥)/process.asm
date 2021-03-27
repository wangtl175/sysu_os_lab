.8086

public _fork
public _exit
public _wait
public _GetSema
public _FreeSema
public _P
public _V

_TEXT segment byte public 'CODE'
DGROUP group _DATA,_BSS
assume cs:_TEXT,ds:DGROUP,ss:DGROUP

_fork proc near;һ���������ӽ��̵�����
	push bp
	mov bp,sp
	push bx
	mov bx,[bp+4]
	mov ah,3h
	int 21h
	pop bx
	pop bp
	ret;�ӽ���������ͷ��ز���ϵͳ�ˣ��п���ջû�п�����ȷ
_fork endp

_exit proc near
	mov ah,4h
	int 21h
	ret
_exit endp

_wait proc near
@1:
	mov ah,5h
	int 21h
	cmp ax,1
	jnz @1;������ת̫���ˣ���û���ü��л��ֽ���ѭ��
;	int 22h
	ret
_wait endp

_GetSema proc near
	push bp
	mov bp,sp
	push bx
	mov bx,[bp+4]
	mov ah,6h
	int 21h
	pop bx
	pop bp
	ret
_GetSema endp

_FreeSema proc near
	push bp
	mov bp,sp
	push bx
	mov bx,[bp+4]
	mov ah,7h
	int 21h
	pop bx
	pop bp
	ret
_FreeSema endp

_P proc near
	push bp
	mov bp,sp
	push bx
	mov bx,[bp+4]
	mov ah,8h
	int 21h
	pop bx
	pop bp
	ret
_P endp

_V proc near
	push bp
	mov bp,sp
	push bx
	mov bx,[bp+4]
	mov ah,9h
	int 21h
	pop bx
	pop bp
	ret
_V endp

_TEXT ends

_DATA segment word public 'DATA'
_DATA ends

_BSS segment word public 'BSS'
_BSS ends

end
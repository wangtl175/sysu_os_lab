.8086

public _putch
public _readkeypress


_TEXT segment byte public 'CODE'
DGROUP group _TEXT,_DATA,_BSS
assume cs:_TEXT,ds:DGROUP,ss:DGROUP


_putch proc near
	push bp
	mov bp,sp
	push ax
	push dx
	mov dl,[bp+4]
	mov ah,2
	int 21h
	pop dx
	pop ax
	pop bp
	ret
_putch endp

_readkeypress proc near
	mov ah,1
	int 21h
	cbw
	ret
_readkeypress endp

_TEXT ends

_DATA segment word public 'DATA'
_DATA ends

_BSS segment word public 'BSS'
_BSS ends
end
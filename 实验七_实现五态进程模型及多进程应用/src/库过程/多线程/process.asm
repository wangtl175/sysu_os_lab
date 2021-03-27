.8086

public _fork
public _exit
public _wait

_TEXT segment byte public 'CODE'
DGROUP group _DATA,_BSS
assume cs:_TEXT,ds:DGROUP,ss:DGROUP

_fork proc near
	mov ah,3h
	int 21h
	ret;�ӽ���������ͷ��ز���ϵͳ�ˣ��п���ջû�п�����ȷ
_fork endp

_exit proc near
	mov ah,4h
	int 21h
	ret
_exit endp

_wait proc near
	mov ah,5h
	int 21h
	ret
_wait endp

_TEXT ends

_DATA segment word public 'DATA'
_DATA ends

_BSS segment word public 'BSS'
_BSS ends

end
;tlinkʱ��.obj�ļ���˳��Ҫ��
extrn _counter:near

.8086
_TEXT segment byte public 'CODE'
DGROUP group _TEXT,_DATA,_BSS
assume cs:_TEXT,ds:DGROUP,ss:DGROUP
org 100h

_start:
	push ax
	push es
	push ds
	
	mov ax,cs
	mov es,ax
	mov ds,ax
	;mov dx,offset string;offset��֪��Ϊʲô��offset������ȷ�õ�string��ƫ��
	call near ptr _counter;ͨ���鿴.asm�ļ�������֪������ֵ��ax����
	
	pop ds
	pop es
	pop ax

	ret

_TEXT ends



_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends


end _start
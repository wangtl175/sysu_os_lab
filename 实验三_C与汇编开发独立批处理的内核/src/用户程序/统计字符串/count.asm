;tlink时，.obj文件有顺序要求
extrn _counter:near

.8086
_TEXT segment byte public 'CODE'
DGROUP group _TEXT,_DATA,_BSS
assume cs:_TEXT,ds:DGROUP,ss:DGROUP
org 100h

_start:
	mov ax,cs
	mov es,ax
	mov ds,ax
	mov ss,ax
	mov sp,100h
	;mov dx,offset string;offset不知道为什么，offset不能正确得到string的偏移
	call near ptr _counter;通过查看.asm文件，可以知道返回值在ax里面
	mov ax,800h
	push ax
	mov ax,100h
	push ax

	retf

_TEXT ends



_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends


end _start
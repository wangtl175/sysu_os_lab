extrn _kernelmain:near
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
	mov sp,0ffffh
	call near ptr _kernelmain
	jmp _start
_TEXT ends

_DATA segment word public 'DATA'
_DATA ends

_BSS segment word public 'BSS'
_BSS ends

end _start
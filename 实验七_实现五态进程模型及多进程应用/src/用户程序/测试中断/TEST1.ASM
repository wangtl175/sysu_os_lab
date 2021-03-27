.8086
_TEXT segment byte public 'CODE'
DGROUP group _TEXT,_DATA,_BSS
assume cs:_TEXT,ds:DGROUP,ss:DGROUP

org 100h

_start:
	
	int 22h
	ret
_TEXT ends

_DATA segment word public 'DATA'
_DATA ends

_BSS segment word public 'BSS'
_BSS ends

end _start

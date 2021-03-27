.8086
public _loadblock
public _jump
public _LoadOuch
public _LoadWhell
public _save
public _restart
public _LoadINT20H
public _LoadINT21H
public _LoadINT22H
public _printc
public _readkeypress
public _cleanscreen

extrn _CurrentState:near

_TEXT segment byte public 'CODE'
DGROUP group _TEXT,_DATA,_BSS
assume cs:_TEXT,ds:DGROUP,ss:DGROUP

_LoadINT20H proc near
	push ax
	push es
	CLI
	xor ax,ax
	mov es,ax
	lea ax,INT20H

	mov [es:80h],ax
	mov ax,cs
	mov [es:82h],ax
	mov es,ax
	STI

	pop es
	pop ax

	ret

INT20H:
	mov ax,cs
	mov ds,ax
	mov ss,ax
	lea si,kernelsp
	mov sp,[si]
	jmp retp
_LoadINT20H endp

_LoadINT22H proc near
	push ax
	push es
	CLI
	xor ax,ax
	mov es,ax
	lea ax,INT22H

	mov [es:88h],ax
	mov ax,cs
	mov [es:8ah],ax
	mov es,ax
	STI

	pop es
	pop ax

	ret
INT22H:
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

	lea si,int22hstrlen
	mov cx,[si]

	lea si,int22hstr

	mov bx,3800
	mov ah,07h

again3:;��ӡ�ַ�
	mov al,[si]
	mov es:[bx],ax
	inc si
	inc bx
	inc bx
	loop again3
	pop si
	pop cx
	pop bx
	pop ax
	pop es
	pop ds

	iret

_LoadINT22H endp

_printc proc near;һ������c,��int 10h���ַ�c���,21h��Ҫdos֧��
	push bp
	mov bp,sp
	push ax
	push cx
	push bx

	mov ax,cs
	mov ds,ax

	mov bh,0
	mov ah,3
	int 10h;����ǰ���λ��

	mov ax,1301h
	mov bx,0007h
	add bp,4
	mov cx,1
	int 10h

	pop bx
	pop cx
	pop ax
	pop bp
	ret
_printc endp

_readkeypress proc near;��һ������ֵ���Ӽ��̶�һ������
	mov ah,0
	int 16h
	cbw
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
	int 10h;����

	mov bh,0
	mov dx,0
	mov ah,02h
	int 10h;�ѹ�����õ�0��0

	pop ax
	pop dx
	pop cx
	pop bx
	ret
_cleanscreen endp

_loadblock proc near;��4��������es:bx��һ��Ҫ���صĿ�Ŵ�1��ʼ���loadblock������һ��Ҫ���ض��ٿ�(es,bx,id,num)

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

_jump proc near;��������,Ҫ������cs:ip
;������Ͱ��û������ss=cs��sp=0000h����ʼ���ã����Ұ������ss,spѹ���û������ջ�Ȼ����ѹcs,ip���û���retf�Ϳ��Է��ص�������
	push bp
	mov bp,sp
	
	push ax
	push bx
	push cx
	push dx
	push di
	push es
	push ds
	push si
	pushf

	mov bx,[bp+4];cs
	mov ax,[bp+6];ip

	;��PSPд��cs:0��
	mov es,bx
	lea si,PSPBegin
	mov di,0
	;movsb ��ds:si->es:di
	lea cx,PSPEnd
	sub cx,si;ѭ������
	rep movsb;ѭ��д
	
	lea si,kernelsp
	mov [si],sp

	;�����ٸĽ�һ�£��������sp����kernelsp��
	;����ǰ׺��ϵͳ���÷��ص��ںˣ�Ȼ����תһ�¾͵���
	;�û�ջֻ����ǰѹһ��0x0000��ȥ����
	
	;�л����û�ջ
	mov ss,bx
	mov sp,0
	
	xor cx,cx
	push cx;ѹ0x0000

	push bx;Ҫ��ת��cs
	push ax;Ҫ��ת��ip
	retf;��ת
retp:
	;�ָ��Ĵ���
	popf
	pop si
	pop ds
	pop es
	pop di
	pop dx
	pop cx
	pop bx
	pop ax
	pop bp
	ret

PSPBegin:
	int 20h
PSPEnd:nop

_jump endp


whellDelay equ 5;������ӳ�

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

	;����ԭ����int 9
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

again1:;��ӡ�ַ�
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
again2:;����ַ�
	mov es:[bx],ax
	inc bx
	inc bx
	loop again2


	;ģ��int�Ĺ�����ʽ������ԭ����int 9
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


	pop si
	pop cx
	pop bx
	pop ax
	pop es
	pop ds

	iret
_LoadOuch endp

;��ʱջ����call _save��ip,int��ip,cs,flags
_save proc near
	push ds
	push cs
	pop ds;ds=cs
	push si
	
	
	lea si,_CurrentState
	pop word ptr [si+16];����si
	pop word ptr [si+14];����ds
	
	lea si,ret_temp
	pop word ptr [si];����_save�ķ��ص�
	
	lea si,_CurrentState
	pop word ptr [si+22];����ip
	pop word ptr [si+24];����cs
	pop word ptr [si+26];����flags
	
	mov [si+18],ss
	mov [si+20],sp
	
	mov si,ds
	mov ss,si
	
	lea si,_CurrentState
	mov sp,si
	add sp,14
	
	push es
	push bp
	push di
	push dx
	push cx
	push bx
	push ax
	
	lea si,kernelsp
	mov sp,[si]
	;����ss,sp,ds,cs,ip���Ѿ����ں˵���
	
	lea si,ret_temp
	mov ax,[si]
	jmp ax
_save endp

_restart proc near
	lea si,kernelsp
	mov [si],sp
	lea sp,_CurrentState
	pop ax
	pop bx
	pop cx
	pop dx
	pop di
	pop bp
	pop es;��ʱspָ��ds
	
	lea si,ds_temp
	pop word ptr [si]
	lea si,si_temp
	pop word ptr [si]
	
	lea si,bx_temp
	mov [si],bx;����bx��������ss��ֵ
	;�ָ�ջ
	pop bx;��ʱspָ��sp
	mov ss,bx
	mov bx,sp
	mov sp,[bx]
	
	add bx,2;bxָ��ip
	;flags,cs,ipѹջ
	push word ptr [bx+4]
	push word ptr [bx+2]
	push word ptr [bx]
	
	;�ָ�ds,si
	push ax
	push word ptr [si];��bxѹջ����
	lea si,ds_temp
	mov ax,[si]
	lea si,si_temp
	mov bx,[si]
	mov ds,ax
	mov si,bx
	
	pop bx
	pop ax
	
	iret
	
_restart endp

_LoadINT21H proc near
	push ax
	push es
	CLI
	xor ax,ax
	mov es,ax
	lea ax,INT21H

	mov [es:84h],ax
	mov ax,cs
	mov [es:86h],ax
	mov es,ax
	STI

	pop es
	pop ax

	ret
	
INT21H:
	call _save
	
	lea si,_CurrentState
	mov ax,[si];��ȡax

	cmp ah,1h
	jz SYCALL_1H

	cmp ah,2h
	jz SYCALL_2H

	cmp ah,4ch;���ز���ϵͳ
	jz SYCALL_4CH

SYCALLEND:
	jmp _restart;ע����jmp������call,call���ipѹջ
_LoadINT21H endp

SYCALL_4CH proc near
	mov ax,cs
	mov ss,ax
	lea si,kernelsp
	mov sp,[si]
	jmp retp
SYCALL_4CH endp

SYCALL_1H proc near;�Ӽ�������һ���ַ���alΪ������ַ�
	mov ax,cs
	mov ds,ax
	mov ah,0
	int 16h
	xor ah,ah
	lea si,_CurrentState
	mov [si],ax;�޸�ax
	jmp SYCALLEND
SYCALL_1H endp


SYCALL_2H proc near;��ʾһ���ַ���DLΪҪ������ַ�
	mov ax,cs
	mov es,ax

	mov bh,0
	mov ah,3
	int 10h
	lea bp,_CurrentState
	add bp,6
	mov ax,1301h
	mov bx,0007h
	mov cx,1
	int 10h
	jmp SYCALLEND
SYCALL_2H endp



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
	dw 0,0;ԭ��int 9��ip��cs
	
ds_temp label byte
	dw ?
ret_temp label byte
	dw ?
si_temp label byte
	dw ?
kernelsp label byte
	dw ?
bx_temp label byte
	dw ?
int22hstr label byte
	db "INT22H"
int22hstrlen label byte
	db $-int22hstr
_DATA ends

_BSS segment word public 'BSS'
_BSS ends
end
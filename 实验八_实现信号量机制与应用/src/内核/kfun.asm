.8086
public _loadblock
;public _LoadOuch
public _LoadINT8H
public _LoadINT20H
public _LoadINT21H
public _LoadINT22H
public _printc
public _readkeypress
public _cleanscreen
public _initalStack
public _LoadPSP
public _setIF
public _clrIF

extrn _currentPCB:near
extrn _kernelPCB:near
extrn _switchProcess:near
extrn _endProcess:near
extrn _do_fork:near
extrn _do_exit:near
extrn _do_wait:near
extrn _do_GetSema:near
extrn _do_FreeSema:near
extrn _do_P:near
extrn _do_V:near

_TEXT segment byte public 'CODE'
DGROUP group _DATA,_BSS
assume cs:_TEXT,ds:DGROUP,ss:DGROUP

_clrIF proc near
	cli
_clrIF endp

_setIF proc near
	sti
_setIF endp

_LoadPSP proc near
	push bp
	mov bp,sp;[bp+4]ָ�����
	
	push cx
	push es
	push si
	push di

	mov ax,[bp+4]
	mov es,ax
	xor di,di


	mov ax,cs
	mov ds,ax
	lea si,PSPBegin
	;movsb ��ds:si->es:di
	lea cx,PSPEnd
	sub cx,si;ѭ������
	rep movsb;ѭ��д

	pop di
	pop si
	pop es
	pop cx
	pop bp

	ret 
PSPBegin:
	int 20h
PSPEnd:nop

_LoadPSP endp

_initalStack proc near
	push bp
	mov bp,sp
	push bx
	
	mov bx,1000h;�����û������ջ�ζ�������
	mov ss,bx

	mov ax,[bp+4]
	mov sp,ax
	xor ax,ax
	push ax;�û�ջ��ѹһ��0000
	mov ax,sp;����ֵ
	mov bx,cs
	mov ss,bx
	sub bp,2;ss:bp==bx
	mov sp,bp
	pop bx
	pop bp
	ret
_initalStack endp

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
	STI

	pop es
	pop ax

	ret


;����Ӧ�ò����޸ģ���Ϊ���̷߳��ز������
INT20H:
	mov ax,cs
	mov ds,ax
	mov ss,ax
	mov es,ax
	mov si,word ptr DGROUP:_kernelPCB
	mov sp,[si+20]
	jmp _endProcess
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
	mov es,ax

;	lea si,int22hstrlen
;	mov cx,[si]
	mov cx,word ptr DGROUP:int22hstrlen

;	lea si,int22hstr
	mov si,offset DGROUP:int22hstr
	
	mov ax,0b800h
	mov es,ax
	mov bx,3800
	mov ah,07h
again3:;��ӡ�ַ�
	mov al,[si]
;	mov al,'a'
	mov es:[bx],ax
	inc si
	inc bx
	inc bx
	loop again3

;	mov ah,0
;	int 16h

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
;���������id��Ϊ����+����
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

whellDelay equ 5

_LoadINT8H proc near
	
	push ax
	push es


	CLI
	xor ax,ax
	mov es,ax
	lea ax,INT8H

	mov [es:20h],ax
	mov ax,cs
	mov [es:22h],ax
	mov es,ax
	STI

	pop es
	pop ax

	ret
INT8H:
	call _save	

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
	push es
	mov ax,0b800h
	mov es,ax

	mov al,[bx]
	mov ah,07h
	mov bx,3998
	mov es:[bx],ax
	pop es
exit:

	call _switchProcess


	mov al,20h
	out 20h,al
	out 0a0h,al

	jmp _restart
_LoadINT8H endp




;��ʱջ����call _save��ip,int��ip,cs,flags
_save proc near
	push ds
	push cs
	pop ds;ds=cs
	push si

	;lea si,_currentPCB;�����currentPCB�ĵ�ַ
	mov si,word ptr DGROUP:_currentPCB;�����_currentPCB�����ݣ�����ǰPCB�ĵ�ַ
	pop word ptr [si+16];����si
	pop word ptr [si+14];����ds
	
	lea si,ret_temp
	pop word ptr [si];����_save�ķ��ص�
	
	;lea si,_currentPCB
	mov si,word ptr DGROUP:_currentPCB
	pop word ptr [si+22];����ip
	pop word ptr [si+24];����cs
	pop word ptr [si+26];����flags
	
	mov [si+18],ss
	mov [si+20],sp
	
	mov si,ds
	mov ss,si
	
	;lea si,_currentPCB
	;mov sp,si
	;add sp,14
	mov sp,word ptr DGROUP:_currentPCB
	add sp,14
	
	push es
	push bp
	push di
	push dx
	push cx
	push bx
	push ax
	
	mov si,word ptr DGROUP:_kernelPCB
	mov sp,[si+20]
	mov ax,cs
	mov es,ax
	;����ss,sp,ds,es,cs���Ѿ����ں˵���
	
	lea si,ret_temp
	mov ax,[si]
	jmp ax
_save endp

_restart proc near
	mov si,word ptr DGROUP:_kernelPCB
	mov [si+20],sp
	;lea sp,_currentPCB
	mov sp,word ptr DGROUP:_currentPCB
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

SYCALL_1H proc near;�Ӽ�������һ���ַ���alΪ������ַ�

	mov ah,0
	int 16h;����Ὺ�жϣ����ܾͻᱻʱ���л����ˣ�ʱ���жϻ���save�ѷ��ص��޸ĵ�
	cbw
;	mov si,word ptr DGROUP:_currentPCB
;	mov [si],ax
	;jmp SYCALLEND
	iret
SYCALL_1H endp


SYCALL_2H proc near;��ʾһ���ַ���DLΪҪ������ַ�
	call _save	

	mov bh,0
	mov ah,3
	int 10h

;	mov bp,sp
	mov bp,word ptr DGROUP:_currentPCB
	add bp,6
	
	mov ax,1301h
	mov bx,0007h
	mov cx,1
	int 10h

	jmp _restart
SYCALL_2H endp

SYCALL_3H proc near
	call _save

	call _do_fork
	
	jmp _restart
SYCALL_3H endp

SYCALL_4H proc near

	call _save
	call _do_exit;������switchProcess
	jmp _restart

;	mov ax,cs
;	mov ss,ax
;	mov ds,ax
;	mov es,ax
;	mov si,word ptr DGROUP:_kernelPCB
;	mov sp,[si+20]

;	jmp _do_exit

SYCALL_4H endp

SYCALL_5H proc near
	call _save
	call _do_wait;�Ѹ�������Ϊ���������ҵ�����switchProcess
	jmp _restart
SYCALL_5H endp

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
	STI

	pop es
	pop ax

	ret
	
INT21H:
	;call _save

	;mov si,word ptr DGROUP:_currentPCB
	;mov ax,[si];��ȡax

	cmp ah,1h
	jz SYCALL_1H

	cmp ah,2h
	jz SYCALL_2H

	cmp ah,3h
	jz SYCALL_3H

	cmp ah,4h
	jz SYCALL_4H

	cmp ah,5h
	jz SYCALL_5H

	cmp ah,6h
	jz SYCALL_6H;do_GetSema����һ����������bx��

	cmp ah,7h
	jz SYCALL_7H;do_FreeSema����һ����������bx��

	cmp ah,8h
	jz SYCALL_8H;do_P����һ����������bx��

	cmp ah,9h
	jz SYCALL_9H;do_V����һ����������bx��

	cmp ah,4ch;���ز���ϵͳ
	jz SYCALL_4CH


;SYCALLEND:
;	jmp _restart;ע����jmp������call

	
;	iret
_LoadINT21H endp

SYCALL_4CH proc near
	mov ax,cs
	mov ss,ax
	mov ds,ax
	mov es,ax
	mov si,word ptr DGROUP:_kernelPCB
	mov sp,[si+20]
;	jmp retp
	jmp _endProcess;Ҫ��jmp���������ں�ջ������������ip
SYCALL_4CH endp

SYCALL_6H proc near
	call _save
	call _do_GetSema
	jmp _restart
SYCALL_6H endp

SYCALL_7H proc near
	call _save
	call _do_FreeSema
	jmp _restart
SYCALL_7H endp

SYCALL_8H proc near
	call _save
	call _do_P
	jmp _restart
SYCALL_8H endp

SYCALL_9H proc near
	call _save
	call _do_V
	jmp _restart
SYCALL_9H endp

_TEXT ends

_DATA segment word public 'DATA'
count label byte
	db whellDelay
show label byte
	db 1;char + 1 to 3
	db '|'
	db '/'
	db '\'
	
ds_temp label byte
	dw ?
ret_temp label byte
	dw ?
si_temp label byte
	dw ?
bx_temp label byte
	dw ?
int22hstr label byte
	db "INT22H"
int22hstrlen label byte
	dw $-int22hstr
_DATA ends

_BSS segment word public 'BSS'
_BSS ends
end
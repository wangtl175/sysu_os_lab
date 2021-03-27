.8086
public _loadblock
public _jump
public _LoadOuch
public _LoadWhell
_TEXT segment byte public 'CODE'
DGROUP group _TEXT,_DATA,_BSS
assume cs:_TEXT,ds:DGROUP,ss:DGROUP

whellDelay equ 5;������ӳ�

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

	push es

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



	;������Ͱ��û�����Ķ�ջ����ã��������spѹջ���ٰ������cs��ipѹջ��Ȼ���Ҫ��ת��cs��ipѹջ
	mov cx,sp
	;�л����û�ջ
	mov ss,bx
	mov sp,0

	push cx;ѹsp
	push cs;ѹ�����cs
	lea cx,retp
	push cx;ѹ���ص�ip
	
	xor cx,cx
	push cx;ѹ0x0000

	push bx;Ҫ��ת��cs
	push ax;Ҫ��ת��ip
	retf;��ת


retp:
	;���ں�ջ�л�����
	pop bx;sp
	mov ax,cs
	mov ss,ax
	mov sp,bx;�л����

	pop es

	pop cx
	pop bx
	pop ax

	pop bp

	ret

PSPBegin:
	retf
PSPEnd:nop


_jump endp


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


;	mov al,20h
;	out 20h,al
;	out 0a0h,al

	pop si
	pop cx
	pop bx
	pop ax
	pop es
	pop ds

	iret
_LoadOuch endp

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
_DATA ends

_BSS segment word public 'BSS'
_BSS ends
end
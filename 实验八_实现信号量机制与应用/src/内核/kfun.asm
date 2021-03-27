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
	mov bp,sp;[bp+4]指向参数
	
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
	;movsb 把ds:si->es:di
	lea cx,PSPEnd
	sub cx,si;循环次数
	rep movsb;循环写

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
	
	mov bx,1000h;所有用户程序的栈段都在这里
	mov ss,bx

	mov ax,[bp+4]
	mov sp,ax
	xor ax,ax
	push ax;用户栈里压一个0000
	mov ax,sp;返回值
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


;这里应该不用修改，因为子线程返回不用这个
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
again3:;打印字符
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

_printc proc near;一个参数c,用int 10h把字符c输出,21h需要dos支持
	push bp
	mov bp,sp
	push ax
	push cx
	push bx

	mov bh,0
	mov ah,3
	int 10h;读当前光标位置

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

_readkeypress proc near;有一个返回值，从键盘读一个按键
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
	int 10h;清屏

	mov bh,0
	mov dx,0
	mov ah,02h
	int 10h;把光标设置到0，0

	pop ax
	pop dx
	pop cx
	pop bx
	ret
_cleanscreen endp

_loadblock proc near;有4个参数，es:bx，一个要加载的块号从1开始编号loadblock，还有一个要加载多少块(es,bx,id,num)
;五个参数，id改为柱面+扇区
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




;此时栈顶有call _save的ip,int的ip,cs,flags
_save proc near
	push ds
	push cs
	pop ds;ds=cs
	push si

	;lea si,_currentPCB;这个是currentPCB的地址
	mov si,word ptr DGROUP:_currentPCB;这个是_currentPCB的内容，即当前PCB的地址
	pop word ptr [si+16];保存si
	pop word ptr [si+14];保存ds
	
	lea si,ret_temp
	pop word ptr [si];保存_save的返回点
	
	;lea si,_currentPCB
	mov si,word ptr DGROUP:_currentPCB
	pop word ptr [si+22];保存ip
	pop word ptr [si+24];保存cs
	pop word ptr [si+26];保存flags
	
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
	;至此ss,sp,ds,es,cs都已经是内核的了
	
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
	pop es;此时sp指向ds
	
	lea si,ds_temp
	pop word ptr [si]
	lea si,si_temp
	pop word ptr [si]
	
	lea si,bx_temp
	mov [si],bx;保护bx，用它给ss赋值
	;恢复栈
	pop bx;此时sp指向sp
	mov ss,bx
	mov bx,sp
	mov sp,[bx]
	
	add bx,2;bx指向ip
	;flags,cs,ip压栈
	push word ptr [bx+4]
	push word ptr [bx+2]
	push word ptr [bx]
	
	;恢复ds,si
	push ax
	push word ptr [si];把bx压栈保存
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

SYCALL_1H proc near;从键盘输入一个字符，al为输入的字符

	mov ah,0
	int 16h;这里会开中断，可能就会被时钟切换走了，时钟中断会用save把返回点修改掉
	cbw
;	mov si,word ptr DGROUP:_currentPCB
;	mov [si],ax
	;jmp SYCALLEND
	iret
SYCALL_1H endp


SYCALL_2H proc near;显示一个字符，DL为要输出的字符
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
	call _do_exit;调用了switchProcess
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
	call _do_wait;把父进程设为阻塞，并且调用了switchProcess
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
	;mov ax,[si];获取ax

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
	jz SYCALL_6H;do_GetSema，有一个参数，在bx里

	cmp ah,7h
	jz SYCALL_7H;do_FreeSema，有一个参数，在bx里

	cmp ah,8h
	jz SYCALL_8H;do_P，有一个参数，在bx里

	cmp ah,9h
	jz SYCALL_9H;do_V，有一个参数，在bx里

	cmp ah,4ch;返回操作系统
	jz SYCALL_4CH


;SYCALLEND:
;	jmp _restart;注意是jmp而不是call

	
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
	jmp _endProcess;要用jmp，否则在内核栈里会留下这里的ip
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
org 7c00h

OffSetOfUserPrg equ 8100h

mov ax,cs
mov ds,ax
mov ss,ax

Start:
	mov	bp, Message		 ; BP=当前串的偏移地址
	mov	ax, ds		 ; ES:BP = 串地址
	mov	es, ax		 ; 置ES=DS
	mov	cx, MessageLength  ; CX = 串长（=9）
	mov	ax, 1301h		 ; AH = 13h（功能号）、AL = 01h（光标置于串尾）
	mov	bx, 0007h		 ; 页号为0(BH = 0) 黑底白字(BL = 07h)
    mov dh, 0		       ; 行号=0
	mov	dl, 0			 ; 列号=0
	int	10h			 ; BIOS的10h功能：显示一行字符


switch:
    mov ah,0x00
    int 0x16

    cmp al,'1'
    jz LoadUp_Lt
    cmp al,'2'
    jz LoadUp_Rt
    cmp al,'3'
    jz LoadDn_Lt
    cmp al,'4'
    jz LoadDn_Rt
    jmp switch


LoadUp_Lt:
     ;读软盘或硬盘上的若干物理扇区到内存的ES:BX处：
    mov cl,3                 ;起始扇区号 ; 起始编号为1
    jmp BeginLoad   
LoadUp_Rt:
    mov cl,4
    jmp BeginLoad
LoadDn_Lt:
    mov cl,5
    jmp BeginLoad
LoadDn_Rt:
    mov cl,6
    jmp BeginLoad

    mov ah,0x00
    mov al,0x03
    int 0x10

BeginLoad:
    mov ax,cs                ;段地址 ; 存放数据的内存基地址
    mov es,ax                ;设置段地址（不能直接mov es,段地址）
    mov bx, OffSetOfUserPrg  ;偏移地址; 存放数据的内存偏移地址
    mov ah,2                 ; 功能号
    mov al,1                 ;扇区数
    mov dl,0                 ;驱动器号 ; 软盘为0，硬盘和U盘为80H
    mov dh,0                 ;磁头号 ; 起始编号为0
    mov ch,0                 ;柱面号 ; 起始编号为0
    int 13H ;                调用读磁盘BIOS的13h功能
    jmp 0x800:0x100         ;同时修改了cs和ip的值
AfterRun:
    jmp $                      ;无限循环
Message:
    db 'Select user program(1-4):'
    MessageLength  equ ($-Message)
    times 510-($-$$) db 0
    db 0x55,0xaa
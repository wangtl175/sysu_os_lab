org 7c00h

OffSetOfUserPrg equ 8100h

mov ax,cs
mov ds,ax
mov ss,ax

Start:
	mov	bp, Message		 ; BP=��ǰ����ƫ�Ƶ�ַ
	mov	ax, ds		 ; ES:BP = ����ַ
	mov	es, ax		 ; ��ES=DS
	mov	cx, MessageLength  ; CX = ������=9��
	mov	ax, 1301h		 ; AH = 13h�����ܺţ���AL = 01h��������ڴ�β��
	mov	bx, 0007h		 ; ҳ��Ϊ0(BH = 0) �ڵװ���(BL = 07h)
    mov dh, 0		       ; �к�=0
	mov	dl, 0			 ; �к�=0
	int	10h			 ; BIOS��10h���ܣ���ʾһ���ַ�


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
     ;�����̻�Ӳ���ϵ����������������ڴ��ES:BX����
    mov cl,3                 ;��ʼ������ ; ��ʼ���Ϊ1
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
    mov ax,cs                ;�ε�ַ ; ������ݵ��ڴ����ַ
    mov es,ax                ;���öε�ַ������ֱ��mov es,�ε�ַ��
    mov bx, OffSetOfUserPrg  ;ƫ�Ƶ�ַ; ������ݵ��ڴ�ƫ�Ƶ�ַ
    mov ah,2                 ; ���ܺ�
    mov al,1                 ;������
    mov dl,0                 ;�������� ; ����Ϊ0��Ӳ�̺�U��Ϊ80H
    mov dh,0                 ;��ͷ�� ; ��ʼ���Ϊ0
    mov ch,0                 ;����� ; ��ʼ���Ϊ0
    int 13H ;                ���ö�����BIOS��13h����
    jmp 0x800:0x100         ;ͬʱ�޸���cs��ip��ֵ
AfterRun:
    jmp $                      ;����ѭ��
Message:
    db 'Select user program(1-4):'
    MessageLength  equ ($-Message)
    times 510-($-$$) db 0
    db 0x55,0xaa
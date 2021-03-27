mov ax,cs
mov ss,ax
mov ds,ax
mov ax,0x1000
mov es,ax
mov ah,2
mov al,5
mov ch,0
mov cl,2;扇区编号从1开始
mov dh,0
mov dl,0
mov bx,100h
int 13h


jmp 0x1000:0x100

times 510-($-$$) db 0
db 0x55,0xaa
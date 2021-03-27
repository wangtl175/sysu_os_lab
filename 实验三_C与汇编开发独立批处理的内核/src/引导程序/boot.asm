mov ax,cs
mov ss,ax
mov ds,ax
mov es,ax

mov ah,2
mov al,2
mov ch,0
mov cl,2
mov dh,0
mov dl,0
mov bx,8100h
int 13h


jmp 0x800:0x100

times 510-($-$$) db 0
db 0x55,0xaa
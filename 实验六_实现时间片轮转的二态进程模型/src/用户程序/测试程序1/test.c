extern void printf(char *s, ...);
extern void scanf(char *s, ...);
extern void puts(char *s);
extern void putch(char c);
extern void getch(char *c);
extern void gets(char *str);


int main()
{
	char ch, str[80];
	int a;
	printf("ch=");
	getch(&ch);
	printf("str=");
	gets(str);
	printf("a=");
	scanf("%d", &a);
	putch(ch);
	printf("\r\n");
	puts(str);
	printf("\r\n");
	printf("ch=%c,a=%d,str=%s\r\n", ch, a, str);
}
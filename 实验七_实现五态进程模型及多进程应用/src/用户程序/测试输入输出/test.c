extern void printf(char *s, ...);
extern void scanf(char *s, ...);
extern void puts(char *s);
extern void putch(char c);
extern void getch(char *c);
extern void gets(char *str);


char str[80];
char ch;
int a;
int main()
{
	/*char str[80] = "abcd";这样是赋不了值的，把这条语句放到函数体外就可以了*/
	/*char str[80];str在栈段里，虽然这样也可以，但是读到的数据写到了数据段中和，
	gets(str);闯进去的是str在栈段偏移量，而写的时候，用的是[bx]，所以写到了数据段中*/
	/*printf("ch=");
	getch(&ch);这也是写到了数据段中
	printf("str=");
	gets(str);
	
	printf("ch=%c,a=%d,str=%s\r\n", ch, a, str);*/

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
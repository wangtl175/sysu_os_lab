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
	/*char str[80] = "abcd";�����Ǹ�����ֵ�ģ����������ŵ���������Ϳ�����*/
	/*char str[80];str��ջ�����Ȼ����Ҳ���ԣ����Ƕ���������д�������ݶ��кͣ�
	gets(str);����ȥ����str��ջ��ƫ��������д��ʱ���õ���[bx]������д�������ݶ���*/
	/*printf("ch=");
	getch(&ch);��Ҳ��д�������ݶ���
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
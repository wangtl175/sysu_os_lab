extern void printf(char *s, ...);
extern void scanf(char *s, ...);

char input[20];
int i;
char c;
int num = 0;

void counter()
{
	printf("Input a string: ");
	scanf("%s", input);
	printf("Input a character: ");
	scanf("%c", &c);
	for (i = 0; input[i] != 0; i++)
	{
		if (input[i] == c)
		{
			num += 1;
		}
	}
	printf("The number of %c is: %d\r\n", c, num);
	return;
}
extern void scanf(char *s);
extern void printf(char* s);
char snum[10];
char *i2s(int n)
{
	int i = 0, t = 10, s = 0;
	if (n == 0)
	{
		snum[0] = '0';
		snum[1] = 0;
		return snum;
	}
	while (n != 0)
	{
		snum[i] = n % t + '0';
		n /= 10;
		i++;
		s++;
	}
	for (i = 0; i < s / 2; i++)
	{
		char temp = snum[i];
		snum[i] = snum[s - 1 - i];
		snum[s - 1 - i] = temp;
	}
	snum[s] = 0;
	return snum;
}


void counter()
{
	char input[20];
	int i;
	char c;
	int num = 0;
	printf("Input a string: ");
	scanf(input);
	printf("Input a character: ");
	scanf(&c);
	for (i = 0; input[i] != 0; i++)
	{
		if (input[i] == c)
		{
			num += 1;
		}
	}
	printf("The number is: ");
	printf(i2s(num));
	printf("\n\r");
	return;
}
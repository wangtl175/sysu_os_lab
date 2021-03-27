extern char readkeypress(void);
extern void printc(char c);

void printf(char *s)
{
	int i;
	for (i = 0; s[i] != 0; i++)
	{
		printc(s[i]);
	}
}

void scanf(char *s)
{
	int i = 0;
	char c;
	while (1)
	{
		c = readkeypress();
		if (c == 0x0d)
		{
			s[i] = 0;
			printc(0x0a);
			printc(0x0d);
			break;
		}
		s[i] = c;
		i++;
		printc(c);
	}
	return;
}
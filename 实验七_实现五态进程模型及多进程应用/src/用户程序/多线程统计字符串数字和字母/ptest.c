extern void scanf(char *s, ...);
extern void printf(char *s, ...);
extern int fork();
extern void wait();
extern void exit();

char str[80];
int LetterNum = 0;
int DigitalNum = 0;

void countLetter()
{
	int i = 0;
	for (; str[i] != 0; i++)
	{
		if ((str[i] <= 'z' && str[i] >= 'a') || (str[i] <= 'Z' && str[i] >= 'A'))
		{
			LetterNum++;
		}
	}
}

void countDigital()
{
	int i = 0;
	for (; str[i] != 0; i++)
	{
		if (str[i] <= '9' && str[i] >= '0')
		{
			DigitalNum++;
		}
	}
}

int main()
{
	int pid;
	printf("Input a string: ");
	scanf("%s", str);
	pid = fork();
	if (pid == -1)
	{
		printf("error in fork!\r\n");
		exit();
	}
	if (pid)/*父进程*/
	{
		countLetter();
		wait();/*等待子进程*/
		printf("LetterNum=%d DigitalNum=%d\r\n", LetterNum, DigitalNum);
	}
	else
	{
		countDigital();
		exit();
	}
}
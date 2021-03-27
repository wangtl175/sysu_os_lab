/*父线程f创建二个子线程s和d
大儿子线程s反复向父线程f祝福
小儿子进程d反复向父进程送水果(每次一个苹果或其他水果)
当两个子线程分别将一个祝福写到共享数组a和一个水果放进果盘(变量)后
父线程才去享受：从数组a收取出一个祝福和吃一个水果，如此反复进行若干次*/

/*使用用两个信号量*/

extern int fork(int num);/*要改进fork，让它可以fork多个进程*/
extern void wait();
extern void exit();
extern int GetSema(int value);
extern void FreeSema(int s);
extern void P(int s);
extern void V(int s);
extern void printf(char *s, ...);

void strcpy(char *dst, char *src)
{
	int i;
	for (i = 0; src[i] != '\0'; i++)
	{
		dst[i] = src[i];
	}
	dst[i] = '\0';
}

char words[80];
char fruit;/*0 for empty, 1 for apple, 2 for banana*/

int main()
{
	int pid;
	int fs, ws;
	int i;
	words[0] = '\0';
	fruit = 0;
	fs = GetSema(1);
	ws = GetSema(1);
	pid = fork(2);
	if (pid == -1)
	{
		printf("error in fork\r\n");
		return;
	}
	if (pid == 0)
	{
		for (i = 0; i < 10; i++)
		{
			P(ws);
			P(fs);

			if (words[0] == 0 || fruit == 0)
			{
				i--;
			}
			else
			{
				if (fruit == 1)
				{
					printf("words: %s fruit: apple\r\n", words);
				}
				else
				{
					printf("words: %s fruit: banana\r\n", words);
				}
				words[0] = '\0';
				fruit = 0;
			}

			V(fs);
			V(ws);
		}
		wait();
		FreeSema(fs);
		FreeSema(ws);
	}
	else if (pid == 1)/*大儿子*/
	{
		for (i = 0; i < 10; i++)
		{
			P(ws);
			if (words[0] != '\0')
			{
				i--;
			}
			else
			{
				strcpy(words, "Father will live one year after anther for ever!");
			}
			V(ws);
		}
		exit();
	}
	else if (pid == 2)/*二儿子*/
	{
		for (i = 0; i < 10; i++)
		{
			P(fs);
			if (fruit != 0)
			{
				i--;
			}
			else
			{
				fruit = i % 2 + 1;
			}
			V(fs);
		}
		exit();
	}
}
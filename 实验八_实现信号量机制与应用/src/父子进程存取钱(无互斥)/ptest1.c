/*父亲和儿子利用同一个银行账号存取资金，无互斥同步*/

extern int fork(int num);
extern void printf(char *s, ...);
extern void exit();
extern void wait();

void sleep(int time)
{
	int i, j;
	for (i = 0; i < time * 10000; i++)
	{
		for (j = 0; j < 10000; j++);
	}
}

int bankBalance = 1000;

int main()
{
	int pid;
	int i;
	int t, totalSave = 0, totalDraw = 0;
	pid = fork(1);
	if (pid == -1)
	{
		printf("error in fork\r\n");
		return;
	}
	if (pid == 0)/*父进程存钱，每次10元*/
	{
		for (i = 0; i < 10; i++)
		{
			t = bankBalance;
			sleep(2);
			t += 10;
			sleep(2);
			bankBalance = t;
			totalSave += 10;
			printf("bankBalance=%d,totalSave=%d\r\n", bankBalance, totalSave);
		}
		wait();
	}
	else if (pid == 1)/*子进程取钱，每次20*/
	{
		for (i = 0; i < 10; i++)
		{
			t = bankBalance;
			sleep(2);
			t -= 20;
			sleep(2);
			bankBalance = t;
			totalDraw += 20;
			printf("bankBalance=%d,totalDraw=%d\r\n", bankBalance, totalDraw);
			
		}
		exit();
	}
}
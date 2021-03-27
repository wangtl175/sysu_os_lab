/*父亲和儿子利用同一个银行账号存取资金，有互斥同步*/

extern int fork(int num);
extern void wait();
extern void exit();
extern int GetSema(int value);
extern void FreeSema(int s);
extern void P(int s);
extern void V(int s);
extern void printf(char *s, ...);


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
	int s;
	int t, totalSave = 0, totalDraw = 0;
	/*printf("GetSema(1)\r\n");*/
	s = GetSema(1);
	/*printf("%d\r\n", s);*/
	pid = fork(1);
	if (pid == -1)
	{
		printf("error in fork\r\n");
		return;
	}
	if (pid == 0)/*父进程存钱，每次10元*/
	{
		/*printf("%d\r\n", pid);*/
		for (i = 0; i < 10; i++)
		{
			P(s);
			t = bankBalance;
			sleep(2);
			t += 10;
			sleep(2);
			bankBalance = t;
			totalSave += 10;
			printf("bankBalance=%d,totalSave=%d\r\n", bankBalance, totalSave);
			V(s);
		}
		wait();
		FreeSema(s);
	}
	else if (pid == 1)/*子进程取钱，每次20*/
	{
		/*printf("%d\r\n", pid);*/
		for (i = 0; i < 10; i++)
		{
			P(s);
			t = bankBalance;
			sleep(2);
			t -= 20;
			sleep(2);
			bankBalance = t;
			totalDraw += 20;
			printf("bankBalance=%d,totalDraw=%d\r\n", bankBalance, totalDraw);
			V(s);
			/*不用exit，因为exit要和wait配套使用*/
		}
		exit();
	}
}
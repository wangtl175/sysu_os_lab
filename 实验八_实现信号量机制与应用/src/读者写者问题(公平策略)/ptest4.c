/*����д�����⣬��ƽ����*/

#define ReaderNum 3
#define WriterNum 2

extern int fork(int num);
extern void wait();
extern void exit();
extern int GetSema(int value);
extern void FreeSema(int s);
extern void P(int s);
extern void V(int s);
extern void printf(char *s, ...);

int reader_counter = 0;

int main()
{
	int wmutex, rmutex, mutex;/*����д�ߣ�������ߣ�������ߺ�д��*/
	int pid;
	int i;
	wmutex = GetSema(1);
	rmutex = GetSema(1);
	mutex = GetSema(1);
	/*printf("w %d r %d m %d\r\n", wmutex, rmutex, mutex);*/
	pid = fork(ReaderNum + WriterNum);/*0 for father 1-ReaderNum for reader, other of writer*/
	if (pid == -1)
	{
		printf("error in fork\r\n");
		return;
	}
	if (pid == 0)/*father*/
	{
		wait();
		FreeSema(wmutex);
		FreeSema(rmutex);
		FreeSema(mutex);
	}
	else if (1 <= pid && pid <= ReaderNum)/*reader*/
	{
		for (i = 0; i < 2; i++)
		{
			P(rmutex);
			if (reader_counter == 0)
			{
				P(mutex);/*��д��д��ʱ�����߲ſ��Զ�*/
			}
			reader_counter++;
			V(rmutex);
			/*��ʱ�������߿��Խ���*/
			printf("reader %d is reading, num of readers %d\r\n", pid - 1, reader_counter);

			P(rmutex);
			reader_counter--;
			if (reader_counter == 0)
			{
				V(mutex);/*�����н���Ķ��߶�����ʱ��д�߲ſ���д*/
			}
			V(rmutex);
		}
		exit();
	}
	else/*writer*/
	{
		for (i = 0; i < 3; i++)
		{
			P(wmutex);/*ֻ��һ��д�߿��Խ���*/
			P(mutex);/*������ǰ����߶���ʱ����д*/

			printf("writer %d is writing\r\n", pid - ReaderNum - 1);
			V(mutex);
			V(wmutex);
		}
		exit();
	}
}
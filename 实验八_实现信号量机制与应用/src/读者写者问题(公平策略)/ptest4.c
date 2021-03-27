/*读者写者问题，公平方案*/

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
	int wmutex, rmutex, mutex;/*互斥写者，互斥读者，互斥读者和写者*/
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
				P(mutex);/*当写者写完时，读者才可以读*/
			}
			reader_counter++;
			V(rmutex);
			/*此时其它读者可以进入*/
			printf("reader %d is reading, num of readers %d\r\n", pid - 1, reader_counter);

			P(rmutex);
			reader_counter--;
			if (reader_counter == 0)
			{
				V(mutex);/*当所有进入的读者都读完时，写者才可以写*/
			}
			V(rmutex);
		}
		exit();
	}
	else/*writer*/
	{
		for (i = 0; i < 3; i++)
		{
			P(wmutex);/*只有一个写者可以进入*/
			P(mutex);/*当队列前面读者读完时，才写*/

			printf("writer %d is writing\r\n", pid - ReaderNum - 1);
			V(mutex);
			V(wmutex);
		}
		exit();
	}
}
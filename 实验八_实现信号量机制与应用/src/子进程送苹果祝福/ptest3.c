/*���߳�f�����������߳�s��d
������߳�s�������߳�fף��
С���ӽ���d�����򸸽�����ˮ��(ÿ��һ��ƻ��������ˮ��)
���������̷ֱ߳�һ��ף��д����������a��һ��ˮ���Ž�����(����)��
���̲߳�ȥ���ܣ�������a��ȡ��һ��ף���ͳ�һ��ˮ������˷����������ɴ�*/

/*ʹ���������ź���*/

extern int fork(int num);/*Ҫ�Ľ�fork����������fork�������*/
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
	else if (pid == 1)/*�����*/
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
	else if (pid == 2)/*������*/
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
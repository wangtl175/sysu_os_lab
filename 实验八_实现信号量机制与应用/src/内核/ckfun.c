/*ע�Ͳ�����//*/

/*�û�����countʹ��ϵͳ��������һ���ַ���ֹͣ*/

#define MaxProcessNum 11/*�Ӷ���һ��ϵͳPCB*/
#define NameLen 100

#define SizeOfStack 0x100
#define StackSeg 0x1000

#define NRsem 10/*�ź�������*/


extern int loadblock(int es, int bx, int blockid, int num);
extern void cleanscreen();
extern void printc(char c);
extern char readkeypress();
extern void LoadINT8H();
extern void LoadINT20H();
extern void LoadINT21H();
extern void LoadINT22H();
extern int initalStack(char *sp);/*��sp��ȥ���ɣ�����ֵ����ջ��sp���ε�ַ����һ����*/
extern void LoadPSP(int cs);/*���ش����ǰ׺��ָ���Ķ�*/
extern void setIF();/*���ж�*/
extern void clrIF();/*���ж�*/

void strcpy(char *dest, char *source)
{
	int i;
	for (i = 0; source[i] != 0; i++)
	{
		dest[i] = source[i];
	}
	dest[i] = 0;
}

void memcpy(void *src, void *dest, int len)/*lenΪҪ�������ֽ���*/
{
	char *s = (char *)src;
	char *d = (char *)dest;
	int i;
	for (i = 0; i < len; i++)
	{
		d[i] = s[i];
	}
}

void printf(char *s)
{
	int i;
	for (i = 0; s[i] != 0; i++)
	{
		printc(s[i]);
	}
	return;
}

void scanf(char *s)
{
	int i = 0;
	char c;
	while (1)
	{
		/*printc('a');*/
		c = readkeypress();
		/*printc('b');*/
		if (c == 0x0d)
		{
			s[i] = 0;
			printc(0x0a);
			printc(0x0d);
			break;
		}
		else if (c == 0x08)/*Backspace*/
		{
			if (i == 0)
			{
				continue;
			}
			i--;
			printc(0x08);
			printc(' ');
			printc(0x08);
			continue;
		}
		s[i] = c;
		i++;
		printc(c);
	}
	return;
}

typedef struct
{
	int ax;/*ƫ��0*/
	int bx;
	int cx;
	int dx;
	int di;
	int bp;
	int es;
	/*���ȱ���,�����ڶ��ָ�*/
	int ds;/*14*/
	int si;
	int ss;/*18*/
	int sp;
	int ip;
	int cs;
	int flags;/*ƫ��26*/
}cpuState;

typedef struct aStruct
{
	cpuState cpu;
	int pid;
	char name[NameLen + 1];
	char state;/*����״̬,0:PCB���У�1:�������У�2:���̾�����3:��������*/
	int fid;/*�����̵�id������Լ����Ǹ����̣���fid==pid*/
	struct aStruct *next;/*���ź����������γ�һ��������ʾ��������*/
}PCB;


typedef struct/*�ź���*/
{
	int count;
	PCB *next;
	char used;
}semaphone;

semaphone semList[NRsem];

PCB pcbList[MaxProcessNum];/*���һ����ϵͳ�Ľ���*/
char stackList[MaxProcessNum - 1][SizeOfStack];

PCB *currentPCB = &pcbList[MaxProcessNum - 1];/*ָ��ǰ���е�PCB��*/
PCB *kernelPCB = &pcbList[MaxProcessNum - 1];

int numOfProcess = 0;/*�û����̸���*/


void initPCBList()
{
	int i;
	for (i = 0; i < MaxProcessNum; i++)
	{
		pcbList[i].state = 0;
		pcbList[i].next = 0;
	}
	kernelPCB->state = 1;
	kernelPCB->pid = MaxProcessNum - 1;
	kernelPCB->fid = MaxProcessNum - 1;
	strcpy(kernelPCB->name, "kernel");
}

void switchProcess()
{
	/*currentPCB->state��3�����0��1��2*/
	/*Ҫִ�е�PCB��stateΪ0*/
	/*û��Ҫִ�еĽ��̣����ز���ϵͳ*/
	int i;
	if (numOfProcess == 0)
	{
		currentPCB = &pcbList[MaxProcessNum - 1];
		currentPCB->state = 1;
		return;
	}
	for (i = (currentPCB->pid + 1) % MaxProcessNum; i != currentPCB->pid; i++, i %= MaxProcessNum)
	{
		if (pcbList[i].state == 2)
		{
			if (currentPCB->state == 1)
			{
				currentPCB->state = 2;
			}
			/*currentPCB->state = 2;*//*��do_exit�е���switchProcess��currentPCB->state�ǵ���0�ģ����ܰ�����Ϊ2*/
			pcbList[i].state = 1;
			currentPCB = &pcbList[i];
			return;
		}
	}
}

/*�����ɹ����򷵻�pid�����򷵻�-1*/
/*(pid+2)*0x1000�������ڶ�*/
/*һ�������18������*/
int createProcess(int blockid, int numOfblock)
{
	int i, seg, cid, bid;/*cid:���棬bid:������*/
	for (i = 0; i < MaxProcessNum; i++)
	{
		if (pcbList[i].state == 0)
		{
			break;
		}
	}
	if (i == MaxProcessNum)
	{
		printf("Error: create process failed\r\n");
		return 0;
	}
	seg = (i + 2) * 0x1000;
	pcbList[i].cpu.cs = seg;
	pcbList[i].cpu.ds = seg;
	pcbList[i].cpu.es = seg;
	pcbList[i].cpu.ip = 0x100;
	/*��ʼ�����̵�ջ(ѹһ��0x00)*/
	/*pcbList[i].cpu.ss = stackList[i];*//*��ʱ���Եģ�ssӦ��Ҫ�����ں˵Ķε�ַ,��ջ���Ƶ�һ��û���õ����ڴ�*/
	pcbList[i].cpu.ss = StackSeg;
	pcbList[i].cpu.sp = initalStack(&stackList[i][SizeOfStack]);
	/*��ʼ����־�Ĵ���*/
	pcbList[i].cpu.flags = 512;

	pcbList[i].pid = i;
	pcbList[i].fid = i;

	pcbList[i].name[0] = '0' + i;
	pcbList[i].name[1] = 0;
	LoadPSP(seg);
	loadblock(seg, 0x100, blockid, numOfblock);
/*	while (numOfblock > 0)
	{
		bid = blockid % 18 + 1;
		cid = (blockid-1) / 18;
		if (bid + numOfblock - 1 > 18)
		{
			loadblock(seg, 0x100, cid, bid, 18 - bid + 1);
		}
	}*/

	pcbList[i].state = 2;


	numOfProcess += 1;
	/*printf("createProcess\r\n");*/
	return i;
}

void endProcess()/*��������*/
{
	currentPCB->state = 0;
	numOfProcess--;
	/*printc('0' + currentPCB->pid);
	printf(" endProcess\r\n");*/
	/*Ҫ�����￪�ж�*/
	setIF();
	while (1);/*���������ֱ��һ��ʱ��Ƭ����*/
}

/*fork*/
/*��fork����Ϳ�ʼ�˲��У����ǣ���ϵͳ�����У����ڲ����ж�Ƕ�ף����Ծͺ��Ѳ��У���˲���ϵͳ������*/
/*���������Ͱ�sp�������ӽ��̣�����ֻ�и�����ִ����δ��룬���޸�sp��ֵ���ᵼ�¸��ӽ��̲�һ��*/
/*���о��ǣ����ʹ��ϵͳ���õĻ���ֻ�и����̻᷵�أ����ӽ��̵�spҲ�᲻һ��*/
/*�������ϵͳ���ã���һ��ʱ��Ҫ���жϣ����縸��PCB��*/
/*���ң�������PCB�鲻�����µģ����Ի�����ϵͳ���ã���ϵͳ����ʱ��ʹ��_save���¸�����PCB*/
/*�ӽ��̺͸����̹������κ����ݶΣ�����ջ�β������ӽ��̴Ӹ������п���ջ����ʵ�ǿ�����һ����*/
/*ջ�η��룬ֻ����ջ�Σ�0x100���ֽ�*/

/*do_fork����Ҫ������*/
/*1����һ���յ�PCB�飬Ȼ��Ѹ����̵ĸ�ֵ��ȥ��ֻ�޸�ss*/
/*2���޸��ӽ��̵�fid*/
/*3������pid��fid��ȷ���Ǹ��Ǹ����̣��Ǹ����ӽ��̣�Ȼ��ȷ������ֵ*/

void do_fork()
{
	int i;
	int num = currentPCB->cpu.bx;
	for (i = (currentPCB->pid + 1) % MaxProcessNum; i != currentPCB->pid; i++, i %= MaxProcessNum)
	{
		if (pcbList[i].state == 0)
		{
			memcpy(&pcbList[currentPCB->pid], &pcbList[i], sizeof(PCB));/*��copy֮ǰ��������Ҫsaveһ�£�ʹ��PCB����*/
			memcpy(stackList[currentPCB->pid], stackList[i], SizeOfStack);
			
			pcbList[i].state = 2;
			pcbList[i].fid = currentPCB->pid;
			pcbList[i].pid = i;
			pcbList[i].cpu.sp += ((i - currentPCB->pid) * SizeOfStack);/*���ӽ������ڵĶ���ͬ������ջ��ָ�벻ͬ������֮�����n��SizeOfStack*/

			pcbList[i].cpu.ax = num;/*�ӽ���*/

			numOfProcess++;
			num--;
			if (num == 0)
			{
				currentPCB->cpu.ax = 0;/*������*/
				return;
			}
			/*printf("do_fork\r\n");*/
		}
	}
	currentPCB->cpu.ax = -1;/*����ʧ��*/
}

/*do_exit*/
/*����Ҫ���ӽ����õģ��Ѹ����̵�״̬��Ϊ����1���Լ��ĸĳ�0��Ȼ��ͣס(��ѭ���Ϳ�����)*/

void do_exit()
{
	if (pcbList[currentPCB->fid].state == 3)
	{
		pcbList[currentPCB->fid].state = 2;
	}
	currentPCB->state = 0;
	numOfProcess--;
/*	printc('0' + currentPCB->pid);
	printf(" do_exit ");
	printc('0' + pcbList[1].state);*/
	switchProcess();
/*	printc('0' + currentPCB->pid);
	printf("\r\n");*/
/*	setIF();
	while (1);*/
	/*switchProcess();*/
}

/*do_wait*/
/*����Ҫ�Ǹ������õ�*/
/*������״̬��Ϊ����3��Ȼ��ͣס���ؼ��������ͣס��һֱwhile���currentPCB->state*/
/*������Ƴ���һ������ֵ����Ҫ����ʱΪ1��������ʱΪ0�������Ϳ�����������һ��ѭ����ʵ��*/
/*��������������wait����ӽ���*/
void do_wait()
{
	int i;
	for (i = 0; i < MaxProcessNum; i++)
	{
		if (pcbList[i].state != 0 && pcbList[i].fid == currentPCB->pid && i != currentPCB->pid)/*���Լ���pid����fid*/
		{
			currentPCB->state = 3;
			currentPCB->cpu.ax = 0;
			/*printc('0' + currentPCB->pid);
			printf(" do_wait ");
			printc('0' + i);
			printf("\r\n");*/
			switchProcess();
			return;
		}
	}
	currentPCB->cpu.ax = 1;
}

void initSemList()
{
	int i = 0;
	for (; i < NRsem; i++)
	{
		semList[i].used = 0;
		semList[i].next = 0;
	}
}

void do_GetSema()/*����s��currentPCB->bx��*/
{
	int i = 0;
	/*printf("do_GetSema\r\n");*/
	for (; i < NRsem; i++)
	{
		if (semList[i].used == 0)
		{
			semList[i].used = 1;
			semList[i].count = currentPCB->cpu.bx;
			currentPCB->cpu.ax = i;
			/*printc(currentPCB->cpu.ax + '0');
			printf("\r\n");*/
			return;
		}
	}
	currentPCB->cpu.ax = -1;
}

void do_FreeSema()
{
	semList[currentPCB->cpu.bx].used = 0;
}

void do_P()
{
	/*printc('0' + currentPCB->cpu.ax);
	printf("\r\n");*/
	semList[currentPCB->cpu.bx].count--;
	if (semList[currentPCB->cpu.bx].count < 0)
	{
		currentPCB->state = 3;
		currentPCB->next = semList[currentPCB->cpu.bx].next;
		semList[currentPCB->cpu.bx].next = currentPCB;
		switchProcess();
	}
}

void do_V()
{
	PCB *temp;
	semList[currentPCB->cpu.bx].count++;
	if (semList[currentPCB->cpu.bx].count <= 0)
	{
		temp = semList[currentPCB->cpu.bx].next;
		semList[currentPCB->cpu.bx].next = semList[currentPCB->cpu.bx].next->next;
		temp->state = 2;
	}
}

void kernelmain()
{
	char c;
	int i;
	initPCBList();
	initSemList();
	LoadINT8H();
	LoadINT20H();
	LoadINT21H();
	LoadINT22H();

	while (1)
	{
		printf("m for message, l to list, c to clean screen, 1-4 to select program: ");
		scanf(&c);
		switch (c)
		{
		case 'm':
			printf("Designed by 18340168 wangtianlong\n\r");
			break;
		case 'l':
			printf("1.Ptest1\r\n");
			printf("2.Ptest2\r\n");
			printf("4.Ptest3\r\n");
			printf("5.Ptest4\r\n");
			break;
		case 'c':
			cleanscreen();
			break;
		case '1':
			createProcess(8, 3);
			break;
		case '2':
			createProcess(11, 3);
			break;
		case '3':
			createProcess(14, 3);
			break;
		case '4':
			/*for (i = 0; i < MaxProcessNum; i++)
			{
				printc('0' + pcbList[i].state);
				printc(' ');
			}
			printf("\r\n");*/
			createProcess(17, 3);
			break;
		default:
			printf("error\r\n");
			break;
		}
		while (numOfProcess != 0);/*������������ֱ�����̶����*/
	}
}


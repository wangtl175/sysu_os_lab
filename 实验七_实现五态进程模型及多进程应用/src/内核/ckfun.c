/*ע�Ͳ�����//*/

/*�û�����countʹ��ϵͳ��������һ���ַ���ֹͣ*/

#define MaxProcessNum 11/*�Ӷ���һ��ϵͳPCB*/
#define NameLen 10

#define SizeOfStack 0x100
#define StackSeg 0x1000


extern int loadblock(int es, int bx, char id, int num);
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

typedef struct
{
	cpuState cpu;
	int pid;
	char name[NameLen + 1];
	char state;/*����״̬,0:PCB���У�1:�������У�2:���̾�����3:��������*/
	int fid;/*�����̵�id������Լ����Ǹ����̣���fid==pid*/
}PCB;

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
	}
	kernelPCB->state = 1;
	kernelPCB->pid = MaxProcessNum - 1;
	strcpy(kernelPCB->name, "kernel");
}

void switchProcess()
{
	/*currentPCB->state��2�����0��1*/
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
			currentPCB->state = 2;
			pcbList[i].state = 1;
			currentPCB = &pcbList[i];
			return;
		}
	}
}

/*�����ɹ����򷵻�pid�����򷵻�-1*/
/*(pid+2)*0x1000�������ڶ�*/
int createProcess(int blockid, int numOfblock)
{
	int i, seg;
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
	pcbList[i].state = 2;


	numOfProcess += 1;
	return i;
}

void endProcess()/*��������*/
{
	currentPCB->state = 0;
	numOfProcess--;
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

			pcbList[i].cpu.ax = 0;/*�ӽ���*/

			currentPCB->cpu.ax = 1;/*������*/
			numOfProcess++;
			break;
		}
	}
	if (i == currentPCB->pid)
	{
		currentPCB->cpu.ax = -1;/*����ʧ��*/
	}
}

/*do_exit*/
/*����Ҫ���ӽ����õģ��Ѹ����̵�״̬��Ϊ����1���Լ��ĸĳ�0��Ȼ��ͣס(��ѭ���Ϳ�����)*/

void do_exit()
{
	pcbList[currentPCB->fid].state = 2;
	currentPCB->state = 0;
	numOfProcess--;
	setIF();
	while (1);
	/*switchProcess();*/
}

/*do_wait*/
/*����Ҫ�Ǹ������õ�*/
/*������״̬��Ϊ����3��Ȼ��ͣס���ؼ��������ͣס��һֱwhile���currentPCB->state*/

void do_wait()
{
	currentPCB->state = 3;
	switchProcess();
}

void kernelmain()
{
	char c;
	initPCBList();
	LoadINT8H();
	LoadINT20H();
	LoadINT21H();
	LoadINT22H();

	while (1)
	{
		printf("m for message, l to list, c to clean screen, 1-8 to select program: ");
		scanf(&c);
		switch (c)
		{
		case 'm':
			printf("Designed by 18340168 wangtianlong\n\r");
			break;
		case 'l':
			printf("1.Up_Lt, block: 7\n\r");
			printf("2.Up_Rt, block: 8\n\r");
			printf("3.Dn_Lt, block: 9\n\r");
			printf("4.Dn_Rt, block: 10\n\r");
			printf("5.Count, block: 11 12\n\r");
			printf("6.SYCall , block: 13 14\n\r");
			printf("7.INT, block: 15\n\r");
			printf("8.Multi-Thread, block: 16 17\r\n");
			printf("9.Multi-Progress\n\r");
			break;
		case 'c':
			cleanscreen();
			break;
		case '1':
			createProcess(7, 1);
			break;
		case '2':
			createProcess(8, 1);
			break;
		case '3':
			createProcess(9, 1);
			break;
		case '4':
			createProcess(10, 1);
			break;
		case '5':
			createProcess(11, 2);
			break;
		case '6':
			createProcess(13, 2);
			break;
		case '7':
			createProcess(15, 1);
			break;
		case '8':
			createProcess(16, 2);
			break;
		case '9':
			createProcess(7, 1);
			createProcess(8, 1);
			createProcess(9, 1);
			createProcess(10, 1);
			break;
		default:
			printf("error\r\n");
			break;
		}
		while (numOfProcess != 0);/*������������ֱ�����̶����*/
	}
}


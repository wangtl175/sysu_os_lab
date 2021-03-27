/*注释不能用//*/

/*用户程序count使用系统调用输入一个字符后，停止*/

#define MaxProcessNum 11/*加多了一个系统PCB*/
#define NameLen 10
extern int loadblock(int es, int bx, char id, int num);
extern void cleanscreen();
extern void printc(char c);
extern char readkeypress();
extern void LoadINT8H();
extern void LoadINT20H();
extern void LoadINT21H();
extern void LoadINT22H();
extern int initalStack(int cs);/*传段地址进去即可，返回值是新栈的sp*/
extern void LoadPSP(int cs);/*加载代码段前缀到指定的段*/
extern void setIF();

void strcpy(char *dest, char *source)
{
	int i;
	for (i = 0; source[i] != 0; i++)
	{
		dest[i] = source[i];
	}
	dest[i] = 0;
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
	int ax;/*偏移0*/
	int bx;
	int cx;
	int dx;
	int di;
	int bp;
	int es;
	/*最先保存,倒数第二恢复*/
	int ds;/*14*/
	int si;
	int ss;/*18*/
	int sp;
	int ip;
	int cs;
	int flags;/*偏移26*/
}cpuState;

typedef struct
{
	cpuState cpu;
	int pid;
	char name[NameLen + 1];
	char state;/*三种状态,0:PCB空闲，1:进程阻塞，2:进程运行*/
}PCB;

PCB pcbList[MaxProcessNum];/*最后一个是系统的进程*/

PCB *currentPCB = &pcbList[MaxProcessNum - 1];/*指向当前运行的PCB块*/
PCB *kernelPCB = &pcbList[MaxProcessNum - 1];

int numOfProcess = 0;/*用户进程个数*/

/*如何开始?*/
/*首先加载内存块，然后创建PCB但是state为阻塞(loadblock里面操作)，所有都创建完成后，选择一个开始执行*/


/*切换时，先save，然后修改currentPCB(c语言)，然后restart就成功了*/
/*这些在时钟中断里执行*/
/*时钟中断首先save,然后风火轮，然后调用修改currentPCB的函数，最后restart*/

/*进程结束后，返回到内核，numOfProcess-1，pid对应的PCB块置为空闲*/

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
	/*currentPCB->state有2中情况0和1*/
	/*要执行的PCB的state为0*/
	/*没有要执行的进程，返回操作系统*/
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

/*创建成功，则返回pid，否则返回-1*/
int createProcess(int cs, int ip, int blockid, int numOfblock)
{
	int i;
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
	pcbList[i].cpu.cs = cs;
	pcbList[i].cpu.ds = cs;
	pcbList[i].cpu.es = cs;
	pcbList[i].cpu.ip = ip;
	/*初始化进程的栈(压一个0x00)*/
	pcbList[i].cpu.ss = cs;
	pcbList[i].cpu.sp = initalStack(cs);
	/*初始化标志寄存器*/
	pcbList[i].cpu.flags = 512;

	pcbList[i].pid = i;
	pcbList[i].name[0] = '0' + i;
	pcbList[i].name[1] = 0;
	LoadPSP(cs);
	loadblock(cs, ip, blockid, numOfblock);
	pcbList[i].state = 2;
	numOfProcess += 1;
	return i;
}

void endProcess()/*结束进程*/
{
	currentPCB->state = 0;
	numOfProcess--;
	/*要在这里开中断*/
	setIF();
	while (1);/*阻塞在这里，直至一个时间片结束*/
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
			printf("6.Test , block: 13 14\n\r");
			printf("7.Test1, block: 15\n\r");
			printf("8.Multi-Progress\n\r");
			break;
		case 'c':
			cleanscreen();
			break;
		case '1':
			createProcess(0x2000, 0x100, 7, 1);
			break;
		case '2':
			createProcess(0x2000, 0x100, 8, 1);
			break;
		case '3':
			createProcess(0x2000, 0x100, 9, 1);
			break;
		case '4':
			createProcess(0x2000, 0x100, 10, 1);
			break;
		case '5':
			createProcess(0x2000, 0x100, 11, 2);
			break;
		case '6':
			createProcess(0x2000, 0x100, 13, 2);
			break;
		case '7':
			createProcess(0x2000, 0x100, 15, 1);
			break;
		case '8':
			createProcess(0x2000, 0x100, 7, 1);
			createProcess(0x3000, 0x100, 8, 1);
			createProcess(0x4000, 0x100, 9, 1);
			createProcess(0x5000, 0x100, 10, 1);
			break;
		default:
			printf("error\r\n");
			break;
		}
		while (numOfProcess != 0);/*在这里阻塞，直至进程都完成*/
	}
}


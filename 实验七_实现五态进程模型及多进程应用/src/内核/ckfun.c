/*注释不能用//*/

/*用户程序count使用系统调用输入一个字符后，停止*/

#define MaxProcessNum 11/*加多了一个系统PCB*/
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
extern int initalStack(char *sp);/*把sp进去即可，返回值是新栈的sp，段地址都是一样的*/
extern void LoadPSP(int cs);/*加载代码段前缀到指定的段*/
extern void setIF();/*开中断*/
extern void clrIF();/*关中断*/

void strcpy(char *dest, char *source)
{
	int i;
	for (i = 0; source[i] != 0; i++)
	{
		dest[i] = source[i];
	}
	dest[i] = 0;
}

void memcpy(void *src, void *dest, int len)/*len为要拷贝的字节数*/
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
	char state;/*四种状态,0:PCB空闲，1:进程运行，2:进程就绪，3:进程阻塞*/
	int fid;/*父进程的id，如果自己就是父进程，则fid==pid*/
}PCB;

PCB pcbList[MaxProcessNum];/*最后一个是系统的进程*/
char stackList[MaxProcessNum - 1][SizeOfStack];

PCB *currentPCB = &pcbList[MaxProcessNum - 1];/*指向当前运行的PCB块*/
PCB *kernelPCB = &pcbList[MaxProcessNum - 1];

int numOfProcess = 0;/*用户进程个数*/


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
/*(pid+2)*0x1000就是所在段*/
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
	/*初始化进程的栈(压一个0x00)*/
	/*pcbList[i].cpu.ss = stackList[i];*//*这时不对的，ss应该要等于内核的段地址,把栈段移到一段没有用到的内存*/
	pcbList[i].cpu.ss = StackSeg;
	pcbList[i].cpu.sp = initalStack(&stackList[i][SizeOfStack]);
	/*初始化标志寄存器*/
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

void endProcess()/*结束进程*/
{
	currentPCB->state = 0;
	numOfProcess--;
	/*要在这里开中断*/
	setIF();
	while (1);/*阻塞在这里，直至一个时间片结束*/
}

/*fork*/
/*从fork里面就开始了并行，但是，再系统调用中，由于不做中断嵌套，所以就很难并行，因此不做系统调用了*/
/*如果在这里就把sp交给了子进程，由于只有父进程执行这段代码，会修改sp的值，会导致父子进程不一致*/
/*还有就是，如果使用系统调用的话，只有父进程会返回，父子进程的sp也会不一致*/
/*如果不用系统调用，有一段时间要关中断，例如父子PCB块*/
/*而且，父进程PCB块不是最新的，所以还是用系统调用，再系统调用时，使用_save更新父进程PCB*/
/*子进程和父进程共享代码段和数据段，但是栈段不共享，子进程从父进程中拷贝栈，其实是拷贝整一个段*/
/*栈段分离，只拷贝栈段，0x100个字节*/

/*do_fork具体要做的事*/
/*1、找一个空的PCB块，然后把父进程的赋值进去，只修改ss*/
/*2、修改子进程的fid*/
/*3、根据pid和fid，确定那个是父进程，那个是子进程，然后确定返回值*/

void do_fork()
{
	int i;
	for (i = (currentPCB->pid + 1) % MaxProcessNum; i != currentPCB->pid; i++, i %= MaxProcessNum)
	{
		if (pcbList[i].state == 0)
		{
			memcpy(&pcbList[currentPCB->pid], &pcbList[i], sizeof(PCB));/*再copy之前，父进程要save一下，使得PCB最新*/
			memcpy(stackList[currentPCB->pid], stackList[i], SizeOfStack);
			
			pcbList[i].state = 2;
			pcbList[i].fid = currentPCB->pid;
			pcbList[i].pid = i;
			pcbList[i].cpu.sp += ((i - currentPCB->pid) * SizeOfStack);/*父子进程所在的段相同，但是栈顶指针不同，两个之间相差n个SizeOfStack*/

			pcbList[i].cpu.ax = 0;/*子进程*/

			currentPCB->cpu.ax = 1;/*父进程*/
			numOfProcess++;
			break;
		}
	}
	if (i == currentPCB->pid)
	{
		currentPCB->cpu.ax = -1;/*创建失败*/
	}
}

/*do_exit*/
/*这主要是子进程用的，把父进程的状态改为就绪1，自己的改成0，然后停住(死循环就可以了)*/

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
/*这主要是父进程用的*/
/*父进程状态改为阻塞3，然后停住，关键在于如何停住，一直while坚持currentPCB->state*/

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
		while (numOfProcess != 0);/*在这里阻塞，直至进程都完成*/
	}
}


/*注释不能用//*/
extern int loadblock(int es, int bx, char id, int num);
extern int jump(int cs, int ip);
extern void cleanscreen();
extern void printc(char c);
extern void LoadOuch();
extern void LoadWhell();
extern void LoadINT20H();
extern void LoadINT21H();
extern void LoadINT22H();


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
		c = readkeypress();
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

struct cpuStat
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
};



struct cpuStat CurrentState;



void kernelmain()
{
	char c;
	LoadOuch();
	LoadWhell();
	LoadINT20H();
	LoadINT21H();
	LoadINT22H();

	while (1)
	{
		printf("m for message, l to list, c to clean screen, 1-7 to select program: ");
		scanf(&c);
		switch (c)
		{
		case 'm':
			printf("Designed by 18340168 wangtianlong\n\r");
			break;
		case 'l':
			printf("1.Up_Lt, block: 6\n\r");
			printf("2.Up_Rt, block: 7\n\r");
			printf("3.Dn_Lt, block: 8\n\r");
			printf("4.Dn_Rt, block: 9\n\r");
			printf("5.Count, block: 10 11\n\r");
			printf("6.Test , block: 12 13\n\r");
			printf("7.Test1, block: 14\n\r");
			break;
		case 'c':
			cleanscreen();
			break;
		case '1':
			loadblock(0x2000, 0x100, 6, 1);
			jump(0x2000, 0x100);
			break;
		case '2':
			loadblock(0x2000, 0x100, 7, 1);
			jump(0x2000, 0x100);
			break;
		case '3':
			loadblock(0x2000, 0x100, 8, 1);
			jump(0x2000, 0x100);
			break;
		case '4':
			loadblock(0x2000, 0x100, 9, 1);
			jump(0x2000, 0x100);
			break;
		case '5':
			loadblock(0x2000, 0x100, 10, 2);
			jump(0x2000, 0x100);
			break;
		case '6':
			loadblock(0x2000, 0x100, 12, 2);
			jump(0x2000, 0x100);
			break;
		case '7':
			loadblock(0x2000, 0x100, 14, 1);
			jump(0x2000, 0x100);
			break;
		default:
			printf("error\r\n");
			break;
		}
	}
}


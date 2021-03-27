/*×¢ÊÍ²»ÄÜÓÃ//*/
extern int loadblock(int es, int bx, char id, int num);
extern int jump(int cs, int ip);
extern void cleanscreen();
extern void scanf(char *s);
extern void printf(char *s);
extern void LoadOuch();
extern void LoadWhell();
void kernelmain()
{
	char c;
	LoadOuch();
	LoadWhell();

	while (1)
	{
		printf("m for message, l to list, c to clean screen, 1-5 to select program: ");
		scanf(&c);
		switch (c)
		{
		case 'm':
			printf("Designed by 18340168 wangtianlong\n\r");
			break;
		case 'l':
			printf("1.Up_Lt, block: 5\n\r");
			printf("2.Up_Rt, block: 6\n\r");
			printf("3.Dn_Lt, block: 7\n\r");
			printf("4.Dn_Rt, block: 8\n\r");
			printf("5.Count, block: 9 10\n\r");
			break;
		case 'c':
			cleanscreen();
			break;
		case '1':
			loadblock(0x2000, 0x100, 5, 1);
			jump(0x2000, 0x100);
			break;
		case '2':
			loadblock(0x2000, 0x100, 6, 1);
			jump(0x2000, 0x100);
			break;
		case '3':
			loadblock(0x2000, 0x100, 7, 1);
			jump(0x2000, 0x100);
			break;
		case '4':
			loadblock(0x2000, 0x100, 8, 1);
			jump(0x2000, 0x100);
			break;
		case '5':
			loadblock(0x2000, 0x100, 9, 2);
			jump(0x2000, 0x100);
			break;
		default:
			printf("error\r\n");
			break;
		}
	}
}


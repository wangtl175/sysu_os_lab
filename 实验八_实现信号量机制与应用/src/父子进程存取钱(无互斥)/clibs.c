extern char readkeypress(void);
extern void putch(char c);

/*要显示或者输入的数据都要求是在数据段中，即在函数体之外*/

char snum[6];/*0xffff=65535，最多5位*/

char *i2s(int n)
{
	int i = 0, t = 10, s = 0;
	if (n == 0)
	{
		snum[0] = '0';
		snum[1] = 0;
		return snum;
	}
	while (n != 0)
	{
		snum[i] = n % t + '0';
		n /= 10;
		i++;
		s++;
	}
	for (i = 0; i < s / 2; i++)
	{
		char temp = snum[i];
		snum[i] = snum[s - 1 - i];
		snum[s - 1 - i] = temp;
	}
	snum[s] = 0;
	return snum;
}

/*把snum变成整数*/
int s2i(void)
{
	int i, s = 0;
	for (i = 0; snum[i] != 0; i++)
	{
		s *= 10;
		s += (snum[i] - '0');
	}
	return s;
}

void puts(char *s)
{
	int i;/*如果这里从栈段中取地址*/
	for (i = 0; s[i] != 0; i++)
	{
		putch(s[i]);
	}
}

void getch(char *c)
{
	char t = 0;
	*c = 0;
	while (1)
	{
		t = *c;
		*c = readkeypress();
		if (*c == 0x0d)
		{
			*c = t;
			putch(0x0a);
			putch(0x0d);
			break;
		}
		putch(*c);
	}
	return;
}

void gets(char *s)
{
	int i = 0;
	char c;
	while (1)
	{
		c = readkeypress();
		if (c == 0x0d)
		{
			s[i] = 0;
			putch(0x0a);
			putch(0x0d);
			break;
		}
		else if (c == 0x08)/*Backspace*/
		{
			if (i == 0)
			{
				continue;
			}
			i--;
			putch(0x08);
			putch(' ');
			putch(0x08);
			continue;
		}
		s[i] = c;/*s[i]为[bx+si]这里把读到的字符写到数据段中了*/
		i++;
		putch(c);
	}
	return;
}
/*参考https://zhuanlan.zhihu.com/p/45153186*/
void printf(char *str, ...)
{
	int i;
	int parOffset = 1;
	int par;
	for (i = 0; str[i] != 0; i++)
	{
		if (str[i] != '%')
		{
			putch(str[i]);
		}
		else
		{
			i++;
			par = (int)*(&str + parOffset);
			switch (str[i])
			{
			case 'd':
				puts(i2s(par));/*指针+1实际上加的是2(一个单位)*/
				break;
			case 'c':
				putch((char)(par));
				break;
			case 's':
				puts((char *)(par));
				break;
			default:
				continue;
			}
			parOffset += 1;
		}
	}
}

void scanf(char *str, ...)
{
	int parOffset = 1;
	int i;
	int *par;
	for (i = 0; str[i] != 0; i++)
	{
		if (str[i] == '%')
		{
			i++;
			par = (int *)*(&str + parOffset);
			switch (str[i])
			{
			case 'd':
				gets(snum);
				*par = s2i();
				break;
			case 'c':
				getch((char *)(par));
				break;
			case 's':
				gets((char *)(par));
				break;
			default:
				continue;
			}
			parOffset += 1;
		}
	}
}
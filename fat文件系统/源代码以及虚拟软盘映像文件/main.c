#include<stdio.h>
#include<string.h>
#include"fat12.h"

#define TRUE 1


int main()
{
	char command[10];
	openFAT("dossys.img");
	while (TRUE)
	{
		printCurrentDIR();
		scanf("%s", command);
		uppercase(command);
		if (strcmp(command, "DIR") == 0)
		{
			displayDIR();
		}
		else if (strcmp(command, "DEL") == 0)
		{
			char path[50];
			scanf("%s", path);
			uppercase(path);
			deleteFile(path);
		}
		else if (strcmp(command, "CD") == 0)
		{
			char path[50];
			scanf("%s", path);
			uppercase(path);
			changeDIR(path);
		}
		else if (strcmp(command, "TYPE") == 0)
		{
			char fileName[50];
			scanf("%s", fileName);
			typeFile(fileName);
		}
		else if (strcmp(command, "COPY") == 0)
		{
			char para1[50], para2[50];
			char file1[50], file2[50];
			scanf("%s", para1);
			scanf("%s", para2);
			uppercase(para1);
			uppercase(para2);
			if (strcmp(para1, "CON") == 0)
			{
				editFile(para2);
			}
			else if (dependMerge(para1, file1, file2))
			{
				mergeFile(file1, file2, para2);
			}
			else
			{
				copyFile(para1, para2);
			}
		}
		else if (strcmp(command, "EXIT") == 0)
		{
			break;
		}
		else if (strcmp(command, "MKDIR") == 0)
		{
			char newDIR[50];
			scanf("%s", newDIR);
			createDIR(newDIR);
		}
		else if (strcmp(command, "RMDIR") == 0)
		{
			char dir[50];
			scanf("%s", dir);
			deleteDIR(dir);
		}
		else if (strcmp(command, "TREE") == 0)
		{
			tree(0);
		}
		else if (strcmp(command, "CHECK") == 0)
		{
			if (checkFAT() == 1)
			{
				printf("OK\n");
			}
			else
			{
				printf("Error\n");
			}
		}
		else
		{
			printf("Bad command or file name\n");
		}
	}
	closeFAT("dossys.img");
}

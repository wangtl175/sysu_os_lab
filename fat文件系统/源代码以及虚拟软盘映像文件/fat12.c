#include"fat12.h"
#include<malloc.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
//BLOCK FAT[BLOCK_NUM];

unsigned int CURRENT_DIR_BLOCK = ROOT_DIR_BLOCK;

BYTE fdd144[BLOCK_SIZE * BLOCK_NUM];

BYTE cache[BLOCK_SIZE];

void openFAT(char *filename)
{
	FILE *fp = NULL;
	fp = fopen(filename, "rb");
	fread(fdd144, sizeof(BYTE), BLOCK_SIZE * BLOCK_NUM, fp);
	fclose(fp);
}

void closeFAT(char *filename)
{
	FILE* fp = NULL;
	fp = fopen(filename, "wb");
	fwrite(fdd144, sizeof(BYTE), BLOCK_SIZE * BLOCK_NUM, fp);
	fclose(fp);
}

void readBlock(unsigned int Blockid)
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		cache[i] = fdd144[Blockid * BLOCK_SIZE + i];
	}
}

void writeBlock(unsigned int Blockid)
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		fdd144[Blockid * BLOCK_SIZE + i] = cache[i];
	}
}

void changeDIR(char *path)
{
	int result;
	BYTE DIR_WrtTime[2], DIR_WrtDate[2], DIR_FstClus[2], DIR_FileSize[4];
	unsigned int DIR_Blockid = CURRENT_DIR_BLOCK, Itermid;
	if ((result = processPath(path, &DIR_Blockid, &DIR_WrtTime, DIR_WrtDate, DIR_FstClus, DIR_FileSize, &Itermid)) == 1)
	{
		CURRENT_DIR_BLOCK = hex2dec(DIR_FstClus, 2) + 31;
		if (CURRENT_DIR_BLOCK == 31)
		{
			CURRENT_DIR_BLOCK = ROOT_DIR_BLOCK;
		}
	}
	else if (result == 2)
	{
		CURRENT_DIR_BLOCK = ROOT_DIR_BLOCK;
	}
	else
	{
		printf("not found\n");
	}
}

void displayDIR()
{
	int filenum = 0;
	BYTE DIR_Name[12], DIR_Attr, DIR_WrtTime[2], DIR_WrtDate[2], DIR_FileSize[4];
	unsigned int nextBlock = CURRENT_DIR_BLOCK;
	do
	{
		readBlock(nextBlock);
		for (int i = 0; i < DIR_ITERM_NUM; i++)
		{
			DIR_Attr = cache[i * DIR_ITERM_SIZE + 11];
			if ((DIR_Attr == 0x10 || DIR_Attr == 0x20) && cache[i * DIR_ITERM_SIZE] != 0xE5)
			{
				filenum++;
				for (int j = 0; j < 11; j++)
				{
					DIR_Name[j] = cache[i * DIR_ITERM_SIZE + j];
				}
				DIR_Name[11] = '\0';
				for (int j = 0; j < 2; j++)
				{
					DIR_WrtTime[j] = cache[i * DIR_ITERM_SIZE + 0x17 - j];
					DIR_WrtDate[j] = cache[i * DIR_ITERM_SIZE + 0x19 - j];
				}
				DIR_FileSize[0] = cache[i * DIR_ITERM_SIZE + 0x1F];
				DIR_FileSize[1] = cache[i * DIR_ITERM_SIZE + 0x1E];
				DIR_FileSize[2] = cache[i * DIR_ITERM_SIZE + 0x1D];
				DIR_FileSize[3] = cache[i * DIR_ITERM_SIZE + 0x1C];
				unsigned int dirFileSize = hex2dec(DIR_FileSize, 4);
				/*打印信息*/
				printf("%s  ", DIR_Name);
				if (DIR_Attr == 0x20)
				{
					printf("%15u ", dirFileSize);
				}
				else
				{
					printf("%-15s ", "<DIR>");
				}
				displayTime(DIR_WrtTime, DIR_WrtDate);
				printf("\n");
			}
		}
		nextBlock = getNextBlock(nextBlock);
	} while (nextBlock != 0);
}

int deleteFile(char *path)
{
	int result;
	BYTE DIR_Attr = 0x20, DIR_WrtTime[2], DIR_WrtDate[2], DIR_FstClus[2], DIR_FileSize[4];
	unsigned int DIR_Blockid = CURRENT_DIR_BLOCK, Itermid;
	if ((result = processPath(path, &DIR_Blockid, &DIR_WrtTime, DIR_WrtDate, DIR_FstClus, DIR_FileSize, &Itermid)) == 1)
	{
		readBlock(DIR_Blockid);
		if (cache[Itermid * DIR_ITERM_SIZE + 0x0B] == 0x10)
		{
			printf("can not delete a directory\n");
			return 0;
		}
		readBlock(DIR_Blockid);
		cache[Itermid * DIR_ITERM_SIZE] = 0xE5;
		writeBlock(DIR_Blockid);
		/*修改FAT1、2*/
		unsigned int Block = hex2dec(DIR_FstClus, 2) + 31;
		do
		{
			unsigned int tempBlock = getNextBlock(Block);
			modifyFat(Block - 31, 0x000);
			Block = tempBlock;
		} while (Block != 0);
	}
	else if (result == 2)
	{
		printf("can not delete a directory\n");
	}
	else
	{
		printf("not found\n");
		return 0;
	}
	return 1;
}

unsigned int hex2dec(BYTE *hex, int numOfByte)
{
	unsigned int dec = hex[0];
	for (int i = 1; i < numOfByte; i++)
	{
		dec = dec << 8;
		dec += hex[i];
	}
	return dec;
}

unsigned int getNextBlock(unsigned int Blockid)
{
	if (Blockid >= ROOT_DIR_BLOCK && Blockid < ROOT_DIR_BLOCK + 14)
	{
		if (Blockid == ROOT_DIR_BLOCK + 13)
		{
			return 0;
		}
		return Blockid + 1;
	}
	UINT16 nowClus = Blockid - 31;
	UINT16 nextClus = consultFat(nowClus);
	if (nextClus == 0xFFF)
	{
		return 0;
	}
	else
	{
		return nextClus + 31;
	}
}

void uppercase(char *text)
{
	int len = strlen(text), i;
	for (i = 0; i < len; i++)
	{
		if (text[i] >= 'a' && text[i] <= 'z')
		{
			text[i] = text[i] & 0xDF;
		}
	}
}

void processFileName(char *fileName, BYTE DIR_Attr)
{
	uppercase(fileName);
	int size = strlen(fileName);
	if (DIR_Attr == 0x20)
	{
		for (int i = size; i < 11; i++)
		{
			fileName[i] = ' ';
		}
		for (int i = 0; i < size; i++)
		{
			if (fileName[i] == '.')
			{
				for (int j = 0; j < 3; j++)
				{
					fileName[10 - j] = fileName[size - 1 - j];
				}
				for (; i < 8; i++)
				{
					fileName[i] = ' ';
				}
				break;
			}
		}
	}
	else if (DIR_Attr == 0x10)
	{
		for (int i = size; i < 11; i++)
		{
			fileName[i] = ' ';
		}
	}
	fileName[11] = '\0';
}

void typeFile(char *path)
{
	BYTE DIR_Attr = 0x20, DIR_WrtTime[2], DIR_WrtDate[2], DIR_FstClus[2], DIR_FileSize[4];
	unsigned int DIR_Blockid = CURRENT_DIR_BLOCK, Itermid;
	if (processPath(path, &DIR_Blockid, &DIR_WrtTime, DIR_WrtDate, DIR_FstClus, DIR_FileSize, &Itermid))
	{
		unsigned int nextBlock = hex2dec(DIR_FstClus, 2) + 31;
		unsigned int fileSize = hex2dec(DIR_FileSize, 4), i = 0;
		do
		{
			readBlock(nextBlock);
			for (int offset = 0; offset < BLOCK_SIZE && i < fileSize; i++, offset++)
			{
				if (cache[offset] == 0x1A)
				{
					continue;
				}
				printf("%c", cache[offset]);
			}
			nextBlock = getNextBlock(nextBlock);
		} while (nextBlock != 0 && i != fileSize);
	}
	else
	{
		printf("not found\n");
	}
}

int Find_Dir_Iterm(unsigned int *DIR_Blockid, BYTE *DIR_Name, BYTE DIR_Attr, BYTE *DIR_WrtTime, BYTE *DIR_WrtDate, BYTE *DIR_FstClus, BYTE *DIR_FileSize, unsigned int *Itermid)
{
	unsigned int nextBlock = *DIR_Blockid;
	do
	{
		readBlock(nextBlock);
		for (unsigned int i = 0; i < DIR_ITERM_NUM; i++)
		{
			if ((cache[i * DIR_ITERM_SIZE + 0x0B] == DIR_Attr) && cache[i * DIR_ITERM_SIZE] != 0xE5)
			{
				int j = 0;
				for (; j < 11; j++)
				{
					if (cache[i * DIR_ITERM_SIZE + j] != DIR_Name[j])
					{
						break;
					}
				}
				if (j == 11)
				{
					DIR_WrtTime[0] = cache[i * DIR_ITERM_SIZE + 0x17];
					DIR_WrtTime[1] = cache[i * DIR_ITERM_SIZE + 0x16];
					DIR_WrtDate[0] = cache[i * DIR_ITERM_SIZE + 0x19];
					DIR_WrtDate[1] = cache[i * DIR_ITERM_SIZE + 0x18];
					DIR_FstClus[0] = cache[i * DIR_ITERM_SIZE + 0x1B];
					DIR_FstClus[1] = cache[i * DIR_ITERM_SIZE + 0x1A];
					DIR_FileSize[0] = cache[i * DIR_ITERM_SIZE + 0x1F];
					DIR_FileSize[1] = cache[i * DIR_ITERM_SIZE + 0x1E];
					DIR_FileSize[2] = cache[i * DIR_ITERM_SIZE + 0x1D];
					DIR_FileSize[3] = cache[i * DIR_ITERM_SIZE + 0x1C];
					*DIR_Blockid = nextBlock;
					*Itermid = i;
					return 1;
				}
			}
		}
		nextBlock = getNextBlock(nextBlock);
	} while (nextBlock != 0);
	return 0;
}

void copyFile(char *source, char *dest)
{
	BYTE DIR_Attr = 0x20, Source_DIR_WrtTime[2], Source_DIR_WrtDate[2], Source_DIR_FstClus[2], Source_DIR_FileSize[4];
	unsigned int Source_DIR_Blockid = CURRENT_DIR_BLOCK, Source_Itermid, Source_Block;
	BYTE Dest_DIR_WrtTime[2], Dest_DIR_WrtDate[2], Dest_DIR_FstClus[2], Dest_DIR_FileSize[4];
	unsigned int Dest_DIR_Blockid = CURRENT_DIR_BLOCK, Dest_Itermid, Dest_Block;
	if (processPath(source, &Source_DIR_Blockid, Source_DIR_WrtTime, Source_DIR_WrtDate, Source_DIR_FstClus, Source_DIR_FileSize, &Source_Itermid))
	{
		readBlock(Source_DIR_Blockid);
		if (cache[Source_Itermid * DIR_ITERM_SIZE + 0x0B] != DIR_Attr)
		{
			printf("can not copy a directory\n");
		}
		if (processPath(dest, &Dest_DIR_Blockid, Dest_DIR_WrtTime, Dest_DIR_WrtDate, Dest_DIR_FstClus, Dest_DIR_FileSize, &Dest_Itermid))
		{
			
			readBlock(Dest_DIR_Blockid);
			if (cache[Dest_Itermid * DIR_ITERM_SIZE + 0x0B] == DIR_Attr)
			{
				writeTime(Dest_DIR_WrtTime, Dest_DIR_WrtDate);
				cache[Dest_Itermid * DIR_ITERM_SIZE + 0x1C] = Source_DIR_FileSize[3];
				cache[Dest_Itermid * DIR_ITERM_SIZE + 0x1D] = Source_DIR_FileSize[2];
				cache[Dest_Itermid * DIR_ITERM_SIZE + 0x1E] = Source_DIR_FileSize[1];
				cache[Dest_Itermid * DIR_ITERM_SIZE + 0x1F] = Source_DIR_FileSize[0];
				cache[Dest_Itermid * DIR_ITERM_SIZE + 0x16] = Dest_DIR_WrtTime[0];
				cache[Dest_Itermid * DIR_ITERM_SIZE + 0x17] = Dest_DIR_WrtTime[1];
				cache[Dest_Itermid * DIR_ITERM_SIZE + 0x18] = Dest_DIR_WrtDate[0];
				cache[Dest_Itermid * DIR_ITERM_SIZE + 0x19] = Dest_DIR_WrtDate[1];
				writeBlock(Dest_DIR_Blockid);
				Source_Block = hex2dec(Source_DIR_FstClus, 2) + 31;
				Dest_Block = hex2dec(Dest_DIR_FstClus, 2) + 31;
				do
				{
					readBlock(Source_Block);

					if (Dest_Block == 0)
					{
						unsigned int tempBlock = appendBlock(Dest_Block);
						Dest_Block = tempBlock;
					}

					writeBlock(Dest_Block);
					Source_Block = getNextBlock(Source_Block);
					Dest_Block = getNextBlock(Dest_Block);
				} while (Source_Block != 0);
				unsigned int tempBlock = getNextBlock(Dest_Block);
				modifyFat(Dest_Block - 31, 0xFFF);
				Dest_Block = tempBlock;
				while (Dest_Block != 0)
				{
					tempBlock = getNextBlock(Dest_Block);
					modifyFat(Dest_Block, 0x000);
					Dest_Block = tempBlock;
				}
			}
			else
			{
				printf("can not copy to a directory\n");
			}
			return;
		}
		createFile(&Dest_DIR_Blockid, &Dest_Itermid, dest, DIR_Attr, hex2dec(Source_DIR_FileSize, 4), &Dest_Block);
		Source_Block = hex2dec(Source_DIR_FstClus, 2) + 31;
		do
		{
			readBlock(Source_Block);
			writeBlock(Dest_Block);
			Source_Block = getNextBlock(Source_Block);
			Dest_Block = getNextBlock(Dest_Block);
		} while (Source_Block != 0);
	}
	else
	{
		printf("not found\n");
	}
}

void createFile(unsigned int *DIR_Block, unsigned int *Itermid, char *fileName, BYTE DIR_Attr, unsigned int size, unsigned int *Blockid)
{
	processFileName(fileName, DIR_Attr);
	unsigned int tempBlock;
	
	*Itermid = findEmptyIterm(DIR_Block);

	*Blockid = findEmptyBlock();
	modifyFat(*Blockid - 31, 0x0FFF);

	readBlock(*DIR_Block);


	for (int i = 0; i < 11; i++)
	{
		cache[*Itermid * DIR_ITERM_SIZE + i] = fileName[i];
	}
	cache[*Itermid * DIR_ITERM_SIZE + 0x0B] = DIR_Attr;
	cache[*Itermid * DIR_ITERM_SIZE + 0x1A] = (*Blockid - 31) & 0x00FF;
	cache[*Itermid * DIR_ITERM_SIZE + 0x1B] = ((*Blockid - 31) & 0x0F00) >> 8;
	BYTE DIR_WrtTime[2], DIR_WrtDate[2];
	writeTime(DIR_WrtTime, DIR_WrtDate);
	cache[*Itermid * DIR_ITERM_SIZE + 0x16] = DIR_WrtTime[0];
	cache[*Itermid * DIR_ITERM_SIZE + 0x17] = DIR_WrtTime[1];
	cache[*Itermid * DIR_ITERM_SIZE + 0x18] = DIR_WrtDate[0];
	cache[*Itermid * DIR_ITERM_SIZE + 0x19] = DIR_WrtDate[1];


	if (DIR_Attr == 0x20)//普通文件才需要大小
	{
		cache[*Itermid * DIR_ITERM_SIZE + 0x1C] = size & 0xFF;
		cache[*Itermid * DIR_ITERM_SIZE + 0x1D] = (size & 0xFF00) >> 8;
		cache[*Itermid * DIR_ITERM_SIZE + 0x1E] = (size & 0xFF0000) >> 16;
		cache[*Itermid * DIR_ITERM_SIZE + 0x1F] = (size & 0xFF000000) >> 24;
	}
	writeBlock(*DIR_Block);
	if (DIR_Attr == 0x10)
	{
		formatDIR(*Blockid);
		readBlock(*Blockid);

		cache[0] = '.';
		cache[1] = ' ';
		cache[DIR_ITERM_SIZE] = '.';
		cache[DIR_ITERM_SIZE + 1] = '.';
		for (int j = 2; j < 11; j++)
		{
			cache[j] = ' ';
			cache[DIR_ITERM_SIZE + j] = ' ';
		}
		cache[0x16] = DIR_WrtTime[0];
		cache[0x17] = DIR_WrtTime[1];
		cache[0x18] = DIR_WrtDate[0];
		cache[0x19] = DIR_WrtDate[1];

		cache[DIR_ITERM_SIZE + 0x16] = DIR_WrtTime[0];
		cache[DIR_ITERM_SIZE + 0x17] = DIR_WrtTime[1];
		cache[DIR_ITERM_SIZE + 0x18] = DIR_WrtDate[0];
		cache[DIR_ITERM_SIZE + 0x19] = DIR_WrtDate[1];


		cache[0x0B] = 0x10;
		cache[DIR_ITERM_SIZE + 0x0B] = 0x10;
		cache[0x1A] = (*Blockid - 31) & 0xFF;
		cache[0x1B] = ((*Blockid - 31) & 0xFF00) >> 8;

		cache[DIR_ITERM_SIZE + 0x1A] = (*DIR_Block - 31) & 0xFF;
		cache[DIR_ITERM_SIZE + 0x1B] = ((*DIR_Block - 31) & 0xFF00) >> 8;
		writeBlock(*Blockid);
	}
	else if (DIR_Attr == 0x20)
	{
		unsigned int tblock = *Blockid, tsize = 512;
		for (; tsize < size; tsize += 512)
		{
			tblock = appendBlock(tblock);
		}
	}
}

unsigned int findEmptyBlock()
{
	UINT16 clus;
	for (int i = 3; i < BLOCK_SIZE * 9; i += 3)
	{
		clus = fdd144[512 + i];
		clus += ((fdd144[513 + i] & 0x0F) << 8);
		if (clus == 0)
		{
			return 2 * i / 3 + 31;
		}
		clus = (fdd144[513 + i] & 0xF0) >> 4;
		clus += ((fdd144[514 + i]) << 4);
		if (clus == 0)
		{
			return 2 * i / 3 + 32;
		}
	}
	return 0;
}

UINT16 consultFat(UINT16 clus)
{
	UINT16 nextClus = 0;
	unsigned int offset = clus / 2 * 3;
	unsigned int BlockOffset = offset / 512;
	offset %= 512;

	if (clus % 2 == 0)
	{
		readBlock(1 + BlockOffset);
		nextClus += cache[offset];
		nextClus += ((cache[offset + 1] & 0x0F) << 8);
	}
	else
	{
		readBlock(1 + BlockOffset);
		nextClus += ((cache[offset + 1] & 0xF0) >> 4);
		nextClus += (cache[offset + 2] << 4);
	}
	return nextClus;
}

void modifyFat(UINT16 clus, UINT16 value)
{
	unsigned int offset = clus / 2 * 3;
	unsigned int temp1, temp2;
	unsigned int BlockOffset = offset / 512;
	offset %= 512;
	if (clus % 2 == 0)
	{
		readBlock(1 + BlockOffset);
		cache[offset] = value & 0x00FF;
		temp1 = cache[offset];
		cache[1 + offset] = ((value & 0x0F00) >> 8) + (cache[1 + offset] & 0xF0);
		temp2 = cache[1 + offset];
		writeBlock(1 + BlockOffset);
		readBlock(10 + BlockOffset);
		cache[offset] = temp1;
		cache[1 + offset] = temp2;
		writeBlock(10 + BlockOffset);
	}
	else
	{
		readBlock(1 + BlockOffset);
		cache[1 + offset] = (cache[1 + offset] & 0x0F) + ((value & 0x000F) << 4);
		temp1 = cache[1 + offset];
		cache[2 + offset] = (value & 0x0FF0) >> 4;
		temp2 = cache[2 + offset];
		writeBlock(1 + BlockOffset);
		readBlock(10 + BlockOffset);
		cache[1 + offset] = temp1;
		cache[2 + offset] = temp2;
		writeBlock(10 + BlockOffset);
	}
}

unsigned int findEmptyIterm(unsigned int *Block)
{
	unsigned int temp = *Block;
	while (1)
	{
		readBlock(temp);
		for (int i = 0; i < DIR_ITERM_NUM; i++)
		{
			if (cache[i * DIR_ITERM_SIZE] == 0 || cache[i * DIR_ITERM_SIZE] == 0xE5)
			{
				*Block = temp;
				return i;
			}
		}
		unsigned int ttemp = getNextBlock(temp);
		if (ttemp == 0)
		{
			temp = appendBlock(temp);
			formatDIR(temp);
		}
		else
		{
			temp = ttemp;
		}
	}
	return 0;
}

void formatDIR(unsigned int Blockid)
{
	readBlock(Blockid);
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		cache[i] = 0;
	}
	writeBlock(Blockid);
}

unsigned int appendBlock(unsigned int Blockid)
{
	unsigned int newBlock = findEmptyBlock();
	modifyFat(newBlock - 31, 0x0FFF);
	modifyFat(Blockid - 31, newBlock - 31);
	return newBlock;
}

void editFile(char *fileName)
{
	BYTE DIR_Attr = 0x20, DIR_WrtTime[2], DIR_WrtDate[2], DIR_FstClus[2], DIR_FileSize[4];
	unsigned int DIR_Blockid = CURRENT_DIR_BLOCK, Itermid, inputBlock;
	unsigned int size = 0, offset = 0;
	char inputCh;
	if (processPath(fileName, &DIR_Blockid, DIR_WrtTime, DIR_WrtDate, DIR_FstClus, DIR_FileSize, &Itermid))
	{
		readBlock(DIR_Blockid);
		if (cache[Itermid * DIR_ITERM_SIZE + 0x0B] != DIR_Attr)
		{
			printf("can not edit a directory\n");
			return;
		}
		
		inputBlock = hex2dec(DIR_FstClus, 2) + 31;
		printf("overwrite?(y/n): ");
		CLEAR_INPUT_BUF
		char confirm = getchar();
		CLEAR_INPUT_BUF
		if (confirm == 'y' || confirm == 'Y')
		{
			while ((inputCh = getchar()) != EOF)
			{
				if (inputCh == '\n')
				{
					cache[offset] = 0x0D;
					offset += 1;
					size += 1;
					if (offset == 512)
					{
						offset = 0;
						writeBlock(inputBlock);
						inputBlock = appendBlock(inputBlock);
					}
					cache[offset] = 0x0A;
					offset += 1;
					size += 1;
					if (offset == 512)
					{
						offset = 0;
						writeBlock(inputBlock);
						inputBlock = appendBlock(inputBlock);
					}
					continue;
				}


				cache[offset] = inputCh;
				offset += 1;
				size += 1;
				if (offset == 512)
				{
					offset = 0;
					writeBlock(inputBlock);
					unsigned int tempBlock = getNextBlock(inputBlock);
					if (tempBlock == 0)
					{
						inputBlock = appendBlock(inputBlock);
					}
					else
					{
						inputBlock = tempBlock;
					}
				}
			}
			writeBlock(inputBlock);
			readBlock(DIR_Blockid);
			cache[Itermid * DIR_ITERM_SIZE + 0x1C] = (size & 0xFF);
			cache[Itermid * DIR_ITERM_SIZE + 0x1D] = (size & 0xFF00) >> 8;
			cache[Itermid * DIR_ITERM_SIZE + 0x1E] = (size & 0xFF0000) >> 16;
			cache[Itermid * DIR_ITERM_SIZE + 0x1F] = (size & 0xFF000000) >> 24;
			writeTime(DIR_WrtTime, DIR_WrtDate);
			cache[Itermid * DIR_ITERM_SIZE + 0x16] = DIR_WrtTime[0];
			cache[Itermid * DIR_ITERM_SIZE + 0x17] = DIR_WrtTime[1];
			cache[Itermid * DIR_ITERM_SIZE + 0x18] = DIR_WrtDate[0];
			cache[Itermid * DIR_ITERM_SIZE + 0x19] = DIR_WrtDate[1];

			writeBlock(DIR_Blockid);
			unsigned int tempBlock = getNextBlock(inputBlock);
			modifyFat(inputBlock - 31, 0x0FFF);
			inputBlock = tempBlock;
			while (inputBlock != 0)
			{
				tempBlock = getNextBlock(inputBlock);
				modifyFat(inputBlock - 31, 0x0000);
				inputBlock = tempBlock;
			}
		}
		else if (confirm == 'n' || confirm == 'N')
		{
			return;
		}
		else
		{
			printf("bad command!\n");
		}
	}
	else
	{
		//先创建一个512大小的，不够再加
		createFile(&DIR_Blockid, &Itermid, fileName, DIR_Attr, BLOCK_SIZE, &inputBlock);
		CLEAR_INPUT_BUF
		while ((inputCh = getchar()) != EOF)
		{
			if (inputCh == '\n')
			{
				cache[offset] = 0x0D;
				offset += 1;
				size += 1;
				if (offset == 512)
				{
					offset = 0;
					writeBlock(inputBlock);
					inputBlock = appendBlock(inputBlock);
				}
				cache[offset] = 0x0A;
				offset += 1;
				size += 1;
				if (offset == 512)
				{
					offset = 0;
					writeBlock(inputBlock);
					inputBlock = appendBlock(inputBlock);
				}
				continue;
			}

			cache[offset] = inputCh;
			offset += 1;
			size += 1;
			if (offset == 512)
			{
				offset = 0;
				writeBlock(inputBlock);
				inputBlock = appendBlock(inputBlock);
			}
		}
		writeBlock(inputBlock);

		readBlock(DIR_Blockid);
		cache[Itermid * DIR_ITERM_SIZE + 0x1C] = (size & 0xFF);
		cache[Itermid * DIR_ITERM_SIZE + 0x1D] = (size & 0xFF00) >> 8;
		cache[Itermid * DIR_ITERM_SIZE + 0x1E] = (size & 0xFF0000) >> 16;
		cache[Itermid * DIR_ITERM_SIZE + 0x1F] = (size & 0xFF000000) >> 24;
		writeBlock(DIR_Blockid);
	}
}

void createDIR(char *dirName)
{
	BYTE DIR_Attr = 0x10, DIR_WrtTime[2], DIR_WrtDate[2], DIR_FstClus[2], DIR_FileSize[4];
	unsigned int DIR_Blockid = CURRENT_DIR_BLOCK, Itermid;
	if (processPath(dirName, &DIR_Blockid, DIR_WrtTime, DIR_WrtDate, DIR_FstClus, DIR_FileSize, &Itermid))
	{
		printf("exited");
	}
	else
	{
		unsigned int tempBlock;
		createFile(&DIR_Blockid, &Itermid, dirName, DIR_Attr, 512, &tempBlock);
	}
}

int deleteDIR(char *dirName)
{
	//下面会改变CURRENT_DIR_BLOCK，所以要在这里记录以下，最后可以返回
	int result;
	unsigned int tcurrent = CURRENT_DIR_BLOCK;
	BYTE DIR_Attr = 0x10, DIR_WrtTime[2], DIR_WrtDate[2], DIR_FstClus[2], DIR_FileSize[4];
	unsigned int DIR_Blockid = CURRENT_DIR_BLOCK, Itermid;
	result = processPath(dirName, &DIR_Blockid, DIR_WrtTime, DIR_WrtDate, DIR_FstClus, DIR_FileSize, &Itermid);
	if (result == 2)
	{
		printf("are you sure to delete the root directory which will result in serious problem?\n");
		char ch;
		CLEAR_INPUT_BUF
		ch = getchar();
		CLEAR_INPUT_BUF
		if (ch == 'n' || ch == 'N')
		{
			return;
		}
		DIR_Blockid = ROOT_DIR_BLOCK;
	}
	if (result)
	{
		readBlock(DIR_Blockid);

		if (cache[Itermid * DIR_ITERM_SIZE + 0x0B] != DIR_Attr)
		{
			printf("can not delete a file\n");
			return 0;
		}

		cache[Itermid * DIR_ITERM_SIZE] = 0xE5;
		writeBlock(DIR_Blockid);
		unsigned int Block = hex2dec(DIR_FstClus, 2) + 31;
		readBlock(Block);
		for (int i = 2; i < DIR_ITERM_NUM; i++)
		{
			if (cache[i * DIR_ITERM_SIZE] != 0xE5 && cache[i * DIR_ITERM_SIZE] != 0)
			{
				CURRENT_DIR_BLOCK = Block;
				char fileName[12];
				for (int j = 0; j < 11; j++)
				{
					fileName[j] = cache[i * DIR_ITERM_SIZE + j];
				}
				fileName[11] = '\0';
				if (cache[i * DIR_ITERM_SIZE + 0x0B] == 0x20)
				{
					deleteFile(fileName);
				}
				else if (cache[i * DIR_ITERM_SIZE + 0x0B] == 0x10)
				{
					deleteDIR(fileName);
				}
				//上面的递归删除之后，会改变当前cache里的内容，所以要重新读
				readBlock(Block);
				cache[i * DIR_ITERM_SIZE] = 0xE5;
				writeBlock(Block);
			}
		}
		//如果目录文件不只一个块，还要继续删除其他块的内容，与删除第一块有些不同，其他块要从第0项开始删除
		while (1)
		{
			unsigned int tempBlock = getNextBlock(Block);
			modifyFat(Block - 31, 0x0000);
			Block = tempBlock;
			if (Block == 0)
			{
				break;
			}
			else
			{
				readBlock(Block);
				for (int i = 0; i < DIR_ITERM_NUM; i++)
				{
					CURRENT_DIR_BLOCK = Block;
					if (cache[i * DIR_ITERM_SIZE] != 0xE5 && cache[i * DIR_ITERM_SIZE] != 0)
					{
						char fileName[11];
						for (int j = 0; j < 11; j++)
						{
							fileName[j] = cache[i * DIR_ITERM_SIZE + j];
						}
						if (cache[i * DIR_ITERM_SIZE + 0x0B] == 0x20)
						{
							deleteFile(fileName);
						}
						else if (cache[i * DIR_ITERM_SIZE + 0x0B] == 0x10)
						{
							deleteDIR(fileName);
						}
						//上面的递归删除之后，会改变当前cache里的内容，所以要重新读
						readBlock(Block);
						cache[i * DIR_ITERM_SIZE] = 0xE5;
						writeBlock(Block);
					}
				}
			}
		}
	}
	else
	{
		printf("not found");
	}
	CURRENT_DIR_BLOCK = tcurrent;
	return 1;
}

int dependMerge(char *cmd, char *file1, char *file2)
{
	int size = strlen(cmd);
	for (int i = 0; i < size; i++)
	{
		if (cmd[i] == '+')
		{
			file1[i] = '\0';
			for (int j = i + 1; j < size; j++)
			{
				file2[j - i - 1] = cmd[j];
			}
			file2[size - 1 - i] = '\0';
			return 1;
		}
		file1[i] = cmd[i];
	}
	return 0;
}

void mergeFile(char *file1, char *file2, char *merge)
{
	BYTE DIR_Attr = 0x20, File1_DIR_WrtTime[2], File1_DIR_WrtDate[2], File1_DIR_FstClus[2], File1_DIR_FileSize[4];
	unsigned int File1_DIR_Blockid = CURRENT_DIR_BLOCK, File1_Itermid;
	BYTE File2_DIR_WrtTime[2], File2_DIR_WrtDate[2], File2_DIR_FstClus[2], File2_DIR_FileSize[4];
	unsigned int File2_DIR_Blockid = CURRENT_DIR_BLOCK, File2_Itermid;
	BYTE Merge_DIR_WrtTime[2], Merge_DIR_WrtDate[2], Merge_DIR_FstClus[2], Merge_DIR_FileSize[4];
	unsigned int Merge_DIR_Blockid = CURRENT_DIR_BLOCK, Merge_DIR_Itermid;
	unsigned int mergeSize, file1Size, file2Size, mergeBlock, file1Block, file2Block;
	if (processPath(file1, &File1_DIR_Blockid, File1_DIR_WrtTime, File1_DIR_WrtDate, File1_DIR_FstClus, File1_DIR_FileSize, &File1_Itermid)
		&& processPath(file2, &File2_DIR_Blockid, File2_DIR_WrtTime, File2_DIR_WrtDate, File2_DIR_FstClus, File2_DIR_FileSize, &File2_Itermid))
	{
		file1Block = hex2dec(File1_DIR_FstClus, 2) + 31;
		file2Block = hex2dec(File2_DIR_FstClus, 2) + 31;
		file1Size = hex2dec(File1_DIR_FileSize, 4);
		file2Size= hex2dec(File2_DIR_FileSize, 4);
		mergeSize = file1Size + file2Size;

		readBlock(File1_DIR_Blockid);
		if (cache[File1_Itermid * DIR_ITERM_SIZE + 0x0B] != DIR_Attr)
		{
			printf("can not merge a directory\n");
			return;
		}
		readBlock(File2_DIR_Blockid);
		if (cache[File2_Itermid * DIR_ITERM_SIZE + 0x0B] != DIR_Attr)
		{
			printf("can not merge a directory\n");
			return;
		}

		if (processPath(merge, &Merge_DIR_Blockid, Merge_DIR_WrtTime, Merge_DIR_WrtDate, Merge_DIR_FstClus, Merge_DIR_FileSize, &Merge_DIR_Itermid))
		{
			readBlock(Merge_DIR_Blockid);
			if (cache[Merge_DIR_Itermid * DIR_ITERM_SIZE + 0x0B] != DIR_Attr)
			{
				printf("can not merge to a directory\n");
				return;
			}
			cache[Merge_DIR_Itermid * DIR_ITERM_SIZE + 0x1C] = mergeSize & 0xFF;
			cache[Merge_DIR_Itermid * DIR_ITERM_SIZE + 0x1D] = (mergeSize & 0xFF00) >> 8;
			cache[Merge_DIR_Itermid * DIR_ITERM_SIZE + 0x1E] = (mergeSize & 0xFF0000) >> 16;
			cache[Merge_DIR_Itermid * DIR_ITERM_SIZE + 0x1F] = (mergeSize & 0xFF000000) >> 24;
			cache[Merge_DIR_Itermid * DIR_ITERM_SIZE + 0x16] = Merge_DIR_WrtTime[0];
			cache[Merge_DIR_Itermid * DIR_ITERM_SIZE + 0x17] = Merge_DIR_WrtTime[1];
			cache[Merge_DIR_Itermid * DIR_ITERM_SIZE + 0x18] = Merge_DIR_WrtDate[0];
			cache[Merge_DIR_Itermid * DIR_ITERM_SIZE + 0x19] = Merge_DIR_WrtDate[1];
			writeBlock(Merge_DIR_Blockid);
			mergeBlock = hex2dec(Merge_DIR_FstClus, 2) + 31;
		}
		else
		{
			createFile(&Merge_DIR_Blockid, &Merge_DIR_Itermid, merge, DIR_Attr, file1Size + file2Size, &mergeBlock);
		}
		unsigned int tempBlock = mergeBlock;
		do
		{
			if (tempBlock == 0)
			{
				mergeBlock = appendBlock(mergeBlock);
			}
			else
			{
				mergeBlock = tempBlock;
				tempBlock = getNextBlock(mergeBlock);
			}
			readBlock(file1Block);
			writeBlock(mergeBlock);
			file1Block = getNextBlock(file1Block);
		} while (file1Block != 0);
		//把file2接过来，考虑file1Size%512==0的情况
		unsigned int file2Offset = 0, mergeOffset = file1Size % 512;
		if (mergeOffset == 0)
		{
			tempBlock = getNextBlock(mergeBlock);
			if (tempBlock == 0)
			{
				mergeBlock = appendBlock(mergeBlock);
			}
			else
			{
				mergeBlock = tempBlock;
			}
		}
		BYTE temp[BLOCK_SIZE];
		readBlock(file2Block);
		copyBlock(cache, temp);
		readBlock(mergeBlock);
		for (int i = 0; i < file2Size; i++)
		{
			cache[mergeOffset] = temp[file2Offset];
			if (file2Offset == BLOCK_SIZE - 1)
			{
				file2Offset = 0;
				file2Block = getNextBlock(file2Block);
				readBlock(file2Block);
				copyBlock(cache, temp);
				readBlock(mergeBlock);
			}
			else
			{
				file2Offset += 1;
			}
			if (mergeOffset == BLOCK_SIZE - 1)
			{
				mergeOffset = 0;
				writeBlock(mergeBlock);
				tempBlock = getNextBlock(mergeBlock);
				if (tempBlock == 0)
				{
					mergeBlock = appendBlock(mergeBlock);
				}
				else
				{
					mergeBlock = tempBlock;
				}
			}
			else
			{
				mergeOffset += 1;
			}
		}
		writeBlock(mergeBlock);
		//还要把merge的尾巴给去掉
		tempBlock = getNextBlock(mergeBlock);
		modifyFat(mergeBlock - 31, 0x0FFF);
		mergeBlock = tempBlock;
		while (mergeBlock != 0)
		{
			tempBlock = getNextBlock(mergeBlock);
			modifyFat(mergeBlock, 0);
			mergeBlock = tempBlock;
		}
	}
	else
	{
		printf("not found\n");
	}
}

void copyBlock(BYTE *source, BYTE *dest)
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		dest[i] = source[i];
	}
}

void tree(int depth)
{
	unsigned int Block = CURRENT_DIR_BLOCK;
	BYTE fileName[11];
	BYTE DIR_Attr;
	BYTE DIR_FstClus[2];
	while (Block != 0)
	{
		readBlock(Block);
		for (int i = 0; i < DIR_ITERM_NUM; i++)
		{
			if (cache[i * DIR_ITERM_SIZE] != 0xE5 &&
				((DIR_Attr = cache[i * DIR_ITERM_SIZE + 0x0B]) == 0x10 || cache[i * DIR_ITERM_SIZE + 0x0B] == 0x20)
				&& cache[i * DIR_ITERM_SIZE] != '.')
			{
				for (int j = 0; j < depth; j++)
				{
					printf("  ");
				}
				for (int j = 0; j < 11; j++)
				{
					fileName[j] = cache[i * DIR_ITERM_SIZE + j];
				}
				processDisplayFileName(fileName, DIR_Attr);
				printf("%s\n", fileName);
				if (DIR_Attr == 0x10)
				{
					DIR_FstClus[1] = cache[i * DIR_ITERM_SIZE + 0x1A];
					DIR_FstClus[0] = cache[i * DIR_ITERM_SIZE + 0x1B];
					unsigned int tempDIR = CURRENT_DIR_BLOCK;
					CURRENT_DIR_BLOCK = hex2dec(DIR_FstClus, 2) + 31;
					tree(depth + 1);
					CURRENT_DIR_BLOCK = tempDIR;
					readBlock(Block);
				}
			}
		}
		Block = getNextBlock(Block);
	}
}

void processDisplayFileName(char *fileName, BYTE DIR_Attr)
{
	for (int i = 0; i < 11; i++)
	{
		if (fileName[i] == ' ')
		{
			if (DIR_Attr == 0x10)
			{
				fileName[i] = '\0';
				return;
			}
			else
			{
				fileName[i] = '.';
				for (int j = 0; j < 3; j++)
				{
					fileName[i + 1 + j] = fileName[8 + j];
				}
				fileName[i + 4] = '\0';
			}
		}
	}
}

int processPath(char *path, unsigned int *DIR_Block, BYTE *DIR_WrtTime, BYTE *DIR_WrtDate, BYTE *DIR_FstClus, BYTE *DIR_FileSize, unsigned int *ItermID)
{
	//关键在于切割path
	int size = strlen(path), i, subFileSize;
	char subFile[12];
	BYTE DIR_Attr = 0x10;
	//绝对路径
	if (path[0] == '/')
	{
		*DIR_Block = ROOT_DIR_BLOCK;
		for (i = 0; i < size; i++)
		{
			path[i] = path[i + 1];
		}
		size -= 1;
	}
	if (size == 0)
	{
		return 2;
	}
	for (i = 0; i < 11 && path[i] != '/' && i < size; i++)
	{
		subFile[i] = path[i];
	}
	subFile[i] = '\0';
	subFileSize = i;
	int flat = 0;
	for (int j = 0; j < subFileSize; j++)
	{
		if (subFile[j] == '.' || subFile[j] == ' ')
		{
			flat = 1;
			break;
		}
	}
	if (flat && subFile[0] != '.')
	{
		DIR_Attr = 0x20;
	}
	processFileName(subFile, DIR_Attr);
	if (i == size || (i == size - 1 && path[i] == '/'))
	{
		return Find_Dir_Iterm(DIR_Block, subFile, DIR_Attr, DIR_WrtTime, DIR_WrtDate, DIR_FstClus, DIR_FileSize, ItermID);
	}
	i++;
	int t = i;
	for (; i <= size; i++)
	{
		path[i - t] = path[i];
	}
	if (subFileSize == 1 && subFile[0] == '.' && *DIR_Block == ROOT_DIR_BLOCK)
	{
		return processPath(path, DIR_Block, DIR_WrtTime, DIR_WrtDate, DIR_FstClus, DIR_FileSize, ItermID);
	}
	if (Find_Dir_Iterm(DIR_Block, subFile, DIR_Attr, DIR_WrtTime, DIR_WrtDate, DIR_FstClus, DIR_FileSize, ItermID))
	{
		*DIR_Block = hex2dec(DIR_FstClus, 2) + 31;
		if (*DIR_Block == 31)
		{
			*DIR_Block = ROOT_DIR_BLOCK;
		}
		return processPath(path, DIR_Block, DIR_WrtTime, DIR_WrtDate, DIR_FstClus, DIR_FileSize, ItermID);
	}
	else
	{
		return 0;
	}
}


void printCurrentDIR()
{
	if (CURRENT_DIR_BLOCK == ROOT_DIR_BLOCK)
	{
		printf("A:/");
		return;
	}
	char path[100];
	memset(path, ' ', 100);
	int offset = 99;
	BYTE nextBlock[2];
	readBlock(CURRENT_DIR_BLOCK);
	nextBlock[0] = cache[DIR_ITERM_SIZE + 0x1B];
	nextBlock[1] = cache[DIR_ITERM_SIZE + 0x1A];
	unsigned int temp1 = CURRENT_DIR_BLOCK;
	unsigned int temp2 = hex2dec(nextBlock, 2) + 31;
	if (temp2 == 31)
	{
		temp2 = ROOT_DIR_BLOCK;
	}
	while (1)
	{
		BYTE clus[2];
		unsigned int temp3 = temp2;
		while (1)
		{
			readBlock(temp2);
			for (int i = 0; i < DIR_ITERM_NUM; i++)
			{
				clus[0] = cache[i * DIR_ITERM_SIZE + 0x1B];
				clus[1] = cache[i * DIR_ITERM_SIZE + 0x1A];
				if (hex2dec(clus, 2) + 31 == temp1)
				{
					for (int j = 10; j >= 0; j--)
					{
						if (cache[i * DIR_ITERM_SIZE + j] != ' ')
						{
							path[offset] = cache[i * DIR_ITERM_SIZE + j];
							offset--;
						}
					}
					path[offset] = '/';
					offset--;
					goto A;
				}
			}
			temp2 = getNextBlock(temp2);
		}
	A:
		if (temp3 == ROOT_DIR_BLOCK)
		{
			break;
		}
		readBlock(temp3);
		temp1 = temp3;
		nextBlock[0] = cache[DIR_ITERM_SIZE + 0x1B];
		nextBlock[1] = cache[DIR_ITERM_SIZE + 0x1A];
		temp2 = hex2dec(nextBlock, 2) + 31;
		if (temp2 == 31)
		{
			temp2 = ROOT_DIR_BLOCK;
		}
	}
	printf("A:");
	for (int i = 0; i < 100; i++)
	{
		if (path[i] != ' ')
		{
			printf("%c", path[i]);
		}
	}
	printf(": ");
}

void displayTime(BYTE *time, BYTE *_date)
{
	unsigned int year, month, date, hour, minute, second;
	year = 1980 + (_date[0] >> 1);
	month = ((_date[0] & 0x01) << 3) + ((_date[1] & 0xE0) >> 5);
	date = _date[1] & 0x1F;

	hour = (time[0] & 0xF8) >> 3;
	minute = ((time[0] & 0x07) << 3) + ((time[1] & 0xE0) >> 5);
	second = time[1] & 0x1F;
	printf("%u-%02u-%02u  %02u:%02u", year, month, date, hour, minute);
}

void writeTime(BYTE *Time, BYTE *Date)
{
	time_t now;
	struct tm *tm_now;
	time(&now);
	tm_now = localtime(&now);
	unsigned int t1 = tm_now->tm_hour << 11 | tm_now->tm_min << 5 | tm_now->tm_sec;
	Time[1] = t1 >> 8;
	Time[0] = t1;
	unsigned int t2 = (1900 + tm_now->tm_year - 1980) << 9 | (1 + tm_now->tm_mon) << 5 | tm_now->tm_mday;
	Date[1] = t2 >> 8;
	Date[0] = t2;
}

int checkFAT()
{
	readBlock(0);
	BYTE BS_OEMName[9], BPB_BytsPerSec[2], BPB_SecPerClus, BPB_RsvdSecCnt[2], BPB_NumFATs,
		BPB_RootEntCnt[2], BPB_TotSec16[2], BPB_Media, BPB_FATSz16[2], BPB_SecPerTrk[2], BPB_NumHeads[2], BPB_HiddSec[4], BPB_TotSec32[4],
		BS_DrvNum, BS_Reserved1, BS_BootSig, BS_VoID[4], BS_VoILab[12], BS_FileSysType[9], End[2];
	for (int i = 0; i < 8; i++)
	{
		BS_OEMName[i] = cache[3 + i];
	}
	BS_OEMName[8] = '\0';
	BPB_BytsPerSec[1] = cache[11];
	BPB_BytsPerSec[0] = cache[12];
	BPB_SecPerClus = cache[13];
	BPB_RsvdSecCnt[1] = cache[14];
	BPB_RsvdSecCnt[0] = cache[15];
	BPB_NumFATs = cache[16];
	BPB_RootEntCnt[1] = cache[17];
	BPB_RootEntCnt[0] = cache[18];
	BPB_TotSec16[1] = cache[19];
	BPB_TotSec16[0] = cache[20];
	BPB_Media = cache[21];
	BPB_FATSz16[1] = cache[22];
	BPB_FATSz16[0] = cache[23];
	BPB_SecPerTrk[1] = cache[24];
	BPB_SecPerTrk[0] = cache[25];
	BPB_NumHeads[1] = cache[26];
	BPB_NumHeads[0] = cache[27];
	for (int i = 0; i < 4; i++)
	{
		BPB_HiddSec[3 - i] = cache[28 + i];
		BPB_TotSec32[3 - i] = cache[32 + i];

		BS_VoID[3 - i] = cache[39 + i];
	}
	BS_DrvNum = cache[36];
	BS_Reserved1 = cache[37];
	BS_BootSig = cache[38];
	for (int i = 0; i < 11; i++)
	{
		BS_VoILab[i] = cache[43 + i];
	}
	BS_VoILab[11] = '\0';
	for (int i = 0; i < 8; i++)
	{
		BS_FileSysType[i] = cache[54 + i];
	}
	BS_FileSysType[8] = '\0';
	End[0] = cache[510];
	End[1] = cache[511];
	
	printf("%s\n", BS_OEMName);
	if (hex2dec(BPB_BytsPerSec, 2) != 0x200)
	{
		return 0;
	}
	if (BPB_SecPerClus != 1)
	{
		return 0;
	}
	if (hex2dec(BPB_RsvdSecCnt, 2) != 1)
	{
		return 0;
	}
	if (BPB_NumFATs != 2)
	{
		return 0;
	}
	if (hex2dec(BPB_RootEntCnt, 2) != 0xE0)
	{
		return 0;
	}
	if (hex2dec(BPB_TotSec16, 2) != 0xB40)
	{
		return 0;
	}
	if (BPB_Media != 0xF0)
	{
		return 0;
	}
	if (hex2dec(BPB_FATSz16, 2) != 0x09)
	{
		return 0;
	}
	if (hex2dec(BPB_SecPerTrk, 2) != 0x12)
	{
		return 0;
	}
	if (hex2dec(BPB_NumHeads, 2) != 0x02)
	{
		return 0;
	}
	if (hex2dec(BPB_HiddSec, 4) != 0)
	{
		return 0;
	}
	if (hex2dec(BPB_TotSec32, 4) != 0)
	{
		return 0;
	}
	if (BS_Reserved1 != 0)
	{
		return 0;
	}
	if (BS_BootSig != 0x29)
	{
		return 0;
	}
	/*if (hex2dec(BS_VoID, 4) != 0)
	{
		return 0;
	}*/

	/*if (strcmp(BS_VoILab, "OrangeS0.02") != 0)
	{
		return 0;
	}*/
	printf("%s\n", BS_VoILab);
	if (strcmp(BS_FileSysType, "FAT12   ") != 0)
	{
		return 0;
	}
	printf("%s\n", BS_FileSysType);
	if (End[0] != 0x55 && End[1] != 0xAA)
	{
		return 0;
	}
	return 1;
}

#ifndef FAT12_H
#define FAT12_H

#define BLOCK_SIZE 512
#define BLOCK_NUM 2880
#define DIR_ITERM_SIZE 32
#define DIR_ITERM_NUM 16
#define MBR_BLOCK 0
#define FAT1_BLOCK 1
#define FAT2_BLOCK 2
#define ROOT_DIR_BLOCK 19
#define DATA_BLOCK 32

#define CLEAR_INPUT_BUF {char ch;while ((ch = getchar()) != EOF && ch != '\n'){;}}

typedef unsigned char BYTE;
typedef unsigned short UINT16;

void openFAT(char *filename);

void closeFAT(char *filename);

void readBlock(unsigned int Blockid);
//void readBlock(int blockid, BYTE *temp);

void writeBlock(unsigned int Blockid);
//void writeBlock(int blockid, BYTE *temp);

void changeDIR(char *path);

void displayDIR();

int deleteFile(char *path);

unsigned int hex2dec(BYTE *hex, int numOfByte);

//�����߼������Ż�ȡ��һ�������ţ�������ʱ����0
unsigned int getNextBlock(unsigned int Blockid);

//��ת���ɴ�д
void uppercase(char *text);


//��һ���ַ����ĳ�11���ֽڡ�Ŀ¼����ʽ��Ҫ��fileNameҪԤ��12���ֽ�
void processFileName(char *fileName, BYTE DIR_Attr);

void typeFile(char *fileName);

//����*DIR_BlockΪ��һ���Ŀ¼�ļ���Ѱ��Ŀ¼�����ڵ�λ��
int Find_Dir_Iterm(unsigned int *DIR_Block, BYTE *DIR_Name, BYTE DIR_Attr, BYTE *DIR_WrtTime, BYTE *DIR_WrtDate, BYTE *DIR_FstClus, BYTE *DIR_FileSize, unsigned int *Itermid);

void copyFile(char *source, char *dest);

//����ָ�����ͣ���С���ļ����׿�ΪBlockid
void createFile(unsigned int *DIR_Block, unsigned int *Itermid, char *fileName, BYTE DIR_Attr, unsigned int size, unsigned int *Blockid);

unsigned int findEmptyBlock();

//����һ���غţ���������غ���Fat���϶�Ӧ��λ�õ�����
UINT16 consultFat(UINT16 clus);

//�޸�Fat��
void modifyFat(UINT16 clus, UINT16 value);

unsigned int findEmptyIterm(unsigned int *Block);

//��ʽ��һ��Ŀ¼�飬����һ�������ȫ������
void formatDIR(unsigned int Blockid);

//��Blockid�����ٽ�һ����Block
unsigned int appendBlock(unsigned int Blockid);

void editFile(char *fileName);

void createDIR(char *dirName);

int deleteDIR(char *dirName);

//����'+'�Ƿ���cmd(��С����Ϊ25)���ж��Ƿ�ʱҪ�ϲ������Ҫ���ͷ��������ļ���������file1=file2=NULL;
int dependMerge(char *cmd, char *file1, char *file2);

void mergeFile(char *file1, char *file2, char *merge);

void copyBlock(BYTE *source, BYTE *dest);

//Ŀ¼��
void tree(int depth);

void processDisplayFileName(char *fileName, BYTE DIR_Attr);

//�������·�������·��������ǰ��DIR_BlockҪ�ȳ�ʼ��ΪCURRENT_DIR_BLOCK���ҵ�·���������һ���ļ���Ŀ¼��Ŀ¼�����ڿ�
int processPath(char *path, unsigned int *DIR_Block, BYTE *DIR_WrtTime, BYTE *DIR_WrtDate, BYTE *DIR_FstClus, BYTE *DIR_FileSize, unsigned int *ItermID);

void printCurrentDIR();

void displayTime(BYTE *time, BYTE *date);

void writeTime(BYTE *Time, BYTE *Date);

int checkFAT();

#endif // !FAT12_H
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

//根据逻辑扇区号获取下一个扇区号，当结束时返回0
unsigned int getNextBlock(unsigned int Blockid);

//都转换成大写
void uppercase(char *text);


//把一个字符串改成11个字节、目录项形式，要求fileName要预留12个字节
void processFileName(char *fileName, BYTE DIR_Attr);

void typeFile(char *fileName);

//在以*DIR_Block为第一块的目录文件里寻找目录项所在的位置
int Find_Dir_Iterm(unsigned int *DIR_Block, BYTE *DIR_Name, BYTE DIR_Attr, BYTE *DIR_WrtTime, BYTE *DIR_WrtDate, BYTE *DIR_FstClus, BYTE *DIR_FileSize, unsigned int *Itermid);

void copyFile(char *source, char *dest);

//创建指定类型，大小的文件，首块为Blockid
void createFile(unsigned int *DIR_Block, unsigned int *Itermid, char *fileName, BYTE DIR_Attr, unsigned int size, unsigned int *Blockid);

unsigned int findEmptyBlock();

//传进一个簇号，返回这个簇号在Fat表上对应的位置的内容
UINT16 consultFat(UINT16 clus);

//修改Fat表
void modifyFat(UINT16 clus, UINT16 value);

unsigned int findEmptyIterm(unsigned int *Block);

//格式化一个目录块，把这一块的内容全都清零
void formatDIR(unsigned int Blockid);

//在Blockid后面再接一个新Block
unsigned int appendBlock(unsigned int Blockid);

void editFile(char *fileName);

void createDIR(char *dirName);

int deleteDIR(char *dirName);

//根据'+'是否在cmd(大小至少为25)里判断是否时要合并，如果要，就返回两个文件名，否则file1=file2=NULL;
int dependMerge(char *cmd, char *file1, char *file2);

void mergeFile(char *file1, char *file2, char *merge);

void copyBlock(BYTE *source, BYTE *dest);

//目录树
void tree(int depth);

void processDisplayFileName(char *fileName, BYTE DIR_Attr);

//处理绝对路径、相对路径，调用前，DIR_Block要先初始化为CURRENT_DIR_BLOCK。找到路径上最后那一个文件或目录的目录项所在块
int processPath(char *path, unsigned int *DIR_Block, BYTE *DIR_WrtTime, BYTE *DIR_WrtDate, BYTE *DIR_FstClus, BYTE *DIR_FileSize, unsigned int *ItermID);

void printCurrentDIR();

void displayTime(BYTE *time, BYTE *date);

void writeTime(BYTE *Time, BYTE *Date);

int checkFAT();

#endif // !FAT12_H
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

//函数声明 
void printBIN(unsigned char m);
void printDEC(unsigned char m);
void printHEX(unsigned char m);
void printOCT(unsigned char m);
void printCHAR(unsigned char m);
int  getFileByteSize(FILE* fp);
void full_screen();
void init();
FILE* fileOpen();
void loadSetting(FILE *settingFileP);
void showSetting();
void showHelp();
//全局变量 
HWND hwnd;
char openFileName[256];
char dealedFileName[256];
int addressStyle=0;
int contentStyle=0;

int main()
{
	init();
	showHelp();
	FILE *settingFileP=fopen("setting.ini","r");
	if(settingFileP==NULL)
	{
		MessageBox(hwnd,TEXT("配置文件(setting.ini)打开失败,请检查程序文件是否完整或权限是否正常"),TEXT("错误"),MB_ICONERROR);
		return -1;
	}
	loadSetting(settingFileP);
	fclose(settingFileP);
	showSetting();
	FILE *fp=fileOpen();
	while (fp == NULL)//打开错误 
	{
		MessageBox(hwnd,TEXT("您所指定的文件打开失败,请检查文件是否存在或权限是否正常"),TEXT("错误"),MB_ICONERROR);
		system("cls");
		fp=fileOpen();
	}
	int fileSize = getFileByteSize(fp);
	printf("文件大小:%d字节\n", fileSize);
	unsigned char *a =(unsigned char*)malloc(fileSize);
	if(a==NULL)//动态内存分配错误 
	{
		MessageBox(hwnd,TEXT("文件过大 或 动态内存空间分配失败"),TEXT("错误"),MB_ICONERROR);
		return -1;
	}
	fread(a, 1, fileSize, fp);
	full_screen();
	for (int i = 0; i < fileSize; i++)
	{
		//每16个字节换一行 
		if((i)%16==0)
		{
			if(addressStyle==0)
			{
				printf("[地址:0x%06X] ",i,a[i]);
			}
			else if(addressStyle==1)
			{
				printf("[地址:%06d] ",i,a[i]);
			}
			else
			{
				MessageBox(hwnd,TEXT("发生了未知错误 ERROR CODE=1"),TEXT("严重错误"),MB_ICONERROR);
				free(fp);
				free(settingFileP);
				return -1;
			}
		}
		if(contentStyle==0)
		{
			printBIN(a[i]);
		}
		else if(contentStyle==1)
		{
			printDEC(a[i]);
		}
		else if(contentStyle==2)
		{
			printHEX(a[i]);
		}
		else if(contentStyle==3)
		{
			printOCT(a[i]);
		}
		else if(contentStyle==4)
		{
			printCHAR(a[i]);
		}
		else
		{
			MessageBox(hwnd,TEXT("发生了未知错误 ERROR CODE=2"),TEXT("严重错误"),MB_ICONERROR);
			free(fp);
			free(settingFileP);
			return -1;
		}
		if ((i + 1) % 16 == 0)printf("\n");
	}
	printf("\n");
	system("pause");
	free(a);
	fclose(fp);
	fclose(settingFileP);
}
void printBIN(unsigned char m)
{
	unsigned int n = m;
	unsigned int bit[8];
	for (int i = 0; i < 8; i++)
	{
		bit[i] = ((n >> 7 - i) << 31) >> 31;
	}
	printf("%d%d%d%d%d%d%d%d ", bit[0], bit[1], bit[2], bit[3], bit[4], bit[5], bit[6], bit[7]);
}
int getFileByteSize(FILE* fp)
{
	fseek(fp, 0L, SEEK_END);
	int size = ftell(fp);
	rewind(fp);//把文件重新指向开头 这非常必要 
	return size;
}
void full_screen()
{
    int cx = GetSystemMetrics(SM_CXSCREEN);            /* 屏幕宽度 像素 */
    int cy = GetSystemMetrics(SM_CYSCREEN);            /* 屏幕高度 像素 */
    LONG l_WinStyle = GetWindowLong(hwnd,GWL_STYLE);   /* 获取窗口信息 */
    /* 设置窗口信息 最大化 取消标题栏及边框 */
    SetWindowLong(hwnd,GWL_STYLE,(l_WinStyle | WS_POPUP | WS_MAXIMIZE));
    SetWindowPos(hwnd, HWND_TOP, 0, 0, cx, cy, SWP_SHOWWINDOW);
}
void printDEC(unsigned char m)
{
	printf("[%03u] ",m);
}
void printHEX(unsigned char m)
{
	printf("[%03X] ",m);
}
void printOCT(unsigned char m)
{
	printf("[%03o] ",m);
}
void printCHAR(unsigned char m)
{
	if(m>=33&&m<=127)
	{
		printf("%c ",m);
	}
	else
	{
		printf("  ");
	}
}
void init()
{
	hwnd = GetForegroundWindow();
	SetWindowText(hwnd,"二进制阅读器 v1.0");
}
FILE* fileOpen()
{
	printf("请输入要打开的文件名(如D:\\a.txt)或拖入该窗口并回车\n");
	gets(openFileName);
	FILE* fp = fopen(openFileName, "rb");
	return fp;
}
void loadSetting(FILE *settingFileP)
{
	int errorInfo1=0,errorInfo2=0;
	errorInfo1=fscanf(settingFileP,"%d",&addressStyle);
	if(addressStyle<0||addressStyle>1)
	{
		MessageBox(hwnd,TEXT("配置文件(setting.ini)中有错误,程序以默认设置运行(WAR-INI-1)"),TEXT("警告"),MB_ICONINFORMATION);
		addressStyle=0;	return;
	}
	errorInfo2=fscanf(settingFileP,"%d",&contentStyle);
	if(contentStyle<0||contentStyle>4)
	{
		MessageBox(hwnd,TEXT("配置文件(setting.ini)中有错误,程序以默认设置运行(WAR-INI-2)"),TEXT("警告"),MB_ICONINFORMATION);
		contentStyle=0;	return;
	}
}
void showSetting()
{
	if(addressStyle==0)puts("地址表示:十六进制HEX");
	else if(addressStyle==1)puts("地址表示:十进制DEC");
	else{}
	if(contentStyle==0)puts("内容表示:二进制BIN");
	else if(contentStyle==1)puts("内容表示:十进制DEC");
	else if(contentStyle==2)puts("内容表示:十六进制HEX");
	else if(contentStyle==3)puts("内容表示:八进制OCT");
	else if(contentStyle==4)puts("内容表示:ASCII");
	else{}
	puts("");
}
void showHelp()
{
	MessageBox(hwnd,TEXT("默认设置\n地址用十六进制表示\n内容用二进制表示\n如果需要更改显示的内容(进制等),请打开setting.ini自行修改\n\n由于printf()函数的自身原因 不建议打开500kb以上的文件\n显示完毕的时间可能会过长 敬请原谅"),TEXT("提示"),0);
}

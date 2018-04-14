#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS
#include<cstdio>
#include <IOSTREAM>
#include <CTIME>
#include<stdlib.h>
#include<string.h>
#include <tchar.h>
using namespace std;

FILE *fp1, *fp2;
int sudoku[9][9];
int rule[3][9][9] = { 0 }, map_start[9][9], flag[9][9];
//生成终盘
bool set(int x, int y, int val) 
{
	if (sudoku[y][x] != 0)		//非空
		return false;
	int x0, y0;
	for (x0 = 0; x0<9; x0++)
	{
		if (sudoku[y][x0] == val)	//行冲突
			return false;
	}
	for (y0 = 0; y0<9; y0++)
	{
		if (sudoku[y0][x] == val)	//列冲突
			return false;
	}
	for (y0 = y / 3 * 3; y0<y / 3 * 3 + 3; y0++)
	{
		for (x0 = x / 3 * 3; x0<x / 3 * 3 + 3; x0++)
		{
			if (sudoku[y0][x0] == val) //格冲突
				return false;
		}
	}
	sudoku[y][x] = val;
	return true;
}

void reset(int x, int y)
{
	sudoku[y][x] = 0;
}

void initXOrd(int* xOrd)	//0~9随机序列
{
	int i, k, tmp;
	for (i = 0; i<9; i++)
	{
		xOrd[i] = i;
	}
	for (i = 0; i<9; i++)
	{
		k = rand() % 9;
		tmp = xOrd[k];
		xOrd[k] = xOrd[i];
		xOrd[i] = tmp;
	}
}

bool fillFrom(int y, int val)
{
	int xOrd[9];
	initXOrd(xOrd);		//生成当前行的扫描序列
	for (int i = 0; i<9; i++)
	{
		int x = xOrd[i];
		if (set(x, y, val) || (y == 0 && val == 3))
		{
			if (y == 8)					//到了最后一行
			{
				if (val == 9 || fillFrom(0, val + 1))	//当前填9则结束, 否则从第一行填下一个数
					return true;
			}
			else
			{
				if (fillFrom(y + 1, val))	//下一行继续填当前数
					return true;
			}
			reset(x, y);	//回溯
		}
	}
	return false;
}

void printSudo()
{
	for (int y = 0; y<9; y++)
	{
		for (int x = 0; x<9; x++)
		{
			if (x<8)
				fprintf(fp1, "%d ", sudoku[y][x]);
			else
				fprintf(fp1, "%d", sudoku[y][x]);
		}
		fprintf(fp1, "\n");
	}
	fprintf(fp1, "\n");
	return;
}


//数独求解

int get_index_in_small(int y, int x)
{
	return 3 * (y / 3) + (x / 3);
}

int get_next(int y, int x, int start)
{//以当前坐标的当前值为基准，寻找交集中的下一个数
	for (int i = start; i<9; i++)
		if (rule[0][y][i] == 0 && rule[1][x][i] == 0 && rule[2][get_index_in_small(y, x)][i] == 0)return i;
	return -1;
}

void show()
{//打印九宫的可行解
	for (int i = 1; i <= 81; i++)
	{
		if((i % 9 - 1)!=8)
		fprintf(fp2, "%d ", map_start[i / 9][i % 9 - 1] + 1);
		else
			fprintf(fp2, "%d", map_start[i / 9][i % 9 - 1] + 1);
		if (!(i % 9))
			fprintf(fp2, "\n");
	}
	fprintf(fp2, "\n");
}

void change_rule(int style, int x, int y)
{//当style为0,改写规则为未占用。当style为1,改写规则为已占用
	rule[0][y][map_start[y][x]] = style;
	rule[1][x][map_start[y][x]] = style;
	rule[2][get_index_in_small(y, x)][map_start[y][x]] = style;
}

void solve(int y, int x)
{
	if (x == 9) { y++; x = 0; }
	while (!flag[y][x] && y <= 8)
	{
		x++;
		if (x == 9)
		{
			y++; x = 0;
		}
	}//获取下一个可填数的坐标
	if (y>8)
	{
		show();
		return;
	}//当前的坐标已经超出数独范围，默认已得出一个可行解
	while ((map_start[y][x] = get_next(y, x, map_start[y][x] + 1)) != -1)//以当前坐标的当前值为基准，寻找下一个可填数
	{
		change_rule(1, x, y);//改写规则为已占用
		solve(y, x + 1);      //若只想求出一个可行解就退出，可加上这代码：if(count_way)break;
		change_rule(0, x, y);//改写规则为未占用
	}
}




int main(int argc, char* argv[])
{
	if (argc == 3)
	{
		if (strcmp(argv[1], "-c") == 0)
		{
			if (argv[2][0] >= '0'&&argv[2][0] <= '9')
			{
				fp1 = fopen("sudoku.txt", "w");
				int n = atoi(argv[2]);
				srand((unsigned)time(NULL));
				for (int i = 0; i<n; i++)
				{
					memset(sudoku, 0, sizeof(sudoku));
					sudoku[0][0] = 3;
					while (!fillFrom(0, 1));
					printSudo();
				}
				fclose(fp1);
			}
			else
			{
				printf("重新输入\n");
				return 0;
			}
		}
		if (strcmp(argv[1], "-s") == 0)
		{
			char str[100];
			fp1 = fopen(argv[2], "r");
			fp2 = fopen("sudoku1.txt", "w");
			if (NULL == fp1)
			{
				printf("failed to open\n");
				return 1;
			}
			int i = 0;
			int k = 0;
			while (!feof(fp1))
			{
				memset(str, 0, sizeof(str));
				fgets(str, sizeof(str) - 1, fp1); 
				if (str[0] == '\n' || str[0] == '\0')
					continue;
				else
				{
					for (int j = 0; j < 17; j = j + 2)
					{
						map_start[i][k] = str[j] - '0';
						if (map_start[i][k]==0)
							flag[i][k] = 1;
						else
							flag[i][k] = 0;
						map_start[i][k]--;//为方便计算，初始化数字范围为0-8，代填的标为-1
						if (flag[i][k]==0)
							change_rule(1, k, i);//改写规则为已占用
						k++;
					}
					if (i == 8)
					{
						solve(0, 0);
						for (int i1 = 0; i1 < 3; i1++)
							for (int i2 = 0; i2 < 9; i2++)
								for (int i3 = 0; i3 < 9; i3++)
									rule[i1][i2][i3] = 0;/*重置*/
						i = 0;
						k = 0;
					}
					else
					{
						i++;
						k = 0;
					}
				}
			}
		}
	}
	else
	{
		printf("重新输入\n");
	}
	return 0;
}


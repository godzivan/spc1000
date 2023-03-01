#include <stdio.h>
#include <string.h>
#include "iocs.h"

unsigned char data[0x800];

void cas2dsk();
void dsk2cas();
void dsk2dsk();
void listdir(uint8 p, uint8 c);
void bootbasic();
int pifiles(char *);
int pioldnum();
void run(int);
// const int PAGE=12;
#define PAGE 12
void main(void)
{
	char  ch, p, l, c, pg;
	int t = 0, num = 0;
	char *addr = (char *)0x0c7f;
	*addr = 0;
	p = 0;
	l = 0;
	c = 0;
	num = 0;
	memset(data, 0, sizeof(data));
	cls();
	printf(" RPI extension box for SPC1000\n-------------------------------");
	ch = 0;
	t = pifiles("*.tap");
	l = (t / PAGE);
	num = pioldnum();
	num = 0;
	p = num / PAGE;
	c = num - PAGE * p;
	while(1)
	{
		gotoxy(0,2);
		if (p == l) {
			pg = t % PAGE - 1;
			if (c > pg)
				c = pg;
		}
		else
			pg = PAGE - 1;
		listdir(p, c);
		printf("-------------------------------       meeso.kim@gmail.com");
		// while(1);
		gotoxy(4,c+2);
		ch = getchar();
		switch (ch)
		{
			case 0x1d:
				if (p > 0)
					p--;
				break;
			case 0x1c:
				if (p < l)
					p++;
				break;
			case 0x1e:
				if (c > 0)
					c--;
				break;
			case 0x1f:
				if (c < pg)
					c++;
				break;
			case 0x0d:
				run((int)p * PAGE + c);
				break;
			default:
				continue;
		}
	}
}

void run(int num)
{
	int k = num, s=0;
	cls2();
	while(k--) while(data[s++] != 0);
	gotoxy(10, 6);
	printf("Loading...");
	gotoxy((32-strlen(data+s))/2, 8);
	printf("%s", data+s);
	pload2(num);
}

void listdir(uint8 p, uint8 c)
{
	int i = PAGE * p;
	uint8 j = 0;
	int k = i;
	int s = 0;
	while(k--) while(data[s++] != 0);
	attr_clear();
	for(;j<PAGE;j++)
	{
		if (*(data+s) != 0)
			printf("%03d. %-25s\n", i+j, data+s);
		else
			printf("%-30s\n", " ");
		while(data[s++] != 0);
	}
	attr_set(1, 0x840+c*32, 32);
}

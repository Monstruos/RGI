#include <stdlib.h>
#include <stdint.h>
#define _GNU_SOURCE
#include <stdio.h>

#pragma pack (1)
struct head {
	uint16_t bftype;
	uint32_t bfsize;
	uint16_t rez1, rez2;
	uint32_t bfoffbits;
	uint32_t bisize;
	uint32_t biwidth;
	uint32_t biheight;
	uint16_t biplanes;
	uint16_t bibitcount;
	uint32_t bicompression;
	uint32_t bisizeimage;
	uint32_t bix;
	uint32_t biy;
	uint32_t biclrused;
	uint32_t biclrimp;
} head_file;

unsigned char palitra[256][4];
unsigned char buffer[1024];

void main()
{
	FILE *f1 = fopen("../resources/kisa.BMP", "r");
	FILE *f2 = fopen("../resources/black_kisa.BMP", "w");
	fread(&head_file, sizeof(head_file), 1, f1);
	fwrite(&head_file, sizeof(head_file), 1, f2);
	for (int i = 0; i < (head_file.bfoffbits - sizeof(head_file)) / 4; i++) {
		fread(palitra[i], 4, 1, f1);

		int avg = palitra[i][0] + palitra[i][1] + palitra[i][2];
		avg /= 3;

		palitra[i][0] = avg;
		palitra[i][1] = avg;
		palitra[i][2] = avg;

		fwrite(palitra[i], 4, 1, f2);
	}
	int n;
	do {
		n = fread(buffer, 1, 1024, f1);
		fwrite(buffer, 1, n, f2);
	} while (n == 1024);
	fcloseall();
}

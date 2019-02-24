#include <stdlib.h>
#include <stdint.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>

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

#define BORDER 16
unsigned char palitra[256][4];

void main()
{
	srand(time(NULL));
	FILE *f1 = fopen("../resources/kisa.BMP", "r");
	FILE *f2 = fopen("../resources/ramka_kisa.BMP", "w");
	fread(&head_file, sizeof(head_file), 1, f1);
	head_file.biheight += BORDER * 2;
	head_file.biwidth += BORDER * 2;
	fwrite(&head_file, sizeof(head_file), 1, f2);
	for (int i = 0; i < (head_file.bfoffbits - sizeof(head_file)) / 4; i++) {
		fread(palitra[i], 4, 1, f1);
		fwrite(palitra[i], 4, 1, f2);
	}
	int n = head_file.biwidth;
	char *buffer_line = malloc(n * sizeof(char));

	for (int i = 0; i < BORDER; i++) {
		for (int j = 0; j < n; j++) {
			buffer_line[j] = rand() % 256;
		}
		fwrite(buffer_line, 1, n, f2);
	}
	for (int i = 0; i < head_file.biheight - BORDER * 2; i++) {
		fread(buffer_line + BORDER, 1, n - BORDER * 2, f1);
		for (int j = 0; j < BORDER; j++) {
			buffer_line[j] = rand() % 256;
		}
		for (int j = n - BORDER; j < n; j++) {
			buffer_line[j] = rand() % 256;
		}
		fwrite(buffer_line, 1, n, f2);
	}
	for (int i = 0; i < BORDER; i++) {
		for (int j = 0; j < n; j++) {
			buffer_line[j] = rand() % 256;
		}
		fwrite(buffer_line, 1, n, f2);
	}
	fcloseall();
}

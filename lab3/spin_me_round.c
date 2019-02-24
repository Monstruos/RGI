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

void read_bmp_head(FILE * f1)
{
	fread(&head_file, sizeof(head_file), 1, f1);
	for (int i = 0; i < (head_file.bfoffbits - sizeof(head_file)) / 4; i++) {
		fread(palitra[i], 4, 1, f1);
	}
}

void write_bmp_head(FILE * f1)
{
	fwrite(&head_file, sizeof(head_file), 1, f1);
	for (int i = 0; i < (head_file.bfoffbits - sizeof(head_file)) / 4; i++) {
		fwrite(palitra[i], 4, 1, f1);
	}
}

void main()
{
	FILE *f1 = fopen("../resources/spin_kisa.BMP", "r");
	FILE *f2 = fopen("../resources/spin2_kisa.BMP", "w");

	read_bmp_head(f1);
	size_t n = head_file.biheight * head_file.biwidth * sizeof(char);
	char *data = malloc(n);
	for (int i = 0; i < head_file.biheight; i++) {
		for (int j = 0; j < head_file.biwidth; j++) {
			fread(&data[i + j * head_file.biheight], 1, 1, f1);
		}

	}

	uint32_t swap = head_file.biheight;
	head_file.biheight = head_file.biwidth;
	head_file.biwidth = swap;

	write_bmp_head(f2);
	fwrite(data, 1, n, f2);
	fcloseall();
}

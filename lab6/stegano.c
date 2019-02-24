#include <stdlib.h>
#include <stdint.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <SDL2/SDL.h>

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

#pragma pack (1)
typedef struct RGBquad {
	char rgbBlue;
	char rgbGreen;
	char rgbRed;
	char rgbReserved;
} RGBQ;

FILE *f1;
FILE *textf;
FILE *f2;
int color_num;

void bmp_handler();
void copy_st_image_tc();
void rounding(int size);
int read_and_encode(unsigned char *buf, int n);
char next_bit();

int byte_write = 0;

int main(void)
{
	f1 = fopen("../resources/TC.bmp", "rb");
	textf = fopen("../resources/text.txt", "r");
	fseek(textf, 0L, SEEK_END);
	int text_file_size = ftell(textf);
	rewind(textf);
	f2 = fopen("../resources/TC_text.bmp", "w");

	bmp_handler();
	fcloseall();
	printf("%i/%i bytes writed\n", byte_write, text_file_size);

	return 0;

}

void bmp_handler()
{
	fread(&head_file, sizeof(head_file), 1, f1);
	fwrite(&head_file, sizeof(head_file), 1, f2);
	color_num = 1 << head_file.bibitcount;
	switch (color_num) {
	case (1 << 24):
		rounding(sizeof(RGBQ) * color_num);
		copy_st_image_tc();
		break;
	default:
		printf("Bad match. No such color resolution.");
		exit(1);
		break;
	}
}

void rounding(int size)
{
	if (size % 4 != 0) {
		for (size_t i = 0; i < size % 4; i++) {
			char buf;
			fread(&buf, 1, 1, f1);
			fwrite(&buf, 1, 1, f2);
		}
	}
}

void copy_st_image_tc()
{
	unsigned char buf[3];
	int i, j;
	for (i = 0; i < head_file.biheight; ++i) {
		for (j = 0; j < head_file.biwidth; j++) {
			int n = read_and_encode(buf, 3);
			fwrite(buf, 1, n, f2);
		}
		rounding(head_file.biwidth);
	}
}

char next_bit()
{
	static int bit = 7;
	static unsigned char file_byte;
	if (bit == 7) {
		int n = fread(&file_byte, sizeof(unsigned char), 1, textf);
		if (n == 0)
			return 0;
	}
	char ret_bit = (file_byte >> bit--) & 0x1;
	if (bit < 0) {
		bit = 7;
		byte_write++;
	}
	return ret_bit;
}

int read_and_encode(unsigned char *buf, int n)
{
	int ret = fread(buf, sizeof(unsigned char), n, f1);
	for (int i = 0; i < n; i++) {
		buf[i] = (buf[i] & 0xFE) | next_bit();
	}
	return ret;
}

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
int color_num;

void bmp_handler();
void decode_st_image_tc();
void rounding(int size);
int read_and_decode(unsigned char *buf, int n);
void save_bit(unsigned char bit);

int text_readable = 1;

int main(void)
{
	f1 = fopen("../resources/TC_text.bmp", "rb");
	textf = fopen("../resources/text_decode.txt", "w");
	bmp_handler();
	fcloseall();

	return 0;

}

void bmp_handler()
{
	fread(&head_file, sizeof(head_file), 1, f1);
	color_num = 1 << head_file.bibitcount;
	switch (color_num) {
	case (1 << 24):
		rounding(sizeof(RGBQ) * color_num);
		decode_st_image_tc();
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
			read_and_decode(&buf, 1);
		}
	}
}

void decode_st_image_tc()
{
	unsigned char buf[3];
	int i, j;
	for (i = 0; i < head_file.biheight; ++i) {
		for (j = 0; j < head_file.biwidth; j++) {
			int n = read_and_decode(buf, 3);
		}
		rounding(head_file.biwidth);
	}
}

void save_bit(unsigned char bit)
{
	bit &= 0x1;
	static int count = 7;
	static unsigned char file_byte = 0;
	file_byte = file_byte | (bit << count);
	count--;
	if (count < 0) {
		count = 7;
		fwrite(&file_byte, 1, 1, textf);
		if (file_byte != 0) {
			file_byte = 0;
		} else {
			text_readable = 0;
			fwrite(&file_byte, 1, 1, textf);
		}
	}
}

int read_and_decode(unsigned char *buf, int n)
{
	int ret = fread(buf, sizeof(unsigned char), n, f1);
	if (text_readable) {
		for (int i = 0; i < n; i++) {
			save_bit(buf[i] & 0x1);
		}
	}
	return ret;
}

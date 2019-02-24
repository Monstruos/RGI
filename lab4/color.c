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

RGBQ *palitra;
int color_num;

#define SCALE_256 2
#define SCREEN_SIZE_X 1366
#define SCREEN_SIZE_Y 768
SDL_Renderer *renderer;
SDL_Window *window;

void bmp_handler(FILE * f1);
void read_draw_image_16(FILE * f1);
void read_draw_image_256(FILE * f1);
void read_draw_image_256_scalable(FILE * f1);
void read_draw_image_tc(FILE * f1);
void rounding(FILE * f1, int size);

int main(void)
{
	SDL_Init(SDL_INIT_VIDEO);
	//Set up screen
	SDL_CreateWindowAndRenderer(SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, &window,
				    &renderer);

	FILE *f1 = fopen("../resources/256.bmp", "rb");
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	bmp_handler(f1);
	SDL_RenderPresent(renderer);

	SDL_Event event;
	while (1) {
		if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
			break;
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	fcloseall();

	return 0;

}

void bmp_handler(FILE * f1)
{
	fread(&head_file, sizeof(head_file), 1, f1);
	color_num = 1 << head_file.bibitcount;
	switch (color_num) {
	case 16:
		palitra = malloc(color_num * sizeof(RGBQ));
		fread(palitra, sizeof(RGBQ), color_num, f1);
		rounding(f1, sizeof(RGBQ) * color_num);
		read_draw_image_16(f1);
		break;
	case 256:
		palitra = malloc(color_num * sizeof(RGBQ));
		fread(palitra, sizeof(RGBQ), color_num, f1);
		rounding(f1, sizeof(RGBQ) * color_num);
		read_draw_image_256_scalable(f1);
		break;
	case (1 << 24):
		rounding(f1, sizeof(RGBQ) * color_num);
		read_draw_image_tc(f1);
		break;
	default:
		printf("Bad match. No such color resolution.");
		exit(1);
		break;
	}
}

void rounding(FILE * f1, int size)
{
	if (size % 4 != 0) {
		for (size_t i = 0; i < size % 4; i++) {
			char buf;
			fread(&buf, 1, 1, f1);
		}
	}
}

void read_draw_image_16(FILE * f1)
{
	int x_center = SCREEN_SIZE_X / 2;
	int y_center = SCREEN_SIZE_Y / 2;
	int x_start = x_center - head_file.biwidth / 2;
	int y_start = y_center + head_file.biheight / 2;
	int i, j;
	for (i = 0; i < head_file.biheight; ++i) {
		for (j = 0; j < head_file.biwidth; j += 2) {
			unsigned char buf;
			int n = fread(&buf, 1, 1, f1);
			if (n != 1)
				break;
			RGBQ fst = palitra[(buf >> 4) & ((1 << 4) - 1)];
			RGBQ snd = palitra[buf & ((1 << 4) - 1)];
			SDL_SetRenderDrawColor(renderer, fst.rgbRed,
					       fst.rgbGreen, fst.rgbBlue, 255);
			SDL_RenderDrawPoint(renderer, x_start + j, y_start - i);
			SDL_SetRenderDrawColor(renderer, snd.rgbRed,
					       snd.rgbGreen, snd.rgbBlue, 255);
			SDL_RenderDrawPoint(renderer, x_start + j + 1,
					    y_start - i);
		}
	}
}

void read_draw_image_256(FILE * f1)
{
	int x_center = SCREEN_SIZE_X / 2;
	int y_center = SCREEN_SIZE_Y / 2;
	int x_start = x_center - head_file.biwidth / 2;
	int y_start = y_center + head_file.biheight / 2;
	int i, j;
	for (i = 0; i < head_file.biheight; ++i) {
		for (j = 0; j < head_file.biwidth; j++) {
			unsigned char buf;
			int n = fread(&buf, 1, 1, f1);
			if (n != 1)
				break;
			RGBQ pix = palitra[buf];
			SDL_SetRenderDrawColor(renderer, pix.rgbRed,
					       pix.rgbGreen, pix.rgbBlue, 255);
			SDL_RenderDrawPoint(renderer, x_start + j, y_start - i);
		}
		rounding(f1, head_file.biwidth);
	}
}

void read_draw_image_tc(FILE * f1)
{
	unsigned char buf[3];
	int x_center = SCREEN_SIZE_X / 2;
	int y_center = SCREEN_SIZE_Y / 2;
	int x_start = x_center - head_file.biwidth / 2;
	int y_start = y_center + head_file.biheight / 2;
	int i, j;
	for (i = 0; i < head_file.biheight; ++i) {
		for (j = 0; j < head_file.biwidth; j++) {
			int n = fread(buf, sizeof(unsigned char), 3, f1);
			if (n != 3)
				break;
			SDL_SetRenderDrawColor(renderer, (int)buf[2],
					       (int)buf[1], (int)buf[0], 255);
			SDL_RenderDrawPoint(renderer, x_start + j, y_start - i);
		}
		rounding(f1, head_file.biwidth);
	}
}

void read_draw_image_256_scalable(FILE * f1)
{
	int x_center = SCREEN_SIZE_X / 2;
	int y_center = SCREEN_SIZE_Y / 2;
	int x_start = x_center - (int)(head_file.biwidth / 2 * SCALE_256);
	int y_start = y_center + (int)(head_file.biheight / 2 * SCALE_256);
	unsigned char *rastr = malloc(head_file.biwidth * head_file.biheight);
	for (int i = 0; fread(&rastr[i], 1, 1024, f1) == 1024; i += 1024) ;

	double i, j;
	int x = 0, y = 0;
	for (i = 0.0; i < head_file.biheight; i += 1.0 / SCALE_256) {
		for (j = 0.0; j < head_file.biwidth; j += 1.0 / SCALE_256) {
			RGBQ pix =
			    palitra[rastr
				    [((int)i) * head_file.biwidth + ((int)j)]];
			SDL_SetRenderDrawColor(renderer, pix.rgbRed,
					       pix.rgbGreen, pix.rgbBlue, 255);
			SDL_RenderDrawPoint(renderer, x_start + x, y_start - y);
			x++;
		}
		y++;
		x = 0;
	}
	free(rastr);
}

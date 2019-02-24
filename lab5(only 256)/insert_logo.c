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
} head_file, logo_head_file;

#pragma pack (1)
typedef struct RGBquad {
	unsigned char rgbBlue;
	unsigned char rgbGreen;
	unsigned char rgbRed;
	unsigned char rgbReserved;
} RGBQ;

RGBQ *logo;
int color_num;

#define SCREEN_SIZE_X 1366
#define SCREEN_SIZE_Y 768
SDL_Renderer *renderer;
SDL_Window *window;

void bmp_handler(FILE * f1);
void read_draw_image_tc(FILE * f1);
void rounding(FILE * f1, int size);
void read_logo();
void insert_logo(unsigned char *buf, int i, int j);

int main(void)
{
	SDL_Init(SDL_INIT_VIDEO);
	//Set up screen
	SDL_CreateWindowAndRenderer(SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, &window,
				    &renderer);

	FILE *f1 = fopen("../resources/TC.bmp", "rb");
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
	read_logo();
	color_num = 1 << head_file.bibitcount;
	switch (color_num) {
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
			fread(buf, sizeof(unsigned char), 3, f1);
			if (i > 100 && i < logo_head_file.biheight + 100
			    && j > 100 && j < logo_head_file.biwidth + 100) {
				insert_logo(buf, i - 100, j - 100);
			}
			SDL_SetRenderDrawColor(renderer, (int)buf[2],
					       (int)buf[1], (int)buf[0], 255);
			SDL_RenderDrawPoint(renderer, x_start + j, y_start - i);
		}
		rounding(f1, head_file.biwidth);
	}
}

void read_logo()
{
	FILE *logo_file = fopen("../resources/logo.bmp", "rb");
	fread(&logo_head_file, sizeof(logo_head_file), 1, logo_file);

	// Converter insert palitra in TC
	RGBQ *logo_palitra;
	logo_palitra = malloc(21 * sizeof(RGBQ));
	fread(logo_palitra, sizeof(RGBQ), 21, logo_file);
	free(logo_palitra);
	int i, j;
	logo =
	    malloc(sizeof(RGBQ) * logo_head_file.biheight *
		   logo_head_file.biwidth);
	for (i = 0; i < logo_head_file.biheight; ++i) {
		for (j = 0; j < logo_head_file.biwidth; ++j) {
			unsigned char buf[3];
			fread(buf, 3, 1, logo_file);
			int index = i * logo_head_file.biwidth + j;
			logo[index].rgbBlue = buf[0];
			logo[index].rgbGreen = buf[1];
			logo[index].rgbRed = buf[2];
			printf("{%3i, %3i, %3i}\n", logo[index].rgbRed,
			       logo[index].rgbGreen, logo[index].rgbBlue);
		}
	}
}

void insert_logo(unsigned char *buf, int i, int j)
{
	RGBQ logo_pixel = logo[i * logo_head_file.biwidth + j];
	if (logo_pixel.rgbBlue > 200 && logo_pixel.rgbGreen > 200
	    && logo_pixel.rgbRed > 200) {
		return;
	}
	buf[0] = ((int)buf[0] + logo_pixel.rgbBlue) / 2;
	buf[1] = ((int)buf[1] + logo_pixel.rgbGreen) / 2;
	buf[2] = ((int)buf[2] + logo_pixel.rgbRed) / 2;
}

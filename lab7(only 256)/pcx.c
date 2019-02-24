#include <stdlib.h>
#include <stdint.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <SDL2/SDL.h>

#pragma pack (1)
typedef struct TPCXHeaderStruct {
	unsigned char ID;	//Постоянный флаг 10 = ZSoft .PCX
	unsigned char Version;	// Информация о версии
	unsigned char Coding;	//1 = PCX кодирование RLE
	unsigned char BitPerPixel;	// Число бит на пиксел
	uint16_t XMin;		// Размеры изображения
	uint16_t YMin;
	uint16_t XMax;
	uint16_t YMax;
	uint16_t HRes;		// Горизонтальное разрешение создающего устройства
	uint16_t VRes;		// Вертикальное разрешение создающего устройства
	unsigned char Palette[48];	// Набор цветовой палитры (для 16-цветного изображения)
	unsigned char Reserved;
	unsigned char Planes;	// Число цветовых слоев
	uint16_t BytePerLine;	// Число байт на строку в цветовом слое (для PCX-файлов всегда должно быть четным)
	uint16_t PaletteInfo;	// Как интерпретировать палитру:
	// 1 = цветная/черно-белая,
	// 2 = градации серого
	uint16_t HScreenSize;
	uint16_t VScreenSize;
	unsigned char Filler[54];	// Заполняется нулями до конца заголовка
} TPCX;

#pragma pack (1)
typedef struct RGBquad {
	char rgbRed;
	char rgbGreen;
	char rgbBlue;
} RGB;

TPCX head_file;
RGB *palitra;
unsigned char *rastr;
int size_x;
int size_y;
int total_bytes;

#define SCREEN_SIZE_X 1366
#define SCREEN_SIZE_Y 768
SDL_Renderer *renderer;
SDL_Window *window;

void pcx_handler(FILE * f1);
void rounding(FILE * f1, int size);
void read_data(FILE * f1);
int read_data1(unsigned char *data, int *count, FILE * f1);
void read_palette(FILE * f1);
void draw_data();

int main(void)
{
	SDL_Init(SDL_INIT_VIDEO);
	//Set up screen
	SDL_CreateWindowAndRenderer(SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, &window,
				    &renderer);

	FILE *f1 = fopen("../resources/CAT_LIT.PCX", "rb");
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	pcx_handler(f1);
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

void pcx_handler(FILE * f1)
{
	fread(&head_file, sizeof(head_file), 1, f1);
	size_x = head_file.XMax - head_file.XMin + 1;
	size_y = head_file.YMax - head_file.YMin + 1;

	rastr = malloc(sizeof(unsigned char *) * size_y * size_x);
	total_bytes = head_file.Planes * head_file.BytePerLine;

	if (head_file.Version == 5 && head_file.BitPerPixel == 8) {
		read_data(f1);
		read_palette(f1);
		draw_data();
	}
}

void rounding(FILE * f1, int readed)
{
	int diff = total_bytes - readed;
	if (diff > 0) {
		int trash_size;
		printf("round %i\n", diff);
		unsigned char *trash = malloc(sizeof(unsigned char) * diff);
		read_data1(trash, &trash_size, f1);
		if (trash_size != diff) {
			printf("rounding error");
		}
		free(trash);
	}
}

void rounding_line(FILE * f1)
{
	int round = 8 - (size_y % 8);
	if (round != 8) {
		printf("round_lines: %i\n", round);
		for (int i = 0; i < round; i++) {
			unsigned char *trash =
			    malloc(sizeof(unsigned char) * total_bytes);
			fread(trash, sizeof(unsigned char), total_bytes, f1);
			free(trash);
		}
	}
}

void read_data(FILE * f1)
{
	int count = 0;
	int readed_bytes = 0;
	for (int i = 0; i < size_y; i++) {
		int readed_line_bytes = 0;
		while (readed_line_bytes < size_x) {
			if (read_data1
			    (rastr + readed_line_bytes + readed_bytes, &count,
			     f1) == 0) {
				readed_line_bytes += count;
			} else {
				break;
			}
		}
		readed_bytes += readed_line_bytes;
		rounding(f1, readed_line_bytes);
	}
	rounding_line(f1);
}

int read_data1(unsigned char *data, int *count, FILE * f1)
{
	unsigned char buf;
	*count = 1;
	if (0 == fread(&buf, 1, 1, f1)) {
		printf("Can't read file");
		return -1;
	}

	if (0xc0 == (0xc0 & buf)) {
		(*count) = 0x3f & buf;
		if (0 == fread(&buf, 1, 1, f1)) {
			printf("No data byte");
			return -1;
		}
		for (int i = 0; i < (*count); i++) {
			data[i] = buf;
		}
	} else {
		*data = buf;
	}
	return 0;
}

void read_palette(FILE * f1)
{
	fseek(f1, -769, SEEK_END);
	unsigned char delim;
	fread(&delim, 1, 1, f1);
	if (delim != 12) {
		printf("No palette\n");
		return;
	}
	palitra = malloc(256 * sizeof(RGB));
	fread(palitra, sizeof(RGB), 256, f1);
}

void draw_data()
{
	int x_center = SCREEN_SIZE_X / 2;
	int y_center = SCREEN_SIZE_Y / 2;
	int x_start = x_center - size_x / 2;
	int y_start = y_center - size_y / 2;
	for (int i = 0; i < size_y; ++i) {
		for (int j = 0; j < size_x; j++) {
			unsigned char buf = rastr[i * size_x + j];
			int r = palitra[buf].rgbRed;
			int g = palitra[buf].rgbGreen;
			int b = palitra[buf].rgbBlue;
			SDL_SetRenderDrawColor(renderer, r, g, b, 255);
			SDL_RenderDrawPoint(renderer, x_start + j, y_start + i);
		}
	}
}

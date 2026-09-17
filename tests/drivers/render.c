#include "render.h"

static uint16_t *const vga_buffer = (uint16_t *)VGA_ADDRESS;
static uint16_t double_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];

void render_init(void) {
	render_clear(COLOR_BLACK);
	render_present();
}

void render_clear(uint8_t color) {
	uint16_t val = ((uint16_t)color << 12) | ' ';
	int i = 0;
	while(i < SCREEN_WIDTH * SCREEN_HEIGHT) {
		double_buffer[i++] = val;
	}
}

void render_pixel(int x, int y, uint8_t color, char glyph) {
	if((unsigned int)x < SCREEN_WIDTH && (unsigned int)y < SCREEN_HEIGHT) {
		double_buffer[y * SCREEN_WIDTH + x] = ((uint16_t)color << 12) | (unsigned char)glyph;
	}
}

void render_rect(int x, int y, int width, int height, uint8_t color, char glyph) {
	int start_x = x < 0 ? 0 : x;
	int start_y = y < 0 ? 0 : y;
	int end_x = x + width;
	int end_y = y + height;

	if(end_x > SCREEN_WIDTH) end_x = SCREEN_WIDTH;
	if(end_y > SCREEN_HEIGHT) end_y = SCREEN_HEIGHT;

	uint16_t val = ((uint16_t)color << 12) | (unsigned char)glyph;

	for(int row = start_y; row < end_y; row++) {
		int row_offset = row * SCREEN_WIDTH;
		for(int col = start_x; col < end_x; col++) {
			double_buffer[row_offset + col] = val;
		}
	}
}

void render_char(int x, int y, char c, uint8_t fg, uint8_t bg) {
	if((unsigned int)x < SCREEN_WIDTH && (unsigned int)y < SCREEN_HEIGHT) {
		uint16_t attr = ((uint16_t)bg << 4) | (fg & 0x0F);
		double_buffer[y * SCREEN_WIDTH + x] = (attr << 8) | (unsigned char)c;
	}
}

void render_string(int x, int y, const char *str, uint8_t fg, uint8_t bg) {
	if((unsigned int)y >= SCREEN_HEIGHT) return;

	uint16_t attr = ((uint16_t)bg << 4) | (fg & 0x0F);
	int cur_x = x;

	while(*str && cur_x < SCREEN_WIDTH) {
		if(cur_x >= 0) {
			double_buffer[y * SCREEN_WIDTH + cur_x] = (attr << 8) | (unsigned char)*str;
		}
		cur_x++;
		str++;
	}
}

void render_present(void) {
	int i = 0;
	while(i < SCREEN_WIDTH * SCREEN_HEIGHT) {
		vga_buffer[i] = double_buffer[i];
		i++;
	}
}

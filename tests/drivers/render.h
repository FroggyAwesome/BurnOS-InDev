#ifndef RENDER_H
#define RENDER_H

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define VGA_ADDRESS 0xB8000

#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_BROWN 6
#define COLOR_LIGHT_GRAY 7
#define COLOR_DARK_GRAY 8
#define COLOR_LIGHT_BLUE 9
#define COLOR_LIGHT_GREEN 10
#define COLOR_LIGHT_CYAN 11
#define COLOR_LIGHT_RED 12
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_YELLOW 14
#define COLOR_WHITE 15

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

void render_init(void);
void render_clear(uint8_t color);
void render_pixel(int x, int y, uint8_t color, char glyph);
void render_rect(int x, int y, int width, int height, uint8_t color, char glyph);
void render_char(int x, int y, char c, uint8_t fg, uint8_t bg);
void render_string(int x, int y, const char *str, uint8_t fg, uint8_t bg);
void render_present(void);

#endif

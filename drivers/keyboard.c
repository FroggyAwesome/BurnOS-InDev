#include "keyboard.h"
#include "io.h"
#include "vga.h"

unsigned char
get_scancode(void)
{
	while (!(inb(0x64) & 1)) {
		__asm__ volatile("pause");
	}
	return inb(0x60);
}

void
keyboard_flush(void)
{
	while (inb(0x64) & 0x01) {
		inb(0x60);
		__asm__ volatile("pause");
	}
}

char
scancode_to_ascii(unsigned char scancode)
{
	static const char map[128] = {
		0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
		0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
		0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
		0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
		'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		'7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0,
		0, 0, 0, 0, 0
	};
	if (scancode & 0x80)
		return 0;
	return map[scancode & 0x7F];
}

void
read_line(char *buf, int max_len)
{
	int i = 0;
	while (i < max_len - 1) {
		char c = scancode_to_ascii(get_scancode());
		if (c == 0)
			continue;
		if (c == '\n') {
			vga_putchar('\n');
			break;
		}
		if (c == '\b') {
			if (i > 0) {
				i--;
				vga_putchar('\b');
			}
			continue;
		}
		if (c >= 32 && c <= 126) {
			buf[i++] = c;
			vga_putchar(c);
		}
	}
	buf[i] = '\0';
}

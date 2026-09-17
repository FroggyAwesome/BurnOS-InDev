#include "keyboard.h"
#include "io.h"
#include "vga.h"

#define KEYBOARD_BUFFER_SIZE 256

static unsigned char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static volatile int buffer_head = 0;
static volatile int buffer_tail = 0;

void keyboard_handler(void);

void
keyboard_handler(void)
{
	unsigned char scancode = inb(0x60);
	int next = (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
	if (next != buffer_tail) {
		keyboard_buffer[buffer_head] = scancode;
		buffer_head = next;
	}
}

unsigned char
get_scancode(void)
{
	while (buffer_head == buffer_tail) {
		__asm__ volatile("sti; hlt");
	}
	unsigned char scancode = keyboard_buffer[buffer_tail];
	buffer_tail = (buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
	return scancode;
}

void
keyboard_flush(void)
{
	buffer_head = 0;
	buffer_tail = 0;
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

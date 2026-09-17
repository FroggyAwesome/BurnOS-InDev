#include "io.h"

void
outb(unsigned short port, unsigned char val)
{
	__asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

void
outw(unsigned short port, unsigned short val)
{
	__asm__ volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

unsigned char
inb(unsigned short port)
{
	unsigned char ret;
	__asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

void
io_wait(void)
{
	outb(0x80, 0);
}

void
sleep_ms(unsigned int milliseconds)
{
	for (unsigned int i = 0; i < milliseconds; i++) {
		for (unsigned int j = 0; j < 1000; j++) {
			io_wait();
			__asm__ volatile("pause");
		}
	}
}

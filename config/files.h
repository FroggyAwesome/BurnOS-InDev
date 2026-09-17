#ifndef FILES_H
#define FILES_H

#include "vfs.h"

static struct file files[] = {
	{ "kernel.c", "void main() {\n    // BurnOS kernel core\n}" },
	{ "welcome.txt", "Welcome to BurnOS! Flat file system active." }
};

static const int file_count = 2;

#endif

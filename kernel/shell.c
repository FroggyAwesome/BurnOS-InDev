#include "shell.h"
#include "drivers.h"
#include "files.h"
#include "games.h"
#include "general.h"
#include "kernel.h"
#include "keyboard.h"
#include "os.h"
#include "system.h"
#include "vfs.h"
#include "vga.h"

static char current_prompt[32] = DEFAULT_PROMPT;
static unsigned char current_bg = 1;
static unsigned char current_fg = 15;

static int
streq(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}
	return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

static int
strprefix(const char *pre, const char *str)
{
	while (*pre) {
		if (*pre++ != *str++)
			return 0;
	}
	return 1;
}

static void
help(void)
{
	print("Available commands:\n");
	print("  help                - set prompt [n|c|f]\n");
	print("  whoami              - games\n");
	print("  hostname            - reboot\n");
	print("  version             - poweroff\n");
	print("  arch                - exit\n");
	print("  dir                 - panic\n");
	print("  cat [FILE]          - halt\n");
	print("  echo [TEXT]         - set cursor [b|h|l]\n");
	print("  cls                 -\n");
	print("  set color [BG FG|d] -\n");
}

static int
parse_int(const char *str)
{
	int res = 0;
	while (*str >= '0' && *str <= '9') {
		res = res * 10 + (*str - '0');
		str++;
	}
	return res;
}

static void
execute_set(const char *args)
{
	if (strprefix("color ", args)) {
		const char *p = args + 6;

		if (p[0] == 'd' && (p[1] == '\0' || p[1] == ' ')) {
			vga_set_color(SHELL_BG, SHELL_FG);
			clear_screen();
			print("Color reset to default.\n");
			return;
		}

		if (p[0] < '0' || p[0] > '7' || p[1] != ' ') {
			print("Error: Background must be 0-7.\n");
			return;
		}

		int bg = p[0] - '0';
		p += 2;

		if (*p < '0' || *p > '9') {
			print("Error: Foreground must be 0-15.\n");
			return;
		}

		int fg = 0;
		while (*p >= '0' && *p <= '9') {
			fg = fg * 10 + (*p - '0');
			p++;
		}

		if (fg > 15) {
			print("Error: Foreground must be 0-15.\n");
			return;
		}

		current_bg = (unsigned char)bg;
		current_fg = (unsigned char)fg;

		vga_set_color(current_bg, current_fg);
		clear_screen();
		print("Color updated successfully.\n");
	} else if (strprefix("prompt ", args)) {
		char type = args[7];
		const char *src = SH_PROMPT_NORMAL;

		if (type == 'n') {
			src = SH_PROMPT_NORMAL;
		} else if (type == 'c') {
			src = SH_PROMPT_CLASSIC;
		} else if (type == 'f') {
			src = SH_PROMPT_FIXED;
		} else {
			print("Invalid prompt type. Use: n, c, f\n");
			return;
		}

		int i = 0;
		while (src[i] != '\0' && i < 31) {
			current_prompt[i] = src[i];
			i++;
		}
		current_prompt[i] = '\0';
	} else if (strprefix("cursor ", args)) {
		char type = args[7];
		if (type == 'b' || type == 'h' || type == 'l') {
			set_cursor_form(type);
		} else {
			print("Invalid cursor type. Use: b (block), h (half), l (line)\n");
		}
	} else {
		print("Invalid set syntax. Use 'set color', 'set prompt', or 'set cursor'\n");
	}
}

static void
execute_command(const char *buffer)
{
	if (streq(buffer, "help") == 0) {
		help();
	} else if (streq(buffer, "dir") == 0) {
		vfs_dir();
	} else if (strprefix("cat ", buffer)) {
		vfs_cat(buffer + 4);
	} else if (streq(buffer, "poweroff") == 0) {
		poweroff();
	} else if (streq(buffer, "reboot") == 0) {
		reboot();
	} else if (streq(buffer, "cls") == 0) {
		clear_screen();
	} else if (streq(buffer, "version") == 0) {
		print(OS_VERSION "\n");
	} else if (streq(buffer, "arch") == 0) {
		print(ARCH "\n");
	} else if (strprefix("echo ", buffer)) {
		print(buffer + 5);
		print("\n");
	} else if (streq(buffer, "whoami") == 0) {
		print(USER "\n");
	} else if (streq(buffer, "hostname") == 0) {
		print(HOST "\n");
	} else if (streq(buffer, "panic") == 0) {
		panic("Forced");
	} else if (strprefix("panic ", buffer)) {
		panic(buffer + 6);
	} else if (streq(buffer, "halt") == 0) {
		halt();
	} else if (streq(buffer, "games") == 0) {
		games_main();
	} else if (strprefix("set ", buffer)) {
		execute_set(buffer + 4);
	} else {
		print("Command not found. Type 'help' for available commands.\n");
	}
}

void
shell(void)
{
	char buffer[256];

	for (int i = 0; DEFAULT_PROMPT[i] != '\0' && i < 31; i++) {
		current_prompt[i] = DEFAULT_PROMPT[i];
		current_prompt[i + 1] = '\0';
	}

	vga_set_color(SHELL_BG, SHELL_FG);
	clear_screen();
	print(MOTD);
	set_cursor_form('b');

	while (1) {
		print(current_prompt);
		read_line(buffer, sizeof(buffer));

		if (buffer[0] == '/' || buffer[0] == '\0') {
			continue;
		}

		if (streq(buffer, "exit") == 0) {
			vga_reset_color();
			clear_screen();
			return;
		}

		execute_command(buffer);
	}
}

#include "games.h"
#include "keyboard.h"
#include "vga.h"

static int g_seed = 12345;

static void
srand_custom(int seed)
{
	g_seed = seed;
}

static int
rand_custom(void)
{
	g_seed = g_seed * 1103515245 + 12345;
	return (int)((unsigned int)(g_seed / 65536) % 32767U);
}

static int
str_eq(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}
	return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

static void
wait_key(void)
{
	char buf[2];
	read_line(buf, sizeof(buf));
}

static void
play_guess(void)
{
	char buf[16];
	int target = (rand_custom() % 10) + 1;
	int attempts = 5;

	while (attempts > 0) {
		clear_screen();
		print("=== GUESS THE NUMBER (1-10) ===\n");
		print("Attempts left: ");
		vga_putchar((char)('0' + attempts));
		print("\n\nEnter guess > ");
		read_line(buf, sizeof(buf));

		int guess = 0;
		const char *p = buf;
		while (*p >= '0' && *p <= '9') {
			guess = guess * 10 + (*p - '0');
			p++;
		}

		if (guess == target) {
			print("\nYou win! Press any key...\n");
			wait_key();
			return;
		} else if (guess < target) {
			print("\nToo LOW! Press any key...\n");
		} else {
			print("\nToo HIGH! Press any key...\n");
		}
		wait_key();
		attempts--;
	}
	print("\nGame Over! The number was: ");
	vga_putchar((char)('0' + target));
	print("\nPress any key...\n");
	wait_key();
}

static void
play_hangman(void)
{
	const char *words[5] = { "APPLE", "HOUSE", "TRAIN", "PLANT", "WATER" };
	int secret_idx = rand_custom() % 5;
	const char *word = words[secret_idx];
	char guessed[5] = { 0 };
	int lives = 5;
	char buf[16];

	while (lives > 0) {
		clear_screen();
		print("=== HANGMAN ===\n");
		print("Lives: ");
		vga_putchar((char)('0' + lives));
		print("\nWord: ");

		int won = 1;
		for (int i = 0; i < 5; i++) {
			if (guessed[i]) {
				vga_putchar(word[i]);
			} else {
				vga_putchar('_');
				won = 0;
			}
			vga_putchar(' ');
		}
		print("\n\nGuess letter > ");
		read_line(buf, sizeof(buf));

		if (buf[0] == '\0')
			continue;
		char c = buf[0];
		if (c >= 'a' && c <= 'z')
			c -= 32;

		if (won) {
			print("\nYou WIN! Press any key...\n");
			wait_key();
			return;
		}

		int found = 0;
		for (int i = 0; i < 5; i++) {
			if (word[i] == c) {
				guessed[i] = 1;
				found = 1;
			}
		}
		if (!found)
			lives--;
	}
	clear_screen();
	print("Game Over! Word was: ");
	print(word);
	print("\nPress any key...\n");
	wait_key();
}

void
games_main(void)
{
	char buf[16];
	while (1) {
		clear_screen();
		print("========================================\n");
		print("            BURNOS ARCADE HUB\n");
		print("========================================\n");
		print(" 1. Guess the Number (1-10, 5 tries)\n");
		print(" 2. Hangman\n");
		print(" 3. Exit to Shell\n");
		print("========================================\n");
		print("Select option: ");

		read_line(buf, sizeof(buf));

		if (str_eq(buf, "1") == 0)
			play_guess();
		else if (str_eq(buf, "2") == 0)
			play_hangman();
		else if (str_eq(buf, "3") == 0 || str_eq(buf, "exit") == 0)
			break;
	}
	clear_screen();
}

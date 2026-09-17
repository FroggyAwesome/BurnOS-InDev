TARGET = BurnOS
GDBADDR = tcp:localhost:1234
MEM = 24M
FW = BIOS
AS = as
CC = gcc
CF = clang-format
ASFLAGS = --32
INCLUDES = -Iconfig -Ikernel -Idrivers -Iarch/x86 -Iprograms -Imodules -Ifs
CFILES = $(wildcard kernel/*.c) $(wildcard drivers/*.c) $(wildcard programs/*.c) $(wildcard modules/*.c) $(wildcard fs/*.c)
HFILES = $(wildcard kernel/*.h) $(wildcard drivers/*.h) $(wildcard programs/*.h) $(wildcard modules/*.h) $(wildcard fs/*.h)
CONFIGFILES = $(wildcard config/*.h)
NO_BUILTINS = -ffreestanding -fno-pie -fno-stack-protector -fno-builtin -fno-common -fno-unwind-tables -fno-asynchronous-unwind-tables
NO_SIMD = -mno-sse -mno-mmx -mno-sse2
STD = -std=gnu99
COMMON_CFLAGS = $(STD) -pipe -m32 $(NO_BUILTINS) $(NO_SIMD) -fno-strict-aliasing -MMD -MP $(INCLUDES)
CFLAGS = $(COMMON_CFLAGS) -Os -ffunction-sections -fdata-sections -fomit-frame-pointer
CDEBUGFLAGS = $(COMMON_CFLAGS) -O0 -g -fno-omit-frame-pointer
FLAGS ?= $(CDEBUGFLAGS)
CHECK_BASE = $(STD) -m32 $(NO_BUILTINS) $(NO_SIMD) -fsyntax-only $(INCLUDES)
CHECK_WARNINGS = -Wall -Wextra -Wpedantic -Werror -Wshadow -Wundef -Wwrite-strings \
                 -Wcast-align -Wpointer-arith -Wstrict-prototypes -Wredundant-decls -Wnested-externs \
                 -Wcast-qual -Wmissing-prototypes -Wstrict-overflow=2 -Wvla -Winit-self \
                 -Wnull-dereference -Wold-style-definition -Wmissing-declarations
CCHECKFLAGS = $(CHECK_BASE) $(CHECK_WARNINGS) $(CFILES)
CFORMATFLAGS = -i $(CFILES) $(HFILES) $(CONFIGFILES)
EMU = qemu-system-i386
EMUFLAGS = -enable-kvm -cpu host -smp 1 -m $(MEM) -net none -nodefaults -machine pc -bios $(FW) -boot d -vga std
OBJS = objs/arch/x86/boot.o $(patsubst %.c,objs/%.o,$(CFILES))
.PHONY: all debug build format check iso run run-debug gdb clean-objs clean-bin clean-grub clean-os distclean
all: debug
bin/kernel.bin: $(OBJS)
	@mkdir -p bin/
	ld -m elf_i386 -T arch/x86/linker.ld -o $@ $^
debug: FLAGS = $(CDEBUGFLAGS)
debug: bin/kernel.bin
build: FLAGS = $(CFLAGS)
build: check format distclean bin/kernel.bin
objs/arch/x86/boot.o: arch/x86/boot.s
	@mkdir -p objs/arch/x86
	$(AS) $(ASFLAGS) $< -o $@
objs/kernel/%.o: kernel/%.c
	@mkdir -p objs/kernel
	$(CC) $(FLAGS) -c $< -o $@
objs/drivers/%.o: drivers/%.c
	@mkdir -p objs/drivers
	$(CC) $(FLAGS) -c $< -o $@
objs/programs/%.o: programs/%.c
	@mkdir -p objs/programs
	$(CC) $(FLAGS) -c $< -o $@
objs/modules/%.o: modules/%.c
	@mkdir -p objs/modules
	$(CC) $(FLAGS) -c $< -o $@
objs/fs/%.o: fs/%.c
	@mkdir -p objs/fs
	$(CC) $(FLAGS) -c $< -o $@
format:
	$(CF) $(CFORMATFLAGS)
check:
	$(CC) $(CCHECKFLAGS)
iso:
	@mkdir -p out/
	cp bin/kernel.bin iso/boot/kernel.bin
	grub-mkrescue -o out/$(TARGET).iso iso
run:
	$(EMU) $(EMUFLAGS) -cdrom out/$(TARGET).iso
run-debug:
	$(EMU) $(EMUFLAGS) -gdb $(GDBADDR) -S -cdrom out/$(TARGET).iso
clean-objs:
	rm -rf objs/
clean-bin:
	rm -rf bin/
clean-grub:
	rm -rf iso/boot/*.bin
clean-os:
	rm -rf out/
distclean: clean-objs clean-bin clean-grub clean-os
-include $(OBJS:.o=.d)

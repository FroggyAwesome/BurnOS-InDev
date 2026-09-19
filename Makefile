# Project & Version Configuration
TARGET = BurnOS
VERSION = 1
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION = -indev
FULL_VERSION = $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)

# Toolchain & Architecture Settings
GDBADDR = tcp:localhost:1234
MEM = 4M
FW = BIOS
AS = as
CC = gcc
CF = clang-format
ASFLAGS = --32

# Paths & File Discovery
INCLUDES = -Iconfig -Ikernel -Idrivers -Iarch/x86 -Iprograms -Imodules -Ifs
CFILES = $(wildcard kernel/*.c) $(wildcard drivers/*.c) $(wildcard programs/*.c) $(wildcard modules/*.c) $(wildcard fs/*.c)
HFILES = $(wildcard kernel/*.h) $(wildcard drivers/*.h) $(wildcard programs/*.h) $(wildcard modules/*.h) $(wildcard fs/*.h)
CONFIGFILES = $(wildcard config/*.h)

# Compiler Flags & Warnings
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

# QEMU Emulator Configuration
EMU = qemu-system-i386
EMULOGFILE = qemu.log
EMULOG = -d int,cpu_reset -D $(EMULOGFILE)
EMUCPU = -cpu pentium -smp 1
EMUFLAGS = $(EMUCPU) -m $(MEM) -net none -nodefaults -machine pc -bios $(FW) -boot d -vga std

# Object Files
OBJS = objs/arch/x86/boot.o objs/arch/x86/isr.o $(patsubst %.c,objs/%.o,$(CFILES))

# PHONY Targets Declaration
.PHONY: all debug build format check get-scripts iso run run-debug version clean-objs clean-bin clean-grub clean-os clean-scripts clean-log distclean

# Main Build Targets
all: check format bin/kerneldbg.bin bin/kernelstd.bin
bin/kerneldbg.bin: $(OBJS)
	@mkdir -p bin/
	ld -m elf_i386 -T arch/x86/linker.ld -o $@ $^
bin/kernelstd.bin: $(OBJS)
	@mkdir -p bin/
	ld -m elf_i386 -T arch/x86/linker.ld -o $@ $^
debug: FLAGS = $(CDEBUGFLAGS)
debug: bin/kerneldbg.bin
build: FLAGS = $(CFLAGS)
build: check format distclean bin/kernelstd.bin

# Compilation Pattern Rules
objs/arch/x86/boot.o: arch/x86/boot.s
	@mkdir -p objs/arch/x86
	$(AS) $(ASFLAGS) $< -o $@
objs/arch/x86/isr.o: arch/x86/isr.s
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

# Development & Utility Tasks
format:
	$(CF) $(CFORMATFLAGS)
check:
	$(CC) $(CCHECKFLAGS)
get-scripts:
	@for f in scripts/*; do \
		[ -f "$$f" ] && cp -v "$$f" . && chmod +x $$(basename "$$f"); \
	done

# ISO & Emulation Targets
iso:
	@mkdir -p out/
	@if [ -f bin/kerneldbg.bin ] && [ -f bin/kernelstd.bin ]; then \
		cp bin/kerneldbg.bin iso/boot/kerneldbg.bin; \
		cp bin/kernelstd.bin iso/boot/kernelstd.bin; \
	else \
		echo "Error: No kernel binary found. Run 'make debug' or 'make build' first."; \
		exit 1; \
	fi
	grub-mkrescue -o out/$(TARGET).iso iso
run:
	$(EMU) $(EMUFLAGS) -cdrom out/$(TARGET).iso
run-debug:
	$(EMU) $(EMUFLAGS) -gdb $(GDBADDR) -S $(EMULOG) -cdrom out/$(TARGET).iso

# Version Management
version:
	@echo "Updating project name to $(TARGET)..."
	@sed -i "s/#define NAME \".*\"/#define NAME \"$(TARGET)\"/" config/os.h
	@echo "Project name updated successfully."
	@echo "Updating project version to v$(FULL_VERSION)..."
	@sed -i "s/#define VERSION \".*\"/#define VERSION \"v$(FULL_VERSION)\"/" config/os.h
	@sed -i "s/\* \*\*Version\*\*: \*.*/\* \*\*Version\*\*: *v$(FULL_VERSION)*/" doc/STATUS.md
	@echo "Version updated successfully."

# Cleanup Targets
clean-objs:
	rm -rf objs/
clean-bin:
	rm -rf bin/
clean-grub:
	rm -f iso/boot/*.bin
clean-os:
	rm -rf out/
clean-scripts:
	@for f in scripts/*; do \
		[ -e "$$f" ] || continue; \
		base=$$(basename "$$f"); \
		if [ -f "$$base" ]; then \
			rm -v "$$base"; \
		fi \
	done
clean-log:
	rm -f $(EMULOGFILE)
distclean: clean-log clean-objs clean-bin clean-grub clean-os clean-scripts

# Automatic Dependencies
-include $(OBJS:.o=.d)

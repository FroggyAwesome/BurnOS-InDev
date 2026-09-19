# BurnOS Makefile Documentation

This document describes the available targets and workflow automation implemented in the project's `Makefile`.

## Overview
The `Makefile` controls the entire build process, toolchain configuration, code formatting, static analysis, QEMU execution, and version management for BurnOS.

---

## Main Build Targets
* **`make` or `make all`**
  Runs static analysis (`check`), formats the codebase (`format`), and builds both debugging (`bin/kerneldbg.bin`) and standard (`bin/kernelstd.bin`) kernel binaries.
* **`make debug`**
  Compiles the kernel specifically in debug mode (`-O0 -g`), using `CDEBUGFLAGS`.
* **`make build`**
  Performs a clean release build. It runs checks, formats code, cleans previous artifacts (`distclean`), and compiles the standard optimized kernel (`-Os`).

---

## ISO & Emulation Targets
* **`make iso`**
  Packages the compiled kernel binaries from `bin/` into a bootable ISO image located at `out/BurnOS.iso` using `grub-mkrescue`.
* **`make run`**
  Boots the generated ISO image inside the QEMU emulator using predefined configuration flags.
* **`make run-debug`**
  Boots QEMU in debugging mode, halting the CPU on startup (`-S`) and exposing a GDB server socket on the configured address.

---

## Version Management
* **`make version`**
  Automatically updates the project's name (based on the `TARGET` variable) and its version across `config/os.h` and `doc/STATUS.md` using the configuration values defined at the top of the `Makefile` (`VERSION`, `PATCHLEVEL`, `SUBLEVEL`, `EXTRAVERSION`).

---

## Development & Utility Tasks
* **`make check`**
  Runs strict static analysis and syntax checks across all C source files using GCC.
* **`make format`**
  Automatically formats all source and header files using `clang-format` according to project rules.
* **`make get-scripts`**
  Copies helpful utility scripts from the `scripts/` directory directly into the project root and grants them execution permissions (`chmod +x`).

---

## Cleanup Targets
* **`make clean-objs`** -> Removes the object files directory (`objs/`).
* **`make clean-bin`** -> Removes compiled binaries (`bin/`).
* **`make clean-grub`** -> Cleans kernel binaries inside the ISO folder.
* **`make clean-os`** -> Removes output ISO images (`out/`).
* **`make clean-scripts`** -> Removes local copies of scripts synced from `scripts/`.
* **`make clean-log`** -> Deletes QEMU log files (`qemu.log`).
* **`make distclean`** -> Executes all cleanup targets at once for a fresh workspace reset.

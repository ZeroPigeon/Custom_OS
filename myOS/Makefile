# Toolchain
CC=i686-elf-gcc
LD=i686-elf-gcc

# Compiler flags
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra

# Source files
SRC=\
boot/boot.s \
kernel/kernel.c \
kernel/terminal.c \
kernel/keyboard.c \
kernel/shell.c

# Object files
OBJ=$(patsubst %.c,%.o,$(SRC))
OBJ:=$(patsubst %.s,%.o,$(OBJ))

# Final kernel binary
KERNEL=myOS

all: $(KERNEL)

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble ASM files
%.o: %.s
	$(CC) -c $< -o $@

# Link kernel
$(KERNEL): $(OBJ)
	$(LD) -T linker.ld -o $(KERNEL) -ffreestanding -nostdlib $(OBJ) -lgcc

# Build ISO
iso: $(KERNEL)
	mkdir -p iso/boot/grub
	cp $(KERNEL) iso/boot/myOS
	cp grub.cfg iso/boot/grub/
	grub-mkrescue -o myOS.iso iso

# Run in QEMU
run: iso
	qemu-system-i386 -cdrom myOS.iso

# Clean build files
clean:
	rm -rf *.o
	rm -rf kernel/*.o
	rm -rf boot/*.o
	rm -rf iso
	rm -f $(KERNEL)
	rm -f myOS.iso

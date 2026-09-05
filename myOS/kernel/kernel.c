#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "keyboard.h"
#include "shell.h"
#include "terminal.h"
#include "filesystem.h"

/* Check if the compiler thinks you are targeting the wrong operating system */
#if defined(_linux_)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

Directory* current_directory;

Directory root_directory;

#define HEAP_SIZE 1024 * 1024

static unsigned char heap[HEAP_SIZE];

static int heap_top = 0;

void* memcpy(void* dest, const void* src, int size)
{

	unsigned char* d = dest;
	const unsigned char* s = src;

	for (int i = 0; i < size; i++)
	{

		d[i] = s[i];

	}

	return dest;
}

void* kmalloc(int size)
{

	heap_top = (heap_top + 7) & ~7;

	if (heap_top + size >= HEAP_SIZE)
                return NULL;

	void* ptr = &heap[heap_top];
	heap_top += size;

	return ptr;

}

void* krealloc(void* old_ptr, int old_size, int new_size)
{
	void* new_ptr = kmalloc(new_size);

	int copy_size = old_size;

	if (new_size < old_size)
	{

		copy_size = new_size;

	}

	memcpy(new_ptr, old_ptr, copy_size);

	return new_ptr;
}

void initialize_directory(void)
{
	strcpy(root_directory.name, "r");

	root_directory.parent = NULL;

	root_directory.file_count = 0;
	root_directory.file_capacity = 4;
	root_directory.files = kmalloc(sizeof(File) * root_directory.file_capacity);

	root_directory.dir_count = 0;
	root_directory.dir_capacity = 4;
	root_directory.directories = kmalloc(sizeof(Directory) * root_directory.dir_capacity);

	current_directory = &root_directory;
}

void create_dir(char* name)
{

	if (current_directory->dir_count >= current_directory->dir_capacity)
	{

		int old_capacity = current_directory->dir_capacity;

		current_directory->dir_capacity *= 2;

		current_directory->directories = krealloc(current_directory->directories, sizeof(Directory) * old_capacity, sizeof(Directory) * current_directory->dir_capacity);

	}

	Directory* dir = &current_directory->directories[current_directory->dir_count];

	strcpy(dir->name, name);

	dir->parent = current_directory;

	dir->file_count = 0;
	dir->file_capacity = 4;

	dir->dir_count = 0;
	dir->dir_capacity = 4;

	dir->files = kmalloc(sizeof(File) * dir->file_capacity);
	dir->directories = kmalloc(sizeof(Directory) * dir->dir_capacity);

	current_directory->dir_count++;

	terminal_writestring("\n\nDirectory created => ");
	terminal_writestring(current_directory->name);
	terminal_putchar('/');
	terminal_writestring(dir->name);
	terminal_writestring("\n\n");
}

int search_for_dir(char* dirname)
{

        for (int i = 0; i < current_directory->dir_count; i++)
        {
                if (strcmp(current_directory->directories[i].name, dirname) == 0)
		{
                        return i;
        	}
	}

        terminal_writestring_color("\nNo such directory found\n", vga_entry_color(VGA_COLOR_RED, VGA_COLOR_BLACK));
        return -1;

}

void list_dirs(void)
{

	if (current_directory->dir_count > 0)
	{
	        terminal_putchar('\n');
		terminal_putchar('\n');

	        terminal_setcolor(vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK));

        	for (int i = 0; i < current_directory->dir_count; i++)
        	{
                	        terminal_writestring(current_directory->name);
				terminal_putchar('/');
	                        terminal_writestring(current_directory->directories[i].name);
        	                terminal_putchar(' ');
        	}

	        terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

		terminal_putchar('\n');
	}
}

void change_directory(char* name)
{

	if (strcmp(name, "..") == 0)
	{
		if (current_directory->parent)
			current_directory = current_directory->parent;

		return;
	}

	for (int i = 0; i < current_directory->dir_count; i++)
	{

		if (strcmp(current_directory->directories[i].name, name) == 0)
		{
			current_directory = &current_directory->directories[i];

			return;
		}
	}

	terminal_writestring_color("\nDirectory not found\n", vga_entry_color(VGA_COLOR_RED, VGA_COLOR_BLACK));
	terminal_putchar('\n');
}

void create_file(char* name)
{

        if (current_directory->file_count >= current_directory->file_capacity)
	{

		int old_capacity = current_directory->file_capacity;

		current_directory->file_capacity *= 2;

		current_directory->files = krealloc(current_directory->files, sizeof(File) * old_capacity, sizeof(File) * current_directory->file_capacity);

	}


        strcpy(current_directory->files[current_directory->file_count].name, name);

        current_directory->files[current_directory->file_count].size = 0;
        current_directory->files[current_directory->file_count].data[0] = '\0';

	terminal_putchar('\n');
	terminal_putchar('\n');
        terminal_writestring("File created => ");
        terminal_writestring(current_directory->files[current_directory->file_count].name);
        terminal_writestring(".txt");
	terminal_putchar('\n');
	terminal_putchar('\n');

        current_directory->file_count++;

}

void list_files(void)
{

	if (current_directory->file_count > 0)
	{

	        terminal_putchar('\n');
		terminal_putchar('\n');

		terminal_setcolor(vga_entry_color(VGA_COLOR_BLUE, VGA_COLOR_BLACK));

        	for (int i = 0; i < current_directory->file_count; i++)
        	{
                	        terminal_writestring(current_directory->files[i].name);
				terminal_writestring(".txt");
	                        terminal_putchar(' ');
        	}

		terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
		terminal_putchar('\n');
	}
}

int search_for_file(char* filename)
{

	for (int i = 0; i < current_directory->file_count; i++)
        {
		if (strcmp(current_directory->files[i].name, filename) == 0)
			return i;
	}

	terminal_writestring_color("\nNo such file found\n", vga_entry_color(VGA_COLOR_RED, VGA_COLOR_BLACK));
	return -1;

}

void write_to_file(int fileindex, char* data)
{

	if (fileindex == -1) return;

	strcpy(current_directory->files[fileindex].data, data);
	current_directory->files[fileindex].size = strlen(data);
	terminal_writestring("\nContent written to file => ");
	terminal_writestring(current_directory->files[fileindex].name);
	terminal_writestring(".txt");
	terminal_putchar('\n');

}

void append_to_file(int fileindex, char* data)
{

	if (fileindex == -1) return;

	int i = 0;

	while (current_directory->files[fileindex].data[i])
		i++;

	int j = 0;
	while (data[j] && i < 256 - 1)
	{
		current_directory->files[fileindex].data[i++] = data[j++];
	}

	current_directory->files[fileindex].data[i] = '\0';
	current_directory->files[fileindex].size = i;

	terminal_writestring("\nContent appended to file => ");
	terminal_writestring(current_directory->files[fileindex].name);
	terminal_writestring(".txt");
	terminal_putchar('\n');

}

void read_file(int fileindex)
{
	if (fileindex == -1) return;

	terminal_putchar('\n');
	terminal_writestring(current_directory->files[fileindex].data);
	terminal_putchar('\n');

}

void kernel_main(void)
{

	/* Initialize terminal interface */
	terminal_initialize();
	initialize_directory();

	/* Newline support is left as an exercies. */
	terminal_writestring_color("                       Welcome to the terminal of this OS\n", vga_entry_color(VGA_COLOR_BLUE, VGA_COLOR_BLACK));
	draw_prompt();

	while (1)
	{
		keyboard_handler();
	}
}

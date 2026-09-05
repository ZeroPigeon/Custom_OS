#include "shell.h"
#include "terminal.h"
#include "keyboard.h"
#include "filesystem.h"

size_t strlen(const char* str)
{
        size_t len = 0;
        while (str[len])
                len++;
        return len;
}

static inline void outw(uint16_t port, uint16_t value)
{
        asm volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

void shutdown(void)
{
        asm volatile ("cli");

        outw(0x604, 0x2000);

        while (1)
        {
                asm volatile ("hlt");
        }
}

void reboot(void)
{
        uint8_t status;

        asm volatile ("cli");

        do
        {
                status = inb(0x64);
        }
        while (status & 0x02);

        outb(0x64, 0xFE);

        while (1)
        {
                asm volatile ("hlt");
        }
}

int strcmp(const char* a, const char* b)
{
        while (*a && *b)
        {
                if (*a != *b)
                        return 1;

                a++;
                b++;
        }

        return *a != *b;

}

int strncmp(const char* a, const char* b, int n)
{
        while (n && *a && *b)
        {
                if (*a != *b)
                        return 1;

                a++;
                b++;
                n--;
        }

        return 0;

}

void strcpy(char* dest, const char* src)
{

        while (*src)
        {

                *dest = *src;

                dest++;
                src++;

        }

        *dest = '\0';

}

void strcat(char* dest, const char* src)
{
	while (*dest)
	{
		dest++;
	}

	while (*src)
	{
		*dest = *src;

		dest++;
		src++;
	}

	*dest = '\0';
}

int chkstr(const char* source, const char* target)
{

	int i = 0;

	while (source[i])
	{
		int j = 0;

		while (target[j] && source[i + j] && source[i + j] == target[j])
			j++;

		if (target[j] == '\0')
			return i;

		i++;

	}

	return -1;

}

void process_command(char* command)
{

	if (command[0] == '\0')
	{
		terminal_putchar('\n');
		return;
	}
	else if (strcmp(command, "help") == 0)
        {
                terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK));
                terminal_writestring("\n\nCommands:\n");
                terminal_writestring("help\n");
                terminal_writestring("ls {dir/file}\n");
                terminal_writestring("echo\n");
                terminal_writestring("clear\n");
                terminal_writestring("mkfile\n");
                terminal_writestring("mkdir\n");
                terminal_writestring("cd\n");
		terminal_writestring("write\n");
		terminal_writestring("read\n");
                terminal_writestring("reboot\n");
                terminal_writestring("shutdown\n\n");
                terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
        }
        else if (strcmp(command, "clear") == 0)
        {

                terminal_initialize();
                terminal_writestring_color("                       Welcome to the terminal of this OS\n", vga_entry_color(VGA_COLOR_BLUE, VGA_COLOR_BLACK));

        }
        else if (strcmp(command, "shutdown") == 0)
        {
                terminal_writestring_color("\n\nShutting down...\n", vga_entry_color(VGA_COLOR_RED, VGA_COLOR_BLACK));
                shutdown();
        }
        else if (strcmp(command, "reboot") == 0)
        {
                terminal_writestring_color("\n\nRebooting...\n", vga_entry_color(VGA_COLOR_RED, VGA_COLOR_BLACK));
                reboot();
        }
	else if (strcmp(command, "ls") == 0)
        {
                list_dirs();
                list_files();
		terminal_putchar('\n');
        }
        else if (strcmp(command, "ls dir") == 0)
        {
                list_dirs();
                terminal_putchar('\n');
        }
        else if (strcmp(command, "ls file") == 0)
        {
		list_files();
                terminal_putchar('\n');
	}
        else if (strncmp(command, "echo ", 5) == 0)
        {
                terminal_putchar('\n');

                char* data = command + 5;

                terminal_writestring_color(data, vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK));

                terminal_putchar('\n');

        }
        else if (strncmp(command, "mkfile ", 7) == 0)
        {
                char* filename = command + 7;
                create_file(filename);
        }
        else if (strncmp(command, "mkdir ", 6) == 0)
        {
                char* dirname = command + 6;
		create_dir(dirname);
         }
        else if (strncmp(command, "write ", 6) == 0)
        {

		int append = 0;

		int pos = chkstr(command, ">>");

		if (pos != -1)
		{
			append = 1;
		}
		else
		{
			pos = chkstr(command, ">");
		}

		if (pos == -1)
		{
			terminal_writestring("\nMissing > or >>\n");
			return;
		}

                char content[256];
                char filename[64];

		int i;
		for (i = 6; i < pos - 1; i++)
		{
			content[i - 6] = command[i];
		}
		content[i - 6] = ' ';
		content[i - 5] = '\0';

		int operator_length = append ? 2 : 1;

		int j = 0;
		for (i = pos + operator_length; command[i]; i++)
		{
			if (command[i] == ' ' && j == 0)
				continue;

			filename[j++] = command[i];
		}
		filename[j] = '\0';

		int fileindex = search_for_file(filename);

		if (append)
		{
			append_to_file(fileindex, content);
		}
		else
		{
			write_to_file(fileindex, content);
		}

        }
	else if (strncmp(command, "read ", 5) == 0)
	{
		char* filename = command + 5;

		read_file(search_for_file(filename));
	}
	else if (strncmp(command, "cd ", 3) == 0)
	{
		char* newDir = command + 3;

		change_directory(newDir);
		terminal_putchar('\n');
	}
        else
        {

                terminal_writestring_color("\n\nUnknown command\n\n", vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));

        }

}

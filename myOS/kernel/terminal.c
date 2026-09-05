#include "terminal.h"
#include "keyboard.h"
#include "shell.h"
#include "filesystem.h"

#define VGA_MEMORY      0xB8000

int input_start_row;
int input_start_column;

int prompt_start_row;
int prompt_start_column;

uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
        return fg | bg << 4;
}

uint16_t vga_entry(unsigned char uc, uint8_t color)
{
        return (uint16_t) uc | (uint16_t) color << 8;
}


size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

void terminal_initialize(void)
{
        terminal_row = 0;
        terminal_column = 0;
        terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

        for (size_t y = 0; y < VGA_HEIGHT; y++) {
                for (size_t x = 0; x < VGA_WIDTH; x++) {
                        const size_t index = y * VGA_WIDTH + x;
                        terminal_buffer[index] = vga_entry(' ', terminal_color);
                }
        }
}

void terminal_setcolor(uint8_t color)
{
        terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
        const size_t index = y * VGA_WIDTH + x;
        terminal_buffer[index] = vga_entry(c, color);
}

void draw_char_at(int x, int y, char c, uint8_t color)
{
	terminal_buffer[y * VGA_WIDTH + x] = vga_entry(c, color);
}

void terminal_scroll(void)
{
	for (int y = 1; y < VGA_HEIGHT; y++)
        {
        	for (int x = 0; x < VGA_WIDTH; x++)
                {
                	terminal_buffer[(y - 1) * VGA_WIDTH + x] = terminal_buffer[y * VGA_WIDTH + x];
		}
	}

	for (int x = 0; x < VGA_WIDTH; x++)
        {
        	terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
        }

        terminal_row = VGA_HEIGHT - 1;

	update_cursor(terminal_column, terminal_row);

}

void terminal_putchar(char c)
{
        if (c == '\n')
        {

                terminal_row++;
                terminal_column = 0;

                if (terminal_row >= VGA_HEIGHT)
                {
			terminal_scroll();
		}

        	return;

        }

       terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

       if (++terminal_column >= VGA_WIDTH) {

		terminal_column = 0;

		terminal_row++;

                if (terminal_row >= VGA_HEIGHT) {
        		terminal_scroll();
		}
	}
}

void terminal_write(const char* data, size_t size)
{
        for (size_t i = 0; i < size; i++)
                terminal_putchar(data[i]);
}


void terminal_writestring(const char* data)
{
        terminal_write(data, strlen(data));
	update_cursor(terminal_column, terminal_row);
}

void terminal_writestring_color(const char* data, uint8_t color)
{

        uint8_t old_color = terminal_color;

        terminal_setcolor(color);

        terminal_writestring(data);

        terminal_setcolor(old_color);

	update_cursor(terminal_column, terminal_row);

}

void draw_prompt(void)
{
	prompt_start_row = terminal_row;
	prompt_start_column = terminal_column;

	terminal_writestring(" ");
        terminal_writestring(current_directory->name);
        terminal_writestring(" > ");

        input_start_row = terminal_row;
        input_start_column = terminal_column;
}

void clear_current_row(void)
{

	for (int x = 0; x < VGA_WIDTH; x++)
	{
			draw_char_at(x, input_start_row, ' ', terminal_color);
	}

}

void redraw_input_line(void)
{

	clear_current_row();

	char prompt[64];

	strcpy(prompt, " ");
        strcat(prompt, current_directory->name);
        strcat(prompt, " > ");

	int x = prompt_start_column;
	int y = prompt_start_row;

	for (int i = 0; prompt[i]; i++)
	{
		draw_char_at(x, y, prompt[i], terminal_color);

		x++;

		if (x >= VGA_WIDTH)
		{
			x = 0;
			y++;
		}
	}

	for (int i = 0; i < input_length; i++)
	{
		draw_char_at(x, y, input_buffer[i], terminal_color);

		x++;

		if (x >= VGA_WIDTH)
		{
			x = 0;
			y++;
		}
	}

	int total_position = strlen(prompt) + cursor_index;

	int cursor_row = input_start_row + (total_position / VGA_WIDTH);
	int cursor_column = total_position % VGA_WIDTH;

	update_cursor(cursor_column, cursor_row);
}

#include "keyboard.h"
#include "terminal.h"
#include "shell.h"
#include "filesystem.h"

int shift_pressed = 0;
int extended_key = 0;

size_t terminal_columnMax = 0;

char input_buffer[256];
int input_length;
int cursor_index;

void outb(uint16_t port, uint8_t value)
{
        asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t inb(uint16_t port)
{
        uint8_t ret;

        asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));

        return ret;
}

	char keyboard_map[128] = {
        0,
        27, // ESC
        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
        '-', '=',
        '\b', // Backspace
        '\t', // Tab

        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
        '[', ']',
        '\n', // Enter

        0,    // Ctrl

        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
        ';', '\'', '`',

        0, // Left Shift
        '\\',

        'z', 'x', 'c', 'v', 'b', 'n', 'm',
        ',', '.', '/',

        0, // Right Shift
        '*',

        0, // Alt
        ' '// Space
};

        char keyboard_map_shift[128] = {
        0,
        27,
        '!','@','#','$','%','^','&','*','(',')',
        '_','+',
        '\b','\t',

        'Q','W','E','R','T','Y','U','I','O','P',
        '{','}',
        '\n',

        0,

        'A','S','D','F','G','H','J','K','L',
        ':','"','~',

        0,
        '|',

        'Z','X','C','V','B','N','M',
        '<','>','?',

        0,
	'*',

	0,
	' '
};

void update_cursor(int x, int y)
{
        uint16_t pos = y * 80 + x;

        outb(0x3D4, 0x0F);
        outb(0x3D5, (uint8_t)(pos & 0xFF));

        outb(0x3D4, 0x0E);
        outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void terminal_backspace(void)
{
        if (terminal_column == 0) // **** At the moment ensures that shell prompt '>' can't be deleted and can't go up rows, can fix later
                return;

        terminal_column--;

        if (input_length > 0)
        {
                input_length--;
                cursor_index--;
        }

        draw_char_at(terminal_column, terminal_row, ' ', terminal_color);

        if (cursor_index < input_length)
        {

                for (int i = cursor_index; i < input_length; i++)
                {

                        input_buffer[i] = input_buffer[i + 1];

                }
        }

        redraw_input_line();

}

void keyboard_handler(void)
{
        if (!(inb(0x64) & 1))
                return;

        uint8_t scancode = inb(0x60);

        if (scancode == 0xE0)
        {

                extended_key = 1;
                return;

        }

        if (extended_key)
        {
                extended_key = 0;

                switch(scancode)
                {
                        case 0x4B:
                                if (cursor_index == 0)
                                        return;
                                if (terminal_column > 0)
                                {
                                        terminal_column--;
                                        cursor_index--;
                                }

                                update_cursor(terminal_column, terminal_row);
                                //Move cursor left;
                        break;
                        case 0x4D:
                                if (terminal_column == terminal_columnMax)
                                        return;
                                if (terminal_column < VGA_WIDTH - 1)
                                {
                                        terminal_column++;
                                        cursor_index++;
                                }

                                update_cursor(terminal_column, terminal_row);
                                //Move cursor right;
                        break;
                }

                return;
        }

        if (scancode == 0x2A || scancode == 0x36)
        {
                shift_pressed = 1;
        }

        if (scancode == 0xAA || scancode == 0xB6)
        {
                shift_pressed = 0;
        }

        if (scancode & 0x80)
                return;

        char c;

        if (shift_pressed)
             c = keyboard_map_shift[scancode];
        else
             c = keyboard_map[scancode];

        if (c == '\b')
        {
                if (input_length > 0)
                {
                        terminal_backspace();
                        terminal_columnMax--;
                }
        }
        else if (c == '\n')
        {
                input_buffer[input_length] = '\0';

		process_command(input_buffer);

        	draw_prompt();

		input_length = 0;
                cursor_index = 0;

                terminal_columnMax = strlen(current_directory->name) + 4;

        }
        else if (c)
        {
                if (cursor_index < input_length)
                {
                        for (int i = input_length; i > cursor_index; i--)
                        {
                                input_buffer[i] = input_buffer[i - 1];
                        }
                }

                input_buffer[cursor_index++] = c;
                input_length++;
                terminal_columnMax++;
                redraw_input_line();
        }

        outb(0x20, 0x20);

}

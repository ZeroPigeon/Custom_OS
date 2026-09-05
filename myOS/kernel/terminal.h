#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

/* Hardware text mode color constants. */
enum vga_color {
        VGA_COLOR_BLACK = 0,
        VGA_COLOR_BLUE = 1,
        VGA_COLOR_GREEN = 2,
        VGA_COLOR_CYAN = 3,
        VGA_COLOR_RED = 4,
        VGA_COLOR_MAGENTA = 5,
        VGA_COLOR_BROWN = 6,
        VGA_COLOR_LIGHT_GREY = 7,
        VGA_COLOR_DARK_GREY = 8,
        VGA_COLOR_LIGHT_BLUE = 9,
        VGA_COLOR_LIGHT_GREEN = 10,
        VGA_COLOR_LIGHT_CYAN = 11,
        VGA_COLOR_LIGHT_RED = 12,
        VGA_COLOR_LIGHT_MAGENTA = 13,
        VGA_COLOR_LIGHT_BROWN = 14,
        VGA_COLOR_WHITE = 15,
};

extern size_t terminal_row;
extern size_t terminal_column;
extern uint8_t terminal_color;

uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);
uint16_t vga_entry(unsigned char uc, uint8_t color);
void terminal_initialize(void);
void terminal_putchar(char c);
void draw_char_at(int x, int y, char c, uint8_t color);
void terminal_writestring(const char* data);
void terminal_writestring_color(const char* data, uint8_t color);
void terminal_setcolor(uint8_t color);
void draw_prompt(void);
void clear_current_row(void);
void redraw_input_line(void);


#endif

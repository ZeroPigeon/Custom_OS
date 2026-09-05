#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stddef.h>
#include <stdint.h>

extern char input_buffer[256];
extern int input_length;
extern int cursor_index;

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
void update_cursor(int x, int y);
void terminal_backspace(void);
void keyboard_handler(void);

#endif

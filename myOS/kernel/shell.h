#ifndef SHELL_H
#define SHELL_H

#include <stddef.h>
#include <stdint.h>

size_t strlen(const char* str);
int strcmp(const char* a, const char* b);
void strcpy(char* dest, const char* src);
void strcat(char* dest, const char* src);
void process_command(char* command);

#endif

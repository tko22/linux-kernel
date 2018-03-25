#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

// hubert

extern unsigned char getScanCode();
extern unsigned char getChar(unsigned char character);
extern void handle_keyboard_interrupt();
void update_cursor(int row, int col);
int32_t keyboard_open();
int32_t keyboard_write(int32_t fd, char *string, int32_t length);
int32_t keyboard_read(int32_t fd, char *string, int32_t length);
void update_boundaries();
void enable_cursor();
#endif

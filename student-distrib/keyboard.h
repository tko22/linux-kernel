#ifndef _KEYBOARD_H
#define _KEYBOARD_H

// hubert

extern unsigned char getScanCode();
extern unsigned char getChar(unsigned char character);
extern void handle_keyboard_interrupt();
void update_cursor(int row, int col);
void read(char *string, int length);
void update_boundaries();
void enable_cursor();
#endif

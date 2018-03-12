#ifndef _KEYBOARD_H
#define _KEYBOARD_H

// hubert

extern unsigned char getScanCode();
extern unsigned char getChar(unsigned char character);
extern void handle_keyboard_interrupt();
void update_cursor(int row, int col);
void enable_cursor();
#endif

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

// hubert

extern unsigned char getScanCode();
extern unsigned char getChar(unsigned char character);
extern void handle_keyboard_interrupt();
void update_cursor(int row, int col);
int open(char *m);
int close();
int write(void *string, int length);
int read(void *string, int length);
void update_boundaries();
void enable_cursor();
#endif

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

// hubert

extern unsigned char getScanCode();
extern unsigned char getChar(unsigned char character);
extern void handle_keyboard_interrupt();
void update_cursor(int row, int col);
int keyboard_open(char *m);
int keyboard_close();
int keyoard_write(char *string, int length);
int keyboard_read(char *string, int length);
void update_boundaries();
void enable_cursor();
#endif

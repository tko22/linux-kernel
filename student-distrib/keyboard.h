#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"
#include "file_desc.h"

// hubert

extern unsigned char getScanCode();
extern unsigned char getChar(unsigned char character);
extern void handle_keyboard_interrupt();
void update_cursor(int row, int col);
int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(int32_t fd);
int32_t terminal_write(int32_t fd, char *string, int32_t length);
int32_t terminal_read(int32_t fd, char *string, int32_t length);
int32_t invalid_func();
void update_boundaries();
void enable_cursor();

static struct file_ops_jump_table_t stdin_jump = {
    .open = terminal_open,
    .close = terminal_close,
    .write = invalid_func,
    .read = terminal_read
};

static struct file_ops_jump_table_t stdout_jump = {
    .open = terminal_open,
    .close = terminal_close,
    .write = terminal_write,
    .read = invalid_func
};

#endif

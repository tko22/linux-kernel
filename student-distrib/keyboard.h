#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"
#include "file_desc.h"
#include "sys.h"

// hubert

extern unsigned char getScanCode();
extern unsigned char getChar(unsigned char character);
extern void handle_keyboard_interrupt();
void update_cursor(int row, int col);
extern int32_t terminal_open(fd_t* a, const uint8_t* filename);
extern int32_t terminal_close(fd_t* fd);
extern int32_t terminal_write(fd_t *fd, const uint8_t* string, int32_t length);
extern int32_t terminal_read(fd_t* fd, uint8_t* string, int32_t length);
extern int32_t invalid_func();
void update_boundaries();
void enable_cursor();

static struct file_ops_jump_table_t stdin_jump __attribute__((unused))= {
    .open = terminal_open,
    .close = terminal_close,
    .write = invalid_func,
    .read = terminal_read
};

static struct file_ops_jump_table_t stdout_jump __attribute__((unused))= {
    .open = terminal_open,
    .close = terminal_close,
    .write = terminal_write,
    .read = invalid_func
};

#endif

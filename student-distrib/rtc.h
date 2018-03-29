#ifndef _RTC_H
#define _RTC_H

#include "file_desc.h"

// tae
extern void init_rtc();
extern void handle_rtc_interrupt();
int32_t open_rtc(fd_t* fd, uint8_t* filename);
int32_t close_rtc(fd_t* fd);
int32_t write_rtc(fd_t* fd, const void* buf, int32_t nbytes);
int32_t read_rtc(fd_t* fd, void* buf, int32_t nbytes);


static struct file_ops_jump_table_t rtc_jump = {
    .open = open_rtc,
    .close = close_rtc,
    .write = write_rtc,
    .read = read_rtc
};

#endif

#ifndef _RTC_H
#define _RTC_H

// tae
extern void init_rtc();
extern void handle_rtc_interrupt();
int32_t open_rtc(const uint8_t* filename);
int32_t close_rtc(const uint8_t* filename);
int32_t read_rtc(int32_t fd, void* buf, int32_t nbytes);
int32_t write_rtc(int32_t fd, const void* buf, int32_t nbytes);

typedef struct rtc_jmp_table = {
    open_rtc,
    close_rtc
    write_rtc,
    read_rtc,
};
#endif

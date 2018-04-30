#ifndef _SCHEDULING_H
#define _SCHEDULING_H
#include "types.h"
#include "file_desc.h"

#define FIFTY_HZ 0x5D38   //1193180/50 = 23864 = 0x5D38 

extern void initalize_PIT();
extern void handle_pit_interrupt();
extern void switch_proc();
uint32_t next_process(uint32_t curr);
void switch_terminal(uint32_t terminal_id);
void init_terminal_buf();

#endif

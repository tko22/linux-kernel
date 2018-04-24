#ifndef _SCHEDULING_H
#define _SCHEDULING_H
#include "types.h"
#include "file_desc.h"

#define ThirtyFIVE_HZ 0x852A   //1193180/35 = 34090 = 0x852A

extern void initalize_PIT();
extern void handle_pit_interrupt();
extern void switch_proc();
uint8_t next_process(uint8_t curr);
void switch_terminal(uint32_t terminal_id);


#endif

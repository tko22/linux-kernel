#ifndef _SCHEDULING_H
#define _SCHEDULING_H

extern void initalize_PIT();
extern void handle_pit_interrupt();
extern void switch_proc();
uint8_t next_process(uint8_t curr);
void switch_terminal(uint8_t terminal_id)

#endif

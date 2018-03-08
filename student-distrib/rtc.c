#include "rtc.h"
#include "lib.h"
// tae
#define cmos_addr 0x70
#define cmos_data 0x71

void init_rtc(){
  cli();		// disable interrupts
  outb(0x70, 0x8A);	// select Status Register A, and disable NMI (by setting the 0x80 bit)
  outb(0x71, 0x20);	// write to CMOS/RTC RAM
  sti();		// re-enable interrupts
}

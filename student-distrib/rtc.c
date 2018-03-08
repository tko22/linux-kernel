#include "rtc.h"
#include "lib.h"
// tae
#define cmos_addr 0x70
#define cmos_data 0x71

void init_rtc(){
  cli();		// disable interrupts
  outb(cmos_addr, 0x8A);	// select Status Register A, and disable NMI (by setting the 0x80 bit)
  outb(cmos_data, 0x20);	// write to CMOS/RTC RAM
  //NOT SURE vvvvvv
  outb(cmos_addr, 0x8B);		// select register B, and disable NMI
  char prev=inb(cmos_data);	// read the current value of register B
  outb(cmos_addr, 0x8B);		// set the index again (a read will reset the index to register D)
  outb(cmos_data, prev | 0x40);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
  //NOT SURE ^^^^^^
  sti();		// re-enable interrupts
}

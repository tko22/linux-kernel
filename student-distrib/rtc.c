#include "rtc.h"
#include "lib.h"
#include "i8259.h"
// tae
#define cmos_addr 0x70
#define cmos_data 0x71

void init_rtc(){
  cli();		// disable interrupts
  //NOT SURE vvvvvv
  outb(0x8A,cmos_addr); // write to register A
  outb(0x2F,cmos_data);
  outb(0x8B,cmos_addr);		// select register B, and disable NMI
  unsigned char prev=inb(cmos_data);	// read the current value of register B
  outb(0x8B,cmos_addr);		// set the index again (a read will reset the index to register D)
  outb( prev | 0x40,cmos_data);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
  //NOT SURE ^^^^^^
  sti();		// re-enable interrupts
}
void handle_rtc_interrupt(){
  test_interrupts();
  send_eoi(8);
  outb(0x83,0x70);
  inb(0x71);
  printf("rtc handled");
}

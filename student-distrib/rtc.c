#include "rtc.h"
#include "lib.h"
// tae
#define cmos_addr = 0x70;
#define cmos_data = 0x71;

void init_rtc(){
  disable_ints();		// disable
  outportb(0x70, 0x8A);	// select Status Register A, and disable NMI (by setting the 0x80 bit)
  outportb(0x71, 0x20);	// write to CMOS/RTC RAM
  enable_ints();		// allow interrupts after this
  //trying to get some time
  outb(cmos_addr,0x04); // set to read from reg 0x04, which is hour
  printk("\nCMOS_VALUE:%x\n",in_byte(cmos_data));
}

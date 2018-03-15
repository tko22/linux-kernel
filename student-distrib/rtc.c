#include "rtc.h"
#include "lib.h"
#include "i8259.h"
// tae
#define cmos_addr 0x70
#define cmos_data 0x71

/* void init_rtc();
 * Inputs: none
 * Return Value: none
 * Function: Initialize the RTC.
 */
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

/* void handle_rtc_interrupt();
 * Inputs: none
 * Return Value: none
 * Function: Handle RTC interrupts. Called using assembly linkage.
 */
void handle_rtc_interrupt(){
  test_interrupts();
  send_eoi(8);
  outb(0x0C,cmos_addr);
  inb(0x71);
  printf("rtc handled");



}

//open the rtc
int32_t open_rtc(const uint8_t* filename){

    return 0;

}

//change the frequency of the RTC
int32_t write_rtc(int32_t fd, const void* buf, int32_t nbytes){

    int i;
    int flag;
    flag = 0;
    int value;
    unsigned char rate;

    // check if nbytes is in the acceptable rate for frequencies. (4 bytes)
    if(nbytes != 4)
        return -1;

    int32_t temp = *(int32_t*)buf;      //frequency to set

    //frequency can be 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024
    int frequency[10] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

    //check is frequency is valid
    for(i = 0; i < 10; i++){
        if(temp == frequency[i]){}
          flag = 1;                 //check if frequency is valid.
          value = i;                //if valid, store at what index
        }
    }
    if(flag != 1)
        return -1;

    //frequency is equal to (2^16/2^rate)
    //however, there are only 10 possible rate values
    //rate can be 0xF, 0xE, 0xD, 0xC, 0xB, 0xA, 0x9, 0x8, 0x7, 0x6
    char rate_val[10] = {0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06};

    rate = rate_val[value];

    cli();
    outportb(cmos_addr, 0x8A);		// set index to register A, disable NMI
    char prev = inportb(cmos_data);	// get initial value of register A
    outportb(cmos_addr, 0x8A);		// reset index to A
    outportb(cmos_data, (prev & 0xF0) | rate); //write only our rate to A. Note, rate is the bottom 4 bits.
    sti();

}

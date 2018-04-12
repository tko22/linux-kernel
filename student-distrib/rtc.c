#include "rtc.h"
#include "lib.h"
#include "i8259.h"
// tae
#define cmos_addr 0x70
#define cmos_data 0x71


volatile int i_flag = 0;

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
 //test_interrupts();
  send_eoi(8);
  outb(0x0C,cmos_addr);
  inb(0x71);
  //printf("rtc handled");
  // int32_t test_buf;
  // test_buf= 512;                     //The buffer which holds the frequency
	// fd_t* test_file;
  // write_rtc(test_file, (const char*)&test_buf, 4);
  //uint8_t* name = 0;
  //open_rtc(name);
  i_flag = 1;


}

/* void open_rtc();
 * Inputs: const uint8_t* filename
 * Return Value: 0
 * Function: Set the defualt rate for the rtc to 2
 */
int32_t open_rtc(const uint8_t* filename){

  cli();
  outb(0x8A, cmos_addr);		// index equal to register A
  char prev = inb(cmos_data);	// obatin what is register A
  outb(0x8A, cmos_addr);		// set back
  outb((prev & 0xF0) | 0x0F, cmos_data); // set rate to 2, the lower bits
  sti();


  return 0;

}


/* void close_rtc();
 * Inputs: const uint8_t* filename
 * Return Value: 0
 * Function: Close the RTC
 */
int32_t close_rtc(fd_t* fd){

    return 0;

}

/* void read_rtc();
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Return Value: 0
 * Function: Return 0 whenever an interrupt occurs
 */
int32_t read_rtc(fd_t* fd, uint8_t* buf, int32_t nbytes){
    if((int32_t)fd < 0){
      return -1;
    }
    i_flag = 0;
  //  sti();
    while(i_flag != 1){           //spinning unitl flags informs you that interrupt has occured

    }
//  printf("READ ");
  //  i_flag = 0;                   //set the flag back to 0
    return 0;                     //return 0 snce interrupt has occured.

}

/* void read_rtc();
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Return Value: 0/-1
 * Function: Set the rate of the RTC based on the frequency being passed
 */
int32_t write_rtc(fd_t* fd, const uint8_t* buf, int32_t nbytes){
    if((int32_t)fd < 0){
      return -1;
    }
    int i;
    int flag;
    flag = 0;
    int value;
    unsigned char rate;

    // check if nbytes is in the acceptable rate for frequencies. (4 bytes)
    if(nbytes != 4)
        return -1;

    int32_t temp = *(int32_t*)buf;      //frequency to set
  printf("value is %d\n", temp);
    //frequency can be 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024
    int frequency[10] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

    //check is frequency is valid
    for(i = 0; i < 10; i++){
        if(temp == frequency[i]){
          flag = 1;                 //check if frequency is valid.
          value = i;                //if valid, store at what index
        }
    }
   //printf("%d\n", value);

    if(flag != 1)
        return nbytes;

    //frequency is equal to (2^16/2^rate)
    //however, there are only 10 possible rate values
    //rate can be 0xF, 0xE, 0xD, 0xC, 0xB, 0xA, 0x9, 0x8, 0x7, 0x6
    char rate_val[10] = {0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06};

    rate = rate_val[value];
  //printf("rate is %x\n", rate);

    cli();
    outb(0x8A, cmos_addr);		// index equal to register A
    char prev = inb(cmos_data);	// obatin what is register A
    outb(0x8A, cmos_addr);		// set back
    outb((prev & 0xF0) | rate, cmos_data); // set A to rate, the lower bits
    sti();
//  printf("done\n");
    return nbytes;

}

#include "scheduling.h"
#include "sys.h"
#include "i8259.h"
#include "x86_desc.h"
#include "paging.h"
#include "lib.h"



void initalize_PIT(void){
    enable_irq(0); //IRQ 0 is for PIT
    //TODO set the frequency for ticks to use in Round-Robin Method
}

//TODO still have to figure out what functions are necessary for switching
//     processes, and updating them based on tick frequency.

//     Needs to be done by getting status of the current process being used and
//     looping them using using queue methodology.

//     Probably going to need to figure out how to add new processes into the
//     queue. 

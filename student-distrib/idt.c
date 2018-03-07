#include "x86_desc.h"
#include "idt.h"

void init_idt(){
    int i;
    // - From 0 to 31   : exceptions and non-maskable interrupts
    // - From 32 to 47  : maskable interrupts
    // - From 48 to 255 : software interrupts

    // 32 Exceptions to handle
    for(i = 0; i < 32; i++) {
        idt[i].seg_selector

        
    }
}
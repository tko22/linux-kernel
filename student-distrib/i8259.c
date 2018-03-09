/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */
/* Initialize the 8259 PIC */
void i8259_init(void) {
    // TODO add the interrupt mask before setting up the pic here
  /*  unsigned char mask1,mask2;
    mask1 =inb(MASTER_8259_PORT_DATA);
    mask2 = inb(SLAVE_8259_PORT_DATA);*/
    outb(0xFF,MASTER_8259_PORT_DATA);
    outb(0xFF,SLAVE_8259_PORT_DATA);
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW2_MASTER, MASTER_8259_PORT_DATA);
    outb(ICW3_MASTER, MASTER_8259_PORT_DATA);
    outb(ICW4, MASTER_8259_PORT_DATA);

    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_SLAVE, SLAVE_8259_PORT_DATA);
    outb(ICW3_SLAVE, SLAVE_8259_PORT_DATA);
    outb(ICW4, SLAVE_8259_PORT_DATA);
    outb(0xFF,MASTER_8259_PORT_DATA);
    outb(0xFF,SLAVE_8259_PORT_DATA);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    unsigned char mask = MASK;
    printf("\nmaster_mask:%x\n",master_mask);
    printf("\nslave_mask:%x\n",slave_mask);
    printf("\nirq_num:%u\n",irq_num);
    if(irq_num > 7){                    //check if master or slave PIC
      uint32_t temp = irq_num - 8;      //reset offset based for Slave
      int i;
      for(i = 0; i < temp; i++){        //bit shift unti IRQ value
          mask = mask << 1;
          mask = mask + 1;
      }
      slave_mask = slave_mask & mask;     //update the slave_mask
      outb(slave_mask, SLAVE_8259_PORT_DATA);
      printf("\nslave_mask:%x\n",slave_mask);
    }
    else{                               //if master PIC
      int i;
      for(i = 0; i < irq_num; i++){     //bit shift until IRQ value
          mask = mask << 1;
          mask = mask + 1;
      }
      master_mask = master_mask & mask;   //update master_mask
      outb(master_mask, MASTER_8259_PORT_DATA);
      printf("\nmaster_mask:%x\n",master_mask);
    }
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    unsigned char disable_mask = DISABLE_MASK;
    master_mask =inb(MASTER_8259_PORT_DATA);
    slave_mask = inb(SLAVE_8259_PORT_DATA);
      if(irq_num > 7){                    //check if master or slave PIC
        uint32_t temp = irq_num - 8;      //reset offset based for Master
        int i;
        for(i = 0; i < temp; i++){        //bit shift unti IRQ value
            disable_mask = disable_mask << 1;
        }
        slave_mask = slave_mask | disable_mask;     //update the master_mask
        outb(slave_mask, SLAVE_8259_PORT_DATA);
      }
      else{                               //if slave PIC
        int i;
        for(i = 0; i < irq_num; i++){     //bit shift until IRQ value
            disable_mask = disable_mask << 1;
        }
        master_mask = master_mask & disable_mask;   //update slave_mask
        outb(master_mask, MASTER_8259_PORT_DATA);
      }

}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {

    uint32_t temp = irq_num - 8;

    if(irq_num > 8){                            //if slave port, send command
      outb(EOI + 2, MASTER_8259_PORT);          //to both master and slave
      outb(EOI | temp, SLAVE_8259_PORT);
    }
    else
      outb(EOI | irq_num, MASTER_8259_PORT);    //if master port, send command
                                                //to master only
}

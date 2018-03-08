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

}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {

    if(irq_num > 7){                    //check if master or slave PIC
      uint32_t temp = irq_num - 8;      //reset offset based for Slave
      int i;
      for(i = 0; i < temp; i++){        //bit shift unti IRQ value
          MASK = MASK << 1;
          MASK = MASK + 1;
      }
      slave_mask = slave_mask & MASK;     //update the slave_mask
      outb(slave_mask, SLAVE_8259_PORT_DATA);
    }
    else{                               //if master PIC
      int i;
      for(i = 0; i < irq_num; i++){     //bit shift until IRQ value
          MASK = MASK << 1;
          MASK = MASK + 1;
      }
      master_mask = master_mask & mask;   //update master_mask
      outb(master_mask, MASTER_8259_PORT_DATA);
    }

}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {

      if(irq_num > 7){                    //check if master or slave PIC
        uint32_t temp = irq_num - 8;      //reset offset based for Master
        int i;
        for(i = 0; i < temp; i++){        //bit shift unti IRQ value
            DISABLE_MASK = DISABLE_MASK << 1;
        }
        slave_mask = slave_mask | DISABLE_MASK;     //update the master_mask
        outb(slave_mask, SLAVE_8259_PORT_DATA);
      }
      else{                               //if slave PIC
        int i;
        for(i = 0; i < irq_num; i++){     //bit shift until IRQ value
            DISABLE_MASK = DISABLE_MASK << 1;
        }
        master_mask = master_mask & DISABLE_MASK;   //update slave_mask
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

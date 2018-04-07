#ifndef _PCB_H
#define _PCB_H

#include "fs.h"
#include "sys.h"
#include "lib.h"
#include "file_desc.h"

pcb_t pcb_init();
void pcb_store(uint32_t pid);
extern pcb_t pcb_init();

#endif

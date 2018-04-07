#ifndef _SYS_H
#define _SYS_H

#include "types.h"
#include "file_desc.h"

<<<<<<< HEAD
#define USER_ADDRESS 0x8048000
#define FOUR_KB 4096
=======
#define PROCESS_ADDRESS 0x800000 // physical address to 4MB page for program
#define USER_ADDRESS 0x8048000
#define FOUR_KB 4096
#define KERNEL_STACK_SIZE 0x2000 // 8KB
#define MB8_ADDRESS 0x80000 // 8 MB
#define KB8 0x2000 // 8 KB
#define PCB_MASK 0xFFFFE000

>>>>>>> f3e81f545cfbc12a29fbbaee1e29c8f2e02d3749

extern void halt(); // TODO: REMOVE THIS and change to below
// extern int32_t halt(uint8_t status);

extern int32_t execute(const uint8_t* command);
pcb_t *get_last_pcb(void);

// int32_t fd is used by user-level programs to access file_array[fd]
asmlinkage int32_t read (int32_t fd, void* buf, int32_t nbytes);
asmlinkage int32_t write (int32_t fd, const void* buf, int32_t nbytes);
asmlinkage int32_t open (const uint8_t* filename);
asmlinkage int32_t close (int32_t fd);

// CP4
// extern int32_t getargs(uint32_t* buf, int32_t nbytes);
// extern int32_t vidmap(uint8_t** screen_start);
// extern int32_t set_handler(int32_t signum, void* handler_address);
// extern int32_t signreturn(void);



#endif

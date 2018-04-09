#ifndef _SYS_H
#define _SYS_H

#include "types.h"
#include "file_desc.h"
#define asmlinkage __attribute__((regparm(0)))


#define PROCESS_ADDRESS 0x8000000 // physical address to 4MB page for program
#define USER_ADDRESS 0x8048000  //virtual address
#define _128MB 0x8000000
#define OFFSET_ADDR 0x48000
#define FOUR_KB 4096 //4KB
#define FOUR_MB 0x400000 //4 MB
#define KERNEL_STACK_SIZE 0x2000 // 8KB
#define KB8 0x2000 // 8 KB
#define PCB_MASK 0xFFFFE000
#define MAGIC_EXECUTABLE1 0x7f
#define MAGIC_EXECUTABLE2 0x45
#define MAGIC_EXECUTABLE3 0x4c
#define MAGIC_EXECUTABLE4 0x46



//extern void halt(); // TODO: REMOVE THIS and change to below
extern int32_t halt(uint8_t status);

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

#ifndef _SYS_H
#define _SYS_H

#include "types.h"
#include "file_desc.h"

extern void halt(); // TODO: REMOVE THIS and change to below
// extern int32_t halt(uint8_t status);

extern int32_t execute(const uint8_t* command);

// int32_t fd is used by user-level programs to access file_array[fd]
extern int32_t read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t write (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t open (const uint8_t* filename);
extern int32_t close (int32_t fd);

// CP4
// extern int32_t getargs(uint32_t* buf, int32_t nbytes);
// extern int32_t vidmap(uint8_t** screen_start);
// extern int32_t set_handler(int32_t signum, void* handler_address);
// extern int32_t signreturn(void);



#endif

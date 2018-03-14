#ifndef _SYS_H
#define _SYS_H

#include "types.h"


extern void halt();
extern int32_t sys_open (const uint8_t* filename);
extern int32_t sys_read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t sys_close (int32_t fd);

#endif

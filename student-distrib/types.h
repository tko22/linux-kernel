/* types.h - Defines to use the familiar explicitly-sized types in this
 * OS (uint32_t, int8_t, etc.).  This is necessary because we don't want
 * to include <stdint.h> when building this OS
 * vim:ts=4 noexpandtab
 */

#ifndef _TYPES_H
#define _TYPES_H

#define NULL 0

#ifndef ASM

#define USER_ADDRESS 0x8048000
#define FOUR_KB 4096
#define KERNEL_STACK_SIZE 0x2000 // 8KB
#define MB8_ADDRESS 0x80000 // 8 MB
#define KB8 0x2000 // 8 KB
#define TERM_BUF_SIZE	128 // buffer of the terminal driver
#define PCB_LOCATION_MASK 0xFFFFE000  // get everything except offset in pcb (8kb)

/* Types defined here just like in <stdint.h> */
typedef int int32_t;
typedef unsigned int uint32_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef char int8_t;
typedef unsigned char uint8_t;

typedef struct PCB{
	uint8_t name[TERM_BUF_SIZE]; // max size of terminal driver size
	uint8_t args[TERM_BUF_SIZE]; // just to make sure it will fit
	uint32_t pid;
 	struct file fd[8];  // file structure
	struct PCB * parent;
	//Register table
} PCB_t;


#endif /* ASM */

#endif /* _TYPES_H */

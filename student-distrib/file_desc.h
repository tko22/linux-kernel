#ifndef _FILE_DESC_H
#define _FILE_DESC_H

#include "types.h"

#define FD_ARRAY_SIZE       8

// File Descriptor - Well, this isn't needed for cp2...
// should've went to discussion lol
typedef struct fd_t {
  uint32_t file_op_table_pointer;
  uint32_t inode;
  uint32_t file_pos;
  uint32_t flags;
} fd_t;



typedef struct pcb_t { // idk wtf this is, Process Control Block....
  fd_t* file_array[FD_ARRAY_SIZE];  // file array holding open files
  struct pcb_t* parent;                    // holds parent
} pcb_t;



// jump table for "file_op_table_pointer"
// example: https://stackoverflow.com/questions/9932212/jump-table-examples-in-c
typedef struct file_ops_jump_table_t {
  int32_t (*open)(fd_t* , uint8_t*);
  int32_t (*close)(fd_t*);
  int32_t (*write)(fd_t *, const uint8_t*, int32_t);
  int32_t (*read)(fd_t*, uint8_t*, int32_t );
} file_ops_jump_table_t;


#endif

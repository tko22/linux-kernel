#ifndef _FILE_DESC_H
#define _FILE_DESC_H

#include "types.h"

#define FD_ARRAY_SIZE       8

typedef struct pcb_t { // idk wtf this is, Process Control Block....
  // more stuff should be in here, but it's in checkpoint 3
  // so idk whether we should just initialize the file array
  // itself instead of initializing pcb
  fd_t file_array[FD_ARRAY_SIZE];
} pcb_t;

// File Descriptor
typedef struct fd_t {
  uint32_t file_op_table_pointer;
  uint32_t inode;
  uint32_t file_pos;
  uint32_t flags;
} fd_t;

// jump table for "file_op_table_pointer"
// example: https://stackoverflow.com/questions/9932212/jump-table-examples-in-c
typedef struct file_ops_jump_table_t {
  int32_t (*open)(uint8_t* filename);
  int32_t (*read)(void* buf, int32_t nbytes);
  int32_t (*write)(const void* buf, int32_t nbytes);
  int32_t (*close)(void);
} file_ops_jump_table_t;

// file array that should be in pcb in cp3
extern fd_t file_array[FD_ARRAY_SIZE];






#endif

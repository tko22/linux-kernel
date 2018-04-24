#ifndef _FILE_DESC_H
#define _FILE_DESC_H

#include "types.h"

#define FD_ARRAY_SIZE       8
#define MAX_NUM_PROCESSES   6
#define MAX_TERMINALS       3
// File Descriptor - Well, this isn't needed for cp2...
// should've went to discussion lol
typedef struct fd_t {
  struct file_ops_jump_table_t* file_op_table_pointer;
  uint32_t inode;
  uint32_t file_pos;
  uint32_t flags;  // in-use = 1, free = 0
} fd_t;



typedef struct pcb { // idk wtf this is, Process Control Block....
  struct fd_t fd_arr[FD_ARRAY_SIZE];  // file array holding open files
  struct pcb* parent;         // holds parent
  uint8_t argsbuffer[128]; // buffer for args to be filled by execute and read by getargs
  uint32_t pid;                 // process id
  uint32_t esp;
  uint32_t ebp;
  uint32_t cr3;
  uint32_t esp0;
  uint32_t ss0;
  uint32_t status;
} pcb_t;

typedef struct terminal_t{
  uint32_t bufferPos;
  uint32_t currentcolumn;
  uint32_t currentrow;
  uint32_t terminalrow;
  uint32_t terminalcol;
  uint8_t vidmap_pid;
  pcb_t* parent_pcb;
  uint32_t video_physical;
}terminal_t;

// index 0 will be NULL and not be used
int active_proc[MAX_NUM_PROCESSES + 1]; // + 1 to match pids, which start at 1

terminal_t terminals[MAX_TERMINALS];
int currentterminal; // initialized in kernel.c

// jump table for "file_op_table_pointer"
// example: https://stackoverflow.com/questions/9932212/jump-table-examples-in-c
typedef struct file_ops_jump_table_t {
  int32_t (*open)(fd_t* , const uint8_t*);
  int32_t (*close)(fd_t*);
  int32_t (*write)(fd_t *, const uint8_t*, int32_t);
  int32_t (*read)(fd_t*, uint8_t*, int32_t );
} file_ops_jump_table_t;


#endif

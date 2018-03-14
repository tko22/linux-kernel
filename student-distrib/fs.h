#ifndef _FS_H
#define _FS_H

#include "types.h"
#define MAX_NAME_LENGTH                 32
#define NUM_DENTRY_RESERVED_BYTES       24
#define NUM_BOOT_BLOCK_RESERVED_BYTES   52
#define NUM_BOOT_DENTRIES               63
#define NUM_DATA_BLOCK                  1023
#define FD_ARRAY_SIZE                   8
#define BLOCK_SIZE                      4096 // 4 KB
#define INODE_NUM                       64

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

typedef struct dentry_t {
    char file_name[MAX_NAME_LENGTH];
    uint32_t file_type;
    uint32_t inode_num;
    uint8_t reserved[NUM_DENTRY_RESERVED_BYTES]; // reserved 24 bytes
} dentry_t;

typedef struct boot_block_t {
    uint32_t num_dir_entries;
    uint32_t num_inodes;
    uint32_t num_data_blocks;
    uint8_t reserved[NUM_BOOT_BLOCK_RESERVED_BYTES];
    dentry_t dentries[NUM_BOOT_DENTRIES];
} boot_block_t;

typedef struct inode_t {
    int32_t length;
    int32_t data_block_num[NUM_DATA_BLOCK];
} inode_t;

// jump table for "file_op_table_pointer"
// example: https://stackoverflow.com/questions/9932212/jump-table-examples-in-c
typedef struct file_ops_jump_table_t {
    int32_t (*open)(uint8_t* filename);
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*close)(int32_t fd);
} file_ops_jump_table_t;

// Boot Block
extern boot_block_t boot_block;

// file array that should be in pcb in cp3
extern fd_t file_array[FD_ARRAY_SIZE];


extern void init_fs();
extern int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
extern int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
extern int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

extern int32_t stdin_open (const uint8_t* filename);
extern int32_t stdin_read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t stdin_write (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t stdin_close (int32_t fd);



#endif

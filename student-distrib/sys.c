#include "sys.h"
#include "types.h"
#include "fs.h"

void halt(){
    asm volatile (".1: hlt; jmp .1;");
}

int32_t execute(const uint8_t* command){
    return 1;
}
int32_t read (int32_t fd, void* buf, int32_t nbytes){
    // returns number of bytes read
    return 0; // returns 0 if initial file position is at or beyond EOF for normal files
}
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    // returns number of bytes written
    return -1; // returns -1 on failure
}
int32_t open (const uint8_t* filename){
    // probably calls read_dentry_by_name
    // returns fd
    return -1; // returns -1 on failure
}
int32_t close (int32_t fd){
    // returns 0 on success

    
    return -1; // returns -1 on failure
}

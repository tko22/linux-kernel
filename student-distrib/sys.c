#include "sys.h"
#include "types.h"
#include "fs.h"
#include "lib.h"

void halt(){
    asm volatile (".1: hlt; jmp .1;");
}
// THE REAL HALT
// int32_t halt(uint8_t status) {
//     asm volatile (".1: hlt; jmp .1;");
//     return 1;
// }

int32_t execute(const uint8_t* command){
    int i;
    char filename[33];
    printf("execute systemcall called\n");
    //parse the command
    int cmdcopied = 0;
    for(i=0;i<strlen((char*)command);i++){
      if(command[i] == ' '){ // there is args
        strncpy(filename,command,i+1); //copy the filename to filename
        cmdcopied=1;
      }
    }
    if(cmdcopied==0){ //there is no args
      strncpy(filename,command,strlen((char*)command));
    }
    printf("filename from command:%s\n",filename);
    // CHECKPOINT 4 : put args here
    // check if file is valid executable
    dentry_t dentry;
    read_dentry_by_name((uint8_t*)filename,&dentry);
    printf("file type:%d",dentry.file_type);
    if(dentry.file_type!=2){
      printf("execute error: file type is not executable");
      return -1;
    }

    return 1;
}
int32_t read (int32_t fd, void* buf, int32_t nbytes){
    // returns number of bytes read
    printf("read systemcall called\n");
    return 0; // returns 0 if initial file position is at or beyond EOF for normal files
}
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    // returns number of bytes written
    printf("write systemcall called");
    return -1; // returns -1 on failure
}
int32_t open (const uint8_t* filename){
    // probably calls read_dentry_by_name
    // returns fd
    printf("open systemcall called");
    return -1; // returns -1 on failure
}
int32_t close (int32_t fd){
    // returns 0 on success
    printf("close systemcall called");
    return -1; // returns -1 on failure
}

// CP4
/*
int32_t getargs(uint32_t* buf, int32_t nbytes) {
    return 0;
}
int32_t vidmap(uint8_t** screen_start){
    return 0;
}
int32_t set_handler(int32_t signum, void* handler_address){
    return 0;
}
extern int32_t signreturn(void){
    return 0;
}
*/

#include "sys.h"
#include "types.h"
#include "fs.h"
#include "lib.h"
#include "file_desc.h"
#include "rtc.h"
#include "paging.h"
#include "pcb.h"


#define FILES 8

// void halt(){
//     asm volatile (".1: hlt; jmp .1;");
// }
//THE REAL HALT
int32_t halt(uint8_t status) {
//     asm volatile (".1: hlt; jmp .1;");
//     return 1;
//
// TOdo
// Use PCB
// Clean
// Set PCB to its parents pid, not its current.
// Also do same for TSS
// Modify esp and ebp

  pcb_t* curr;
  pcb_t* parent;
  curr = get_last_pcb();

  parent = curr.parent;
  curr.pid = parent.pid;

  int i = 0;                      //close all the files
  while(i < FILES){
    close(i);
    i++;
  }
  if(curr.parent.pid == curr.pid){       //execute another shell when trying to halt the parent
        execute((uint8_t *)"shell")
  }

  // asm volatile(
	// 	           "movl %2, %%ebp  				\n"
  //   		       "movl %1, %%eax 			  	\n"
  //     		     "movl %0, %%esp 				  \n"
  //     		    );

}


int32_t execute(const uint8_t* command){
    int i;
    char filename[33];
    printf("execute systemcall called\n");
    pcb_t* caller_pcb;
    pcb_t curr = pcb_init();
    caller_pcb=get_last_pcb();
    printf("call get last pcb:%x\n",caller_pcb);
    uint32_t new_pid = -1;
    int flag = 0;
    for ( i = 0; i < MAX_NUM_PROCESSES; i++){
        if (process_id_in_use[i] == 0){
            new_pid = i + 1; // offset so
            process_id_in_use[i] = 1;
            flag = 1;
            break;
        }
    }
    if (flag == 0 ){
        printf("Maxed processes\n");
        return -1;
    }
    curr.pid = new_pid;
    curr.parent = get_last_pcb();
    pcb_t *p_address = (pcb_t*)((uint32_t)get_last_pcb() - KB8);
    memcpy(p_address, &curr, sizeof(pcb_t));

    // TODO: setup pcb, check whether pcb exists or not
    //parse the command
    int cmdcopied = 0;
     for(i=0;i<strlen((char*)command);i++){
       if(command[i] == ' '){ // there is args
         strncpy(filename,command,i); //copy the filename to filename
         filename[i] = '\0'; // null terminate
         cmdcopied=1;
       }
     }
     printf("%d", strlen((char*)filename));
     if(cmdcopied==0){ //there is no args
       strncpy(filename,command,strlen((char*)command));
       filename[strlen((char*)command)] = '\0';
     }
     printf("filename from command:%s\n",filename);

    // check if file is valid executable
    dentry_t dentry;
    if(read_dentry_by_name((char*)filename,&dentry) == -1){
      printf("error: file not found\n");
      return -1;
    }
    printf("file type:%d",dentry.file_type);
    if(dentry.file_type!=2){
      printf("execute error: file type is not a file\n");
      return -1;
    }
    // TODO: Copy
    load_program(curr.pid);
    // check for magic numbers
    uint8_t magicbuffer[4]; // check first 40 bytes
    read_data(dentry.inode_num, 0, magicbuffer, 4);
    if(magicbuffer[0] != MAGIC_EXECUTABLE1 ||
      magicbuffer[1] != MAGIC_EXECUTABLE2 ||
      magicbuffer[2] != MAGIC_EXECUTABLE3||
      magicbuffer[3] != MAGIC_EXECUTABLE4
    ){ // if magic numbers doesn't preset
      printf("execute error: magic numbers for executable don't match");
      return -1;
    }
    uint8_t *filebuffer = (uint8_t*)USER_ADDRESS;
    inode_t* thisinode = ((void*)boot_block + (dentry.inode_num + 1) * BLOCK_SIZE);
    read_data(dentry.inode_num, 0, filebuffer, thisinode->length);
    // check the filebuffer for following magic number 0: 0x7f; 1: 0x45; 2: 0x4c; 3: 0x46
    // setup the iret thing
    return 1;
}
int32_t read (int32_t fd, void* buf, int32_t nbytes){
    // returns number of bytes read
    printf("read systemcall called\n");
    // TODO: get current pcb
    // then, check if file is in use or whether fd is in bounds
    // if (fd >= 0 && fd < 8 && file_array[fd].flags == 1){
    //     // TODO: Switch file_array to whatever is used in pcb
    //     int32_t ret = file_array[fd]->file_op_table_pointer->read(file_array[fd], buf, nbytes);
    //     return ret;
    // }
    return 0; // returns 0 if fail - initial file position is at or beyond EOF for normal files
}
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    // returns number of bytes written
    printf("write systemcall called");
    // TODO: get current pcb
    // if (fd >= 0 && fd < 8 && file_array[fd].flags == 1 && buf == NULL){
    //     int32_t ret = file_array[fd]->file_op_table_pointer->write(file_array[fd],buf,nbytes);
    //     return ret;
    // }
    return -1; // returns -1 on failure
}
int32_t open (const uint8_t* filename){
    // probably calls read_dentry_by_name
    // returns fd
    printf("open systemcall called");
    if (filename == NULL) return -1;
    // TODO: get currenct pcb
    // dentry_t dentry;
    // int check;
    // check = read_dentry_by_name(filename, &dentry);
    // if (check == -1 ) return -1; // check whether read_dentry worked

    // int i;
    // for (i = 2; i < FD_ARRAY_SIZE; i++){
    //     // check for unused idx
    //     if (file_array[i] == NULL){
    //         // put fd in here
    //         fd_t new_fd;
    //         new_fd.flags = 1;
    //         int sec_check;
    //         if (dentry.file_type == 0){
    //             // rtc
    //             new_fd.inode = NULL;
    //             new_fd.file_pos = 0;
    //             new_fd.file_op_table_pointer = &rtc_jump;
    //             // call open
    //             sec_check = rtc_jump.open(&new_fd, filename);
    //         }
    //         else if ( dentry.file_type == 1){
    //             // directory
    //             new_fd.inode = dentry.inode_num;
    //             new_fd.file_pos = 0;
    //             new_fd.file_op_table_pointer = &dir_jump;
    //             // call open
    //             sec_check = dir_jump.open(&new_fd, filename);
    //         }
    //         else if ( dentry.file_type == 2){
    //             // file
    //             new_fd.inode = dentry.inode_num;
    //             new_fd.file_pos = 0;
    //             new_fd.file_op_table_pointer = &file_jump;
    //             // call open
    //             sec_check = file_jump.open(&new_fd, filename);
    //         }
    //         else {
    //             printf("opening invalid filetype");
    //             return -1;
    //         }
    //         if (sec_check == -1) return -1; // check if opened failed
    //         //TODO: add new_fd to the pcb
    //         return i;
    //     }
    //     // use pcb.filearray later
    //     else if (file_array[i].inode == dentry.inode_num){
    //         // file is already opened
    //         printf("file is already opened");
    //         return -1;
    //     }

    // }
    return -1; // returns -1 on failure
}
int32_t close (int32_t fd){
    // returns 0 on success
    printf("close systemcall called");

    // TODO: get current pcb
    // if (file_array[fd] != NULL && fd >= 0 && fd < 8){
    //     int check;
    //     check = file_array[fd]->file_op_table_pointer->close(file_array[fd]);
    //     if (check == -1){
    //         return -1; // returns -1 on failure
    //     }
    //     fd_t* temp = file_array[fd];
    //     free(temp); // NOT SURE IF I SHOULD DO THIS
    //     file_array[fd] = NULL;
    //     return 0;
    //     // TODO: REMEMBER TO REPLACE ALL FILE_ARRAY STUFF TO PCB
    // }
    return -1; // returns -1 on failure
}


//read_data(dentry.inode, 0, (uint8_t*)USER_ADDRESS, FOUR_KB);

pcb_t *get_last_pcb(void){
  pcb_t *last;
  asm volatile("andl %%esp, %0"
                 : "=r" (last)
                 : "r" (PCB_MASK)
               );
  return last;
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

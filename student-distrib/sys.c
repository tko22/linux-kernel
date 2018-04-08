#include "sys.h"
#include "types.h"
#include "fs.h"
#include "lib.h"
#include "file_desc.h"
#include "rtc.h"
#include "paging.h"
#include "pcb.h"
#include "x86_desc.h"

#define FILES 8


//     asm volatile (".1: hlt; jmp .1;");
int32_t halt(uint8_t status) {

  pcb_t* curr;
  pcb_t* parent;

  curr = get_last_pcb();         //get current and parent pcb
  parent = curr->parent;

  process_id_in_use[curr->pid] = 0;     //set the current pid to not in used
  process_id_in_use[parent->pid] = 1;   //make sure parent pid is the one in use
  tss.esp0 = parent->esp0;              //do the same thing for esp0 and ss0
  tss.ss0 = parent->ss0;

  int i;                              //close all the files
  for(i = 0; i < FILES; i++){
    close(i);
  }
  if(parent->pid == curr->pid){       //execute another shell when trying to halt the parent
        execute((uint8_t *)"shell");
  }

  asm volatile(                                         //restore the registers for execute
               "movl %0, %%ebp		#Save EBP	\n"
               "movl %1, %%esp     #Save ESP 	\n"
               "movl %2, %%cr3 	#Save cr3 	\n"
               : "=r" (parent->ebp), "=r" (parent->esp), "=r" (parent->cr3)
               :
               : "cc"
               );
  asm volatile("jmp halt_ret");
  return 0;

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
    asm volatile(
                 "movl %%ebp, %0		#Save EBP	\n"
                 "movl %%esp, %1     #Save ESP 	\n"
                 "movl %%cr3, %2 	#Save cr3 	\n"
                 : "=r" (p_address->ebp), "=r" (p_address->esp), "=r" (p_address->cr3)
                 :
                 : "cc"
                 );

    // TODO: setup pcb, check whether pcb exists or not
    //parse the command
    int cmdcopied = 0;
     for(i=0;i<strlen((char*)command);i++){
       if(command[i] == ' '){ // there is args
         strncpy(filename,(char*)command,i); //copy the filename to filename
         filename[i] = '\0'; // null terminate
         cmdcopied=1;
       }
     }
     printf("%d", strlen((char*)filename));
     if(cmdcopied==0){ //there is no args
       strncpy(filename,(char*)command,strlen((char*)command));
       filename[strlen((char*)command)] = '\0';
     }
     printf("filename from command:%s\n",filename);

    // check if file is valid executable
    dentry_t dentry;
    if(read_dentry_by_name((uint8_t*)filename,&dentry) == -1){
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
    // check for following magic number 0: 0x7f; 1: 0x45; 2: 0x4c; 3: 0x46
    uint8_t fourtybuffer[4]; // check first 40 bytes
    read_data(dentry.inode_num, 0, fourtybuffer, 40);
    if(fourtybuffer[0] != MAGIC_EXECUTABLE1 ||
      fourtybuffer[1] != MAGIC_EXECUTABLE2 ||
      fourtybuffer[2] != MAGIC_EXECUTABLE3||
      fourtybuffer[3] != MAGIC_EXECUTABLE4
    ){ // if magic numbers doesn't preset
      printf("execute error: magic numbers for executable don't match");
      return -1;
    }
    uint8_t *filebuffer = (uint8_t*)USER_ADDRESS;
    inode_t* thisinode = ((void*)boot_block + (dentry.inode_num + 1) * BLOCK_SIZE);
    read_data(dentry.inode_num, 0, filebuffer, thisinode->length);
    // setup the iret thing

    //parent->esp0 = tss.esp0; // set parent esp0 to current esp0
    tss.ss0 = KERNEL_DS; // set ss0 to kernel's data segment
  	tss.esp0 = PROCESS_ADDRESS-KB8 * curr.pid -4; // set esp0 to the stack
    uint32_t esp = USER_ADDRESS + FOUR_MB; // 4 mb under 128 MB
    uint32_t eip =  (fourtybuffer[27] << 24) | (fourtybuffer[26] << 16) | (fourtybuffer[25] << 8) | fourtybuffer[24];
    asm volatile("\
        movw %2, %%ax 	   # USER_DS	          \n\
    		movw %%ax, %%ds 				                \n\
        pushl %2          # push               \n\
        pushl %1          # esp                \n\
        pushfl             # push flags         \n\
        popl %%eax						                  \n\
        orl $0x200, %%eax	 # enable interrupt   \n\
        pushl %%eax					\n\
        pushl	%3		       # push USER_CS\n\
        pushl	%0				   # push eip (program entry point)\n\
        .globl 	halt_ret \n\
        halt_ret:         # halt return here"
        :
        : "r"(eip),"r"(esp),"i"(USER_DS),"i"(USER_CS)
        : "eax"    // we cobble eax
      );
    // need to return value from eax
    uint32_t eax;
	  asm volatile("movl %%eax, %0":"=r" (eax));
    return 1;
}
int32_t read (int32_t fd, void* buf, int32_t nbytes){
    // returns number of bytes read
    printf("read systemcall called\n");
    // TODO: get current pcb
    pcb_t * caller_pcb;
    caller_pcb = get_last_pcb();
    // then, check if file is in use or whether fd is in bounds
    if (caller_pcb->fd_arr[fd] == NULL){
        printf("file doesn't exist at index: %d\n", fd);
        return 0;
    }
    if (fd >= 0 && fd < 8 && caller_pcb->fd_arr[fd]->flags == 1){
        int32_t ret = caller_pcb->fd_arr[fd]->file_op_table_pointer->read(file_array[fd], buf, nbytes);
        printf("it exists %d\n", ret);
        return ret;
    }

    return 0; // returns 0 if fail - initial file position is at or beyond EOF for normal files
}
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    // returns number of bytes written
    printf("write systemcall called");
    // TODO: get current pcb
    pcb_t * caller_pcb;
    caller_pcb = get_last_pcb();
    if (fd >= 0 && fd < 8 && caller_pcb->fd_arr[fd]->flags == 1 && buf == NULL){
        int32_t ret = caller_pcb->fd_arr[fd]->file_op_table_pointer->write(file_array[fd],buf,nbytes);
        return ret;
    }

    return -1; // returns -1 on failure
}
int32_t open (const uint8_t* filename){
    // probably calls read_dentry_by_name
    // returns fd
    printf("open systemcall called");
    if (filename == NULL) return -1;

    pcb_t * caller_pcb;
    caller_pcb = get_last_pcb();

    dentry_t dentry;
    int32_t check;
    check = read_dentry_by_name(filename, &dentry);
    printf("check: %d\n",check);
    if (check == -1 ) {
        return -1; // check whether read_dentry worked
    }

 int i;
 for (i = 2; i < FD_ARRAY_SIZE; i++){
     // check for unused idx
     if (caller_pcb->fd_arr[i] == NULL){
         // put fd in here
         fd_t new_fd;
         new_fd.flags = 1;
         int sec_check;
         if (dentry.file_type == 0){
             // rtc
             new_fd.inode = NULL;
             new_fd.file_pos = 0;
             new_fd.file_op_table_pointer = &rtc_jump;
             // call open
             sec_check = rtc_jump.open(&new_fd, filename);
         }
         else if ( dentry.file_type == 1){
             // directory
             new_fd.inode = dentry.inode_num;
             new_fd.file_pos = 0;
             new_fd.file_op_table_pointer = &dir_jump;
             // call open
             sec_check = dir_jump.open(&new_fd, filename);
         }
         else if ( dentry.file_type == 2){
             // file
             new_fd.inode = dentry.inode_num;
             new_fd.file_pos = 0;
             new_fd.file_op_table_pointer = &file_jump;
             // call open
             sec_check = file_jump.open(&new_fd, filename);
         }
         else {
             printf("opening invalid filetype");
             return -1;
         }
         if (sec_check == -1) return -1; // check if opened failed
         caller_pcb->fd_arr[i] = &new_fd; // add fd_t to file array
         return i;
     }
     // use pcb.filearray later
     else if (caller_pcb->fd_arr[i]->inode == dentry.inode_num){
         // file is already opened
         printf("file is already opened");
         return -1;
     }

    }
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

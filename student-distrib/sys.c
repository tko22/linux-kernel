#include "sys.h"
#include "types.h"
#include "fs.h"
#include "lib.h"
#include "file_desc.h"
#include "rtc.h"
#include "paging.h"
#include "pcb.h"
#include "x86_desc.h"
#include "keyboard.h"

#define FILES 8

volatile int nump = 0;
uint8_t argsbuffer[128]; // buffer for args to be filled by execute and read by getargs
int argsPresent = 0;
int32_t halt(uint8_t status) {

    //printf("halt systemcall called\n");
    pcb_t* curr;
    pcb_t* parent;

    curr = get_last_pcb();         //get current and parent pcb
    parent = curr->parent;
    load_program(parent->pid);

    //process_id_in_use[curr->pid - 1] = 0;     //set the current pid to not in used
    //process_id_in_use[parent->pid - 1] = 1;   //make sure parent pid is the one in use
    tss.esp0 = curr->esp0;              //do the same thing for esp0 and ss0
    tss.ss0 = curr->ss0;
  //  printf("Using parent process_id\n");

    int i;                              //close all the files
    for(i = 2; i < FILES; i++){
        close(i);
    }
    nump--;
  //  printf("Closed all files\n");

    if(nump == 0){       //execute another shell when trying to halt the parent
            //process_id_in_use[curr->pid - 1] = 0;
            //printf("Execute shell\n");
            execute((uint8_t *)"shell");
    }
    curr->status = (uint32_t)status;
    asm volatile(                                         //restore the registers for execute
                "movl %0, %%ebp		#Save EBP	  \n"
                "movl %1, %%esp    #Save ESP 	\n"
                :
                : "r" (curr->ebp), "r" (curr->esp)
                : "memory","eax"
                );
    // printf("Restore ESP and EBP, going to IRET\n");
    asm volatile("jmp halt_ret");        //jmp to halt_ret in execute
    return 0;

}


int32_t execute(const uint8_t* command){
    int i;
    char filename[33];
  //  printf("execute systemcall called\n");
    pcb_t* caller_pcb;
    pcb_t curr = pcb_init();
    caller_pcb=get_last_pcb();
  //  printf("call get last pcb:%x\n",caller_pcb);
    /*uint32_t new_pid = 0;
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
    }*/
    nump++;
    curr.pid = nump;

    curr.fd_arr[0].file_op_table_pointer = &stdin_jump;
    curr.fd_arr[0].flags = 1;

    curr.fd_arr[1].file_op_table_pointer = &stdout_jump;
    curr.fd_arr[1].flags = 1;

    pcb_t *p_address = (pcb_t*)((uint32_t)get_last_pcb() - KB8);
    memcpy(p_address, &curr, sizeof(pcb_t));
    if(p_address->pid <= 1){
      p_address->parent = p_address;
    }
    else{
      p_address->parent = caller_pcb;
    }
    asm volatile(
                 "movl %%ebp, %0		#Save EBP	\n"
                 "movl %%esp, %1     #Save ESP 	\n"
                 "movl %%cr3, %2 	#Save cr3 	\n"
                 : "=r" (p_address->ebp), "=r" (p_address->esp), "=r" (p_address->cr3)
                 :
                 : "memory"
                 );

    //parse the command
    int cmdcopied = 0;
     for(i=0;i<strlen((char*)command);i++){
       if(command[i] == ' '&&cmdcopied==0){ // there is args
         strncpy(filename,(char*)command,i); //copy the filename to filename
         filename[i] = '\0'; // null terminate
         cmdcopied=1;
         argsPresent=1;
       }
       if(cmdcopied==1){ //stuff after command and the first space
         if(command[i] != ' '){ // start the command
          int j =0;
            //TODO
         }
       }
     }
     //printf("%d", strlen((char*)filename));
     if(cmdcopied==0){ //there is no args
       argsPresent=0;
       strncpy(filename,(char*)command,strlen((char*)command));
       filename[strlen((char*)command)] = '\0';
     }
     else{  // case that there are args

     }
  //   printf("filename from command:%s\n",filename);

    // check if file is valid executable
    dentry_t dentry;
    if(read_dentry_by_name((uint8_t*)filename,&dentry) == -1){
      //printf("error: file not found\n");
      nump--;
      return -1;
    }
  //printf("file type:%d",dentry.file_type);
    if(dentry.file_type!=2){
      //printf("execute error: file type is not a file\n");
      nump--;
      return -1;
    }

    // check for following magic number 0: 0x7f; 1: 0x45; 2: 0x4c; 3: 0x46
    uint8_t fourtybuffer[4]; // check first 40 bytes
    read_data(dentry.inode_num, 0, fourtybuffer, 40);
    if(fourtybuffer[0] != MAGIC_EXECUTABLE1 ||
      fourtybuffer[1] != MAGIC_EXECUTABLE2 ||
      fourtybuffer[2] != MAGIC_EXECUTABLE3||
      fourtybuffer[3] != MAGIC_EXECUTABLE4
    ){ // if magic numbers doesn't preset
      nump--;
      //printf("execute error: magic numbers for executable don't match");
      return -1;
    }
    load_program(curr.pid);
    uint8_t *filebuffer = (uint8_t*)USER_ADDRESS;
    inode_t* thisinode = ((void*)boot_block + (dentry.inode_num + 1) * BLOCK_SIZE);
    read_data(dentry.inode_num, 0, filebuffer, thisinode->length);
    // setup the iret thing

    tss.ss0 = KERNEL_DS; // set ss0 to kernel's data segment
  	tss.esp0 = FOUR_MB * 2 - KB8 * curr.pid; // set esp0 to the stack
    p_address->esp0 = tss.esp0;
    p_address->ss0 = tss.ss0;
    uint32_t esp = _128MB + FOUR_MB - 4; // 4 mb under 128 MB
    uint32_t eip =  (fourtybuffer[27] << 24) | (fourtybuffer[26] << 16) | (fourtybuffer[25] << 8) | fourtybuffer[24];
    asm volatile("                \n\
        movw %2, %%ax 	   # USER_DS	          \n\
    		movw %%ax, %%ds 				                \n\
        pushl %2          # push               \n\
        pushl %1          # esp                \n\
        pushfl             # push flags         \n\
        popl %%ebx						                  \n\
        orl $0x200, %%eax	 # enable interrupt   \n\
        pushl %%ebx					\n\
        pushl	%3		       # push USER_CS\n\
        pushl	%0				   # push eip (program entry point)\n\
        iret                            \n\
        "
        :
        : "r"(eip),"r"(esp),"i"(USER_DS),"i"(USER_CS)
        : "ebx"    // we cobble ebx
      );
    asm volatile (" \n\
    .globl 	halt_ret \n\
    halt_ret:         # halt return here");
    // need to return value from eax
    return p_address->status;
}
int32_t read (int32_t fd, void* buf, int32_t nbytes){
    // returns number of bytes read
  //  printf("read systemcall called\n");

    // get current pcb
    pcb_t * caller_pcb;
    caller_pcb = get_last_pcb();
    // then, check if file is in use or whether fd is in bounds
    if (caller_pcb->fd_arr[fd].flags == 0){
        return 0;
    }
    if (fd >= 0 && fd < 8 && caller_pcb->fd_arr[fd].flags == 1){
        int32_t ret = (caller_pcb->fd_arr[fd].file_op_table_pointer->read(&(caller_pcb->fd_arr[fd]), buf, nbytes));
        return ret;
    }

    return 0; // returns 0 if fail - initial file position is at or beyond EOF for normal files
}
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    // returns number of bytes written
  //  printf("write systemcall called");

    // get current pcb
    pcb_t * caller_pcb;
    caller_pcb = get_last_pcb();
    if (fd >= 0 && fd < 8 && caller_pcb->fd_arr[fd].flags == 1 && buf != NULL){
        int32_t ret = caller_pcb->fd_arr[fd].file_op_table_pointer->write(&(caller_pcb->fd_arr[fd]),buf,nbytes);
        return ret;
    }

    return -1; // returns -1 on failure
}
int32_t open (const uint8_t* filename){
  //  printf("open systemcall called");

    if (filename == NULL) return -1;

    pcb_t * caller_pcb;
    caller_pcb = get_last_pcb();

    dentry_t dentry;
    int32_t check;
    check = read_dentry_by_name(filename, &dentry);
    if (check == -1 ) {
        return -1; // check whether read_dentry worked
    }

    int i;
    for (i = 2; i < FD_ARRAY_SIZE; i++){
     // check for unused idx
        if (caller_pcb->fd_arr[i].flags == 0){
         // put fd in here
         caller_pcb->fd_arr[i].flags = 1;
         int sec_check;
         if (dentry.file_type == 0){
             // rtc
             caller_pcb->fd_arr[i].inode = NULL;
             caller_pcb->fd_arr[i].file_pos = 0;
             caller_pcb->fd_arr[i].file_op_table_pointer = &rtc_jump;
             // call open
             sec_check = rtc_jump.open(&(caller_pcb->fd_arr[i]), filename);
         }
         else if ( dentry.file_type == 1){
             // directory
             caller_pcb->fd_arr[i].inode = dentry.inode_num;
             caller_pcb->fd_arr[i].file_pos = 0;
             caller_pcb->fd_arr[i].file_op_table_pointer = &dir_jump;
             // call open
             sec_check = dir_jump.open(&(caller_pcb->fd_arr[i]), filename);
         }
         else if ( dentry.file_type == 2){
             // file
             caller_pcb->fd_arr[i].inode = dentry.inode_num;
             caller_pcb->fd_arr[i].file_pos = 0;
             caller_pcb->fd_arr[i].file_op_table_pointer = &file_jump;
             // call open
             sec_check = file_jump.open(&(caller_pcb->fd_arr[i]), filename);
         }
         else {
             //printf("opening invalid filetype");
             return -1;
         }
         if (sec_check == -1) return -1; // check if opened failed
         return i;
        }
        // use pcb.filearray later
        else if (caller_pcb->fd_arr[i].inode == dentry.inode_num){
            // file is already opened
            return -1;
        }
    }
    return -1; // returns -1 on failure
}
int32_t close (int32_t fd){
    // returns 0 on success
    //  printf("close systemcall called");

    // get current pcb
    pcb_t * caller_pcb;
    caller_pcb = get_last_pcb();

    if (caller_pcb->fd_arr[fd].flags == 1 && fd >= 0 && fd < 8){
        int check;
        check = (caller_pcb->fd_arr[fd].file_op_table_pointer->close(file_array[fd]));
        if (check == -1){
            return -1; // returns -1 on failure
        }

        caller_pcb->fd_arr[fd].file_op_table_pointer = NULL;
        caller_pcb->fd_arr[fd].file_pos = NULL;
        caller_pcb->fd_arr[fd].flags = 0;
        caller_pcb->fd_arr[fd].inode = NULL;
        return 0;
    }
    return -1; // returns -1 on failure
}

pcb_t *get_last_pcb(void){
  pcb_t *last;
  asm volatile("andl %%esp, %0"
                 : "=r" (last)
                 : "r" (PCB_MASK)
               );
  return last;
}

int32_t getargs(uint32_t* buf, int32_t nbytes) {
    if(argsPresent==0){
      return -1;
    }
    return 0;
}
// CP4
/*

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

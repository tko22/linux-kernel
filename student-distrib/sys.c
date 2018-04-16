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

uint32_t arglength =0;
int argspresent = 0;

/* int32_t halt(uint8_t status);
 * Inputs: uint8_t status
 * Return Value: 0
 * Function: End process, restore to parent process
 */
int32_t halt(uint8_t status) {

    //pointers for current and parent process
    pcb_t* curr;
    pcb_t* parent;

    curr = get_last_pcb();                      //assign to respective process
    parent = curr->parent;
    load_program(parent->pid);


    tss.esp0 = parent->esp0;                    //set esp0 and ss0 to parent esp0 and ss0.
    tss.ss0 = parent->ss0;

    int i;
    for(i = 2; i < FILES; i++){                 //close all the files
        close(i);
    }

    nump--;                                     //set the current pid to not in used
                                                //make sure parent pid is the one in use

    if(nump == 0){                              //execute another shell when trying to halt first process
            execute((uint8_t *)"shell");
    }
    curr->status = (uint32_t)status;
    asm volatile(                                         //restore the registers for execute
                "movl %0, %%ebp		#Restore EBP	  \n"
                "movl %1, %%esp    #Restore ESP 	\n"
                :
                : "r" (curr->ebp), "r" (curr->esp)
                : "memory","eax"
                );
    // printf("Restore ESP and EBP, going to IRET\n");
    asm volatile("jmp halt_ret");        //jmp to halt_ret in execute
    return 0;

}

/* execute(const uint8_t* command);
 * Inputs: uconst uint8_t* command
 * Return Value: -1 for errors
 * Function:
 */
int32_t execute(const uint8_t* command){
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
    if(nump == 6){
      return -1;
    }
    nump++;
    curr.pid = nump;

    curr.fd_arr[0].file_op_table_pointer = &stdin_jump;
    curr.fd_arr[0].flags = 1;

    curr.fd_arr[1].file_op_table_pointer = &stdout_jump;
    curr.fd_arr[1].flags = 1;
    //parse the command
    int i;
    argspresent = 0;
    //int cmdcopied = 0;
    for(i=0;i<128;i++){ //clear argsbuffer which is 128 character long
      curr.argsbuffer[i] = '\0';
    }
    for(i = 0;i < strlen((char*)command); i++){
        if(command[i] != ' ' && command[i] != '\0' ){ // there is args
            filename[i] = command[i];
        }
        else {
            break;
        }
    }
    filename[i] = '\0';
    if (command[i] == ' '){
        argspresent = 1;
        int j;
        while (command[i] == ' '){
            i++;
        }
        for ( j = i; j < strlen((char*)command); j++){
            curr.argsbuffer[j-i] = command[j];
        }
        while (command[j] == ' '){
            curr.argsbuffer[j-i] = '\0';
            j++;
        }
    }

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


    // if(cmdcopied == 0){ //there is no args
    //    argspresent = 0;
    //    strncpy(filename,(char*)command,strlen((char*)command));
    //    filename[strlen((char*)command)] = '\0';
    // }

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

/* int32_t read (int32_t fd, void* buf, int32_t nbytes);
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Return Value: -1 for errors, ret
 * Function:
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes){
    // returns number of bytes read
  //  printf("read systemcall called\n");
    if(fd < 0 || fd >= 1073741823){
      return -1;
    }
    // get current pcb
    pcb_t * caller_pcb;
    caller_pcb = get_last_pcb();
    // then, check if file is in use or whether fd is in bounds
    if (caller_pcb->fd_arr[fd].flags == 0){
        return -1;
    }
    if (fd >= 0 && fd < 8 && caller_pcb->fd_arr[fd].flags == 1){
        int32_t ret = (caller_pcb->fd_arr[fd].file_op_table_pointer->read(&(caller_pcb->fd_arr[fd]), buf, nbytes));
        return ret;
    }

    return -1; // returns 0 if fail - initial file position is at or beyond EOF for normal files
}

/* int32_t write (int32_t fd, const void* buf, int32_t nbytes);
 * Inputs: int32_t fd, const void* buf, int32_t nbytes
 * Return Value: -1 for errors, ret
 * Function:
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    // returns number of bytes written
  //  printf("write systemcall called");
    if(fd < 0 || fd >= 1073741823){
      return -1;
    }
    // get current pcb
    pcb_t * caller_pcb;
    caller_pcb = get_last_pcb();
    if (fd >= 0 && fd < 8 && caller_pcb->fd_arr[fd].flags == 1 && buf != NULL){
        int32_t ret = caller_pcb->fd_arr[fd].file_op_table_pointer->write(&(caller_pcb->fd_arr[fd]),buf,nbytes);
        return ret;
    }

    return -1; // returns -1 on failure
}

/* int32_t open (const uint8_t* filename);
 * Inputs: const uint8_t* filename
 * Return Value: -1 for errors, i
 * Function:
 */
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
        // else if (caller_pcb->fd_arr[i].inode == dentry.inode_num){
        //     // file is already opened
        //     return -1;
        // }
    }
    return -1; // returns -1 on failure
}

/* int32_t close (int32_t fd);
 * Inputs: const uint8_t* filename
 * Return Value: -1 for errors
 * Function:
 */
int32_t close (int32_t fd){
    //  returns 0 on success
    //  printf("close systemcall called");
    if(fd < 0 || fd >= 1073741823){
      return -1;
    }
    // get current pcb
    pcb_t * caller_pcb;
    caller_pcb = get_last_pcb();

    if (caller_pcb->fd_arr[fd].flags == 1 && fd >= 2 && fd < 8){
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

/* pcb_t *get_last_pcb(void);
 * Inputs: void
 * Return Value: pcb_t *last
 * Function: return the last pcb being used so you can know what process.
 */
pcb_t *get_last_pcb(void){
  pcb_t *last;
  asm volatile("andl %%esp, %0"
                 : "=r" (last)
                 : "r" (PCB_MASK)
               );
  return last;
}

/* int32_t getargs(uint8_t* buf, int32_t nbytes);
 * Inputs: uint8_t* buf, int32_t nbytes
 * Return Value: -1 or 0
 * Function: 
 */
int32_t getargs(uint8_t* buf, int32_t nbytes) {
    pcb_t * caller_pcb;
    caller_pcb = get_last_pcb();
    if (nbytes > LINE_BUFFER_LENGTH){
        nbytes = LINE_BUFFER_LENGTH;
    }
    if(strlen((int8_t *)caller_pcb->argsbuffer) > nbytes){
		    return -1;
    }

    // copy nbytes of data to buffer
    int i;
    for (i = 0; i < nbytes; i++){
      if (caller_pcb->argsbuffer[0] == '\0'){
          // checking if there are args
          return 0;
        }
        buf[i] = caller_pcb->argsbuffer[i];
    }

    /*if (!(caller_pcb->argsbuffer[i-1] == '\0')){
        // check if arguments do not fit the buffer aka nbytes is too small
        return -1;
    }*/
    return 0;

}


int32_t vidmap(uint8_t** screen_start){
    if(screen_start == NULL || (uint32_t)screen_start < FOUR_MB * 2){
      return -1;
    }
    pcb_t *curr = get_last_pcb();
    *screen_start = init_vidmap(curr->pid);
    return 0;
}
int32_t set_handler(int32_t signum, void* handler_address){
    return -1;
}
extern int32_t sigreturn(void){
    return -1;
}

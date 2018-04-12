#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "types.h"
#include "fs.h"
#include "rtc.h"
#include "keyboard.h"
#include "file_desc.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}
/* Test suite entry point */
void test_fs(){
	// 	struct fd_t fd;
	// uint8_t buffer[MAX_NAME_LENGTH];
	// uint32_t length = MAX_NAME_LENGTH;
	// int i;
	// for (i =0; i < 17; i++){
	// 	fd.file_pos = i;
	// 	dir_read(&fd,buffer,length);
	// 	printf("%s \n",buffer);
	// 	int j;
	// 	for (j = 0; j < MAX_NAME_LENGTH; j++){
	// 		buffer[j] = '\0';
	// 	}
	// }
	fd_t fd;
	// dentry_t dt;
	uint8_t temp[4] = "fish";
	uint32_t temp_int = 4;
	rtc_jump.open(&fd,temp); // just to stop erros for now
	stdin_jump.write(&fd,temp,4);
	stdout_jump.read(&fd,temp,temp_int);
	// open(temp);
	// FILE TEST
	// printf("TESTING file read \n");
	// uint8_t buffer[1920];
	// uint32_t length = 1920;

	// read(2,buffer,length);
	// int i;
	// inode_t* thisinode = ((void*)boot_block + (dt.inode_num + 1) * BLOCK_SIZE);
	// for(i=0;i<thisinode->length;i++){ //print out using put c
	// 	if(i>1920) break; //quit if it exceeds our buffer
	// 	putc(buffer[i]);// put to the screen
	// }


	// DIRECTORY TEST
	// uint8_t buffer[MAX_NAME_LENGTH];
	// uint32_t length = MAX_NAME_LENGTH;
	// int i;
	// for (i =0; i < MAX_NAME_LENGTH; i++){
	// 	buffer[i] = NULL;
	// }
	// for (i =0; i < 17; i++){
	// 	fd.file_pos = i;
	// 	dir_jump.read(&fd,buffer,length);

	// 	printf("%s \n",buffer);
	// 	int j;
	// 	for (j = 0; j < MAX_NAME_LENGTH; j++){
	// 		buffer[j] = '\0';
	// 	}
	// }

}
	// void test_rtc(){
	//       int val;
	//       int32_t test_buf;
	//       //printf("\nTesting RTC");
	//       test_buf= 64;                     //The buffer which holds the frequency
	//       fd_t* test_file;
	//       write_rtc(test_file, (const char*)&test_buf, 4);    //set rate
	// 	    char name[5] = "";
	//       open_rtc(test_file, name);                                         //call open to set default frequency
	//       while(1){
	//         val = read_rtc(test_file, 0, 0);    //use read_rtc to check for interrupts
	//         if(val == 0){
	//           printf("READ/WRITE CHECK ");            //print statement to check
	//         }
	//       }
	// }

void launch_tests(){
	clear();
	//----------------CHECKPOINT 3----------------------
	test_fs();
//	test_rtc();
}

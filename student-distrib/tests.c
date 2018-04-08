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
	fd_t fd;
	dentry_t dt;
	read_dentry_by_index(0,&dt);
	uint8_t temp[4] = "fish";
	rtc_jump.open(&fd,temp); // just to stop erros for now
	open('fish'); // opening fish
	// FILE TEST
	printf("TESTING file read \n");
	uint8_t buffer[1920];
	uint32_t length = 1920;

	read(3,buffer,length);
	int i;
	inode_t* thisinode = ((void*)boot_block + (dt.inode_num + 1) * BLOCK_SIZE);
	for(i=0;i<thisinode->length;i++){ //print out using put c
		if(i>1920) break; //quit if it exceeds our buffer
		putc(buffer[i]);// put to the screen
	}


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
void launch_tests(){
	clear();
	//----------------CHECKPOINT 3----------------------
	test_fs();
}

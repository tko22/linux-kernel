#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "types.h"
#include "fs.h"
#include "rtc.h"
#include "keyboard.h"

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


/* Checkpoint 1 tests */

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/*
  int page_values_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){										//check first ten values of page
		if ((page_directory[i] == NULL) &&
			(page_table[i] == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}
*/
//testing page faults
//derefrecing address present
void page_address_test(){
	//null pointer
	/*int* addr = NULL;
	int deref = *addr;*/
	// this should blue screen
	/*int* addr = 0x895888;
	int deref = *addr;*/
}

/*int garbage_test(){

}*/



/* Checkpoint 2 tests */
// -------------- FILE SYSTEM TEST ---------------------
void read_dentry_by_index_test(){
	TEST_HEADER;
	clear();
	printf("start testing read_dentry_by_index\n");
	int32_t a;
	int i;
	dentry_t testdentry;
	dentry_t* pointertest = &testdentry;
	for(i=0;i<17;i++){ // tring to read all index
			printf("testdentry:%d\n",i);
			a=read_dentry_by_index(i,pointertest);
			printf("filename from readdentry:%s\n",pointertest->file_name);
			printf("file_type from readdentry:%x\n",pointertest->file_type);
			printf("inode_num from readdentry:%x\n",pointertest->inode_num);
			clear();
	}
	a=read_dentry_by_index(18,pointertest); // out of bound of total inode
	printf("a return from 18 is:%d",a);
	if(a==0){// faile if yo ucan read
		assertion_failure();
	}
}
void read_dentry_by_name_test(){
	clear();
	dentry_t testdentry;
	dentry_t* pointertest = &testdentry;
	uint8_t teststring[34] = "verylargetextwithverylongname.txt";
	int32_t a;
	a=read_dentry_by_name(teststring,pointertest); // trying to read verylargetextwithverylongname.txt (it should fail)
	printf("filename from readdentry:%s\n",pointertest->file_name);
	printf("file_type from readdentry:%x\n",pointertest->file_type);
	printf("inode_num from readdentry:%x\n",pointertest->inode_num);
	if(a==0){// faile if yo ucan read verylargetextwithverylongname.txt
		assertion_failure();
	}
	uint8_t teststring2[33] = "verylargetextwithverylongname.tx";
	a=read_dentry_by_name(teststring2,pointertest);
	printf("filename from readdentry:%s\n",pointertest->file_name);
	printf("file_type from readdentry:%x\n",pointertest->file_type);
	printf("inode_num from readdentry:%x\n",pointertest->inode_num);
	if(a!=0){// fail if you can't read ....txt
		assertion_failure();
	}
}
void read_data_test(uint32_t inode){
	uint32_t offset=0;
	uint8_t buffer[1920];
	uint32_t length = 1920; // it's the entire screen height * width = 24*80 (times ten to make sure it will fit)
	int i=0;
	for(i=0;i<1920;i++){
		buffer[i] = NULL;
	}
	int32_t a;
	a = read_data(inode,offset,buffer,length);
		// VVV Comment this out to use write from keyboard
	inode_t* thisinode = ((void*)boot_block + (inode + 1) * BLOCK_SIZE);
	for(i=0;i<thisinode->length;i++){ //print out using put c
		if(i>1920) break; //quit if it exceeds our buffer
		putc(buffer[i]);// put to the screen
	}
	//keyboard_write(0,(char*)buffer, 1920); //print out stuff to the screen using keyboard write function
}
void print_by_name(uint8_t* filename){
	dentry_t testdentry;
	dentry_t* pointertest = &testdentry;
	int32_t a;
	a=read_dentry_by_name(filename,pointertest);
	printf("\nprint by name filename give:%s\n",pointertest->file_name);
	read_data_test(pointertest->inode_num);
}
void list_all_files(){
	int i=0;
	for(i=0;i<17;i++){
		dentry_t testdentry;
		dentry_t* pointertest;
		pointertest=&testdentry;
		read_dentry_by_index(i,pointertest);
		uint32_t inode_num = pointertest->inode_num;
		inode_t* thisinode = ((void*)boot_block + (inode_num + 1) * BLOCK_SIZE); // pointer to the given inode index
		printf("file_name:%*s, file_type:%*d, file_size:%d\n",pointertest->file_name,pointertest->file_type,thisinode->length);
	}
}

/* void test_rtc();
 * Inputs: Nothing
 * Return Value: Nothing
 * Function: Print statements at different rates to check rtc read, write, and open.
 */
//------------RTC TEST--------------
// void test_rtc(){
// 	int i;
// 	int32_t test_buf;
// 	printf("\n Testing RTC");
// 	test_buf= 128;
// 	int32_t test_file;
// 	write_rtc(test_file, (const char*)&test_buf, 4);
// 	for(i = 0; i < 11; i++){
// 		read_rtc(test_file, NULL, 0);
// 	}
// }

//------------------------------------


/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */

void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	//printf("Testing paging....");
//	page_address_test();

	//------------------------CHECKPOINT 2---------------

	//read_dentry_by_index_test();
	//read_dentry_by_name_test();
 	//read_data_test();
	// ---file test for checkpoint 2
	clear();
	uint32_t i=0;
	uint8_t testfilename[17][33] = {".","sigtest","shell","grep","syserr","rtc","fish","counter","pingpong","cat",
	"frame0.txt","verylargetextwithverylongname.tx","ls","testprint","created.txt","frame1.txt","hello"};
	//list_all_files(); //TEST 1 list out files
	for(i=0;i<17;i++){ //TEST 2 print out every file name
		clear();
		printf("print by filename:%s",testfilename[i]); // BREAK HERE TO SHOW IF IT WORKS
		print_by_name(testfilename[i]);
	}
	for(i=0;i<17;i++){ //TEST 4print out every file name by inode index
		dentry_t testdentry;
		dentry_t* pointertest = &testdentry;
		int32_t a;
		a=read_dentry_by_index(i,pointertest);
		clear();
		printf("print by index :%d\n",i); //BREAK HERE TO SHOW IF IT WORKS
		read_data_test(pointertest->inode_num);

	}
	//print_by_name(testfilename[0]) //TEST 2 print out by filename
	//read_data_test(1); // TEST 4 read and printfiles by inode index
	//read_dentry_by_index_test();



	//test_rtc();
}

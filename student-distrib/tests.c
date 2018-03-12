#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "types.h"

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

/*int rtc_test(){


}

int keyboard_test(){

}*/
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
<<<<<<< HEAD
	/*int* addr = 0x895888;
=======
	/*
	int* addr = NULL;
>>>>>>> keyboard cursor first draft
	int deref = *addr;*/
}

/*int garbage_test(){

}*/


// add more tests here

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */

void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	//printf("Testing paging....");
	page_address_test();
}

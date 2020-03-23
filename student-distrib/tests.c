#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

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
	// asm volatile("int $15");
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
    // int* ptr = NULL;
    // *ptr = 4;
    // asm volatile("ud2");

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

// add more tests here
int breakpoint_test(){
    TEST_HEADER;
    asm volatile("int $3");
    return FAIL;
}
int invalid_opcode_test(){
    TEST_HEADER;
    asm volatile("ud2");
    return FAIL;
}
int device_not_available_test(){
    TEST_HEADER;
    asm volatile("fwait");
    return FAIL;
}
int divide_by_zero_test(){
    TEST_HEADER;
    int i;
    int j = 1;
    j--;
    i = 1 / j;
    return FAIL;
}
int overflow_test(){
    TEST_HEADER;
    int8_t i = 0x7f;
    i += 1;
    asm volatile("into");
    return FAIL;
}
/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// TEST_OUTPUT("idt_test", idt_test());
    // TEST_OUTPUT("breakpoint_test", breakpoint_test());
    // TEST_OUTPUT("invalid_opcode_test", invalid_opcode_test());
    // TEST_OUTPUT("divide_by_zero_test", divide_by_zero_test());
    // TEST_OUTPUT("device_not_available_test", device_not_available_test());
    // TEST_OUTPUT("overflow_test", overflow_test());




	// launch your tests here
}

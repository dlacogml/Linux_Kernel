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
int divide_by_zero_test(){
    TEST_HEADER;
    int i;
    int j = 1;
    j--;
    i = 1 / j;
    return FAIL;
}
int breakpoint_test(){
    TEST_HEADER;
    asm volatile("int $3");
    return FAIL;
}
int overflow_test(){
    TEST_HEADER;
    // int8_t i = 0x7f;
    // i += 1;
    asm volatile("movl $0x7f, %eax; incl %eax; into");
    return FAIL;
}
int bound_exceeded_test(){
    TEST_HEADER;
    int arr[2];
    arr[3] = 0;
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
int double_fault_test(){
    TEST_HEADER;
    asm volatile("int $32");
    return FAIL;
}
int invalid_tss_test(){
    TEST_HEADER;
    asm volatile("fwait");
    return FAIL;
}
// int segment_not_present_test(){
//     TEST_HEADER;
//     asm volatile("mov $30, %ss");
//     return FAIL;
// }
int stack_segment_test(){
    TEST_HEADER;
    asm volatile("fwait");
    return FAIL;
}
int general_protection_test(){
    TEST_HEADER;
    asm volatile("int $60");
    return FAIL;
}
int page_fault_test(){
    TEST_HEADER;
    int* ptr = NULL;
    *ptr = 1;
    return FAIL;
}
int math_fault_test(){
    TEST_HEADER;
    asm volatile("fwait");
    return FAIL;
}
int alignment_check_test(){
    TEST_HEADER;
    asm volatile("fwait");
    return FAIL;
}
int machine_check_test(){
    TEST_HEADER;
    asm volatile("movl %cr4, %eax; orl $0x40, %eax; movl %eax, %cr4");
    return FAIL;
}
int simd_check_test(){
    TEST_HEADER;
    asm volatile("fwait");
    return FAIL;
}
int system_call_test(){
    TEST_HEADER;
    asm volatile("int $0x80");
    return PASS;
}


/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// TEST_OUTPUT("idt_test", idt_test());
    // TEST_OUTPUT("divide_by_zero_test", divide_by_zero_test());
    // TEST_OUTPUT("breakpoint_test", breakpoint_test());
    // TEST_OUTPUT("overflow_test", overflow_test()); // not working
    // TEST_OUTPUT("bound_exceeded_test", bound_exceeded_test()); // not working
    // TEST_OUTPUT("invalid_opcode_test", invalid_opcode_test());
    // TEST_OUTPUT("device_not_available_test", device_not_available_test()); // not working
    // TEST_OUTPUT("double_fault_test", double_fault_test()); // not working
    // TEST_OUTPUT("invalid_tss_test", invalid_tss_test()); // not working
    // TEST_OUTPUT("segment_not_present_test", segment_not_present_test()); // not working
    // TEST_OUTPUT("stack_segment_test", stack_segment_test()); // not working
    // TEST_OUTPUT("general_protection_test", general_protection_test());
    // TEST_OUTPUT("page_fault_test", page_fault_test());
    // TEST_OUTPUT("math_fault_test", math_fault_test()); // not working
    // TEST_OUTPUT("alignment_check_test", alignment_check_test()); // not working
    // TEST_OUTPUT("machine_check_test", machine_check_test()); // not working
    // TEST_OUTPUT("simd_check_test", simd_check_test()); // not working
    // TEST_OUTPUT("system_call_test", system_call_test());





	// launch your tests here
}

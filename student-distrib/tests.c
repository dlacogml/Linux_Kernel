#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "filesystem.h"

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
int idt_test() {
	TEST_HEADER;
	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)) {
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here
/*
 * divide_by_zero_test()
 * Description: tests the divide by zero exception
 */
int divide_by_zero_test(){
    TEST_HEADER;
    int i;
    int j = 1;
    j--;
    i = 1 / j;
    return FAIL;
}
/*
 * breakpoint_test()
 * Description: tests the breakpoint exception
 */
int breakpoint_test(){
    TEST_HEADER;
    asm volatile("int $3");
    return FAIL;
}
/*
 * overflow_test()
 * Description: tests the overflow exception
 */
int overflow_test(){
    TEST_HEADER;
    asm volatile("int $4");
    return FAIL;
}
/*
 * bound_exceeded_test()
 * Description: tests the bounds exceeded exception
 */
int bound_exceeded_test(){
    TEST_HEADER;
    asm volatile("int $5");
    return FAIL;
}
/*
 * invalid_opcode_test()
 * Description: tests the invalid opcode exception
 */
int invalid_opcode_test(){
    TEST_HEADER;
    asm volatile("ud2");
    return FAIL;
}
/*
 * device_not_available_test()
 * Description: tests the device not available exception
 */
int device_not_available_test(){
    TEST_HEADER;
    asm volatile("int $7");
    return FAIL;
}
/*
 * double_fault_test()
 * Description: tests the double fault exception
 */
int double_fault_test(){
    TEST_HEADER;
    asm volatile("int $8");
    return FAIL;
}
/*
 * invalid_tss_test()
 * Description: tests the invalid TSS exception
 */
int invalid_tss_test(){
    TEST_HEADER;
    asm volatile("int $10");
    return FAIL;
}
/*
 * segment_not_present_test()
 * Description: tests the segment not present exception
 */
int segment_not_present_test(){
    TEST_HEADER;
    asm volatile("int $11");
    return FAIL;
}
/*
 * stack_segment_test()()
 * Description: tests the stack segment exception
 */
int stack_segment_test(){
    TEST_HEADER;
    asm volatile("int $12");
    return FAIL;
}
/*
 * general_protection_test()
 * Description: tests the general protection exception
 */
int general_protection_test(){
    TEST_HEADER;
    asm volatile("int $13");
    return FAIL;
}
/*
 * page_fault_test()
 * Description: tests the page fault exception
 */
int page_fault_test(){
    TEST_HEADER;
    int* ptr = NULL;
    *ptr = 1;
    return FAIL;
}
/*
 * math_fault_test()
 * Description: tests the divide by zero exception
 */
int math_fault_test(){
    TEST_HEADER;
    asm volatile("int $16");
    return FAIL;
}
/*
 * alignment_check_test()
 * Description: tests the divide by zero exception
 */
int alignment_check_test(){
    TEST_HEADER;
    asm volatile("int $17");
    return FAIL;
}
/*
 * machine_check_test()
 * Description: tests the machine check test exception
 */
int machine_check_test(){
    TEST_HEADER;
    asm volatile("int $18");
    return FAIL;
}
/*
 * simd_check_test()
 * Description: tests the simd check test exception
 */
int simd_check_test(){
    TEST_HEADER;
    asm volatile("int $19");
    return FAIL;
}
/*
 * system_call_test()
 * Description: tests the system call test
 */
int system_call_test(){
    TEST_HEADER;
    asm volatile("int $0x80");
    return PASS;
}

/* Checkpoint 2 tests */

/*
 * test_dir_read()
 * Description: tests the dir read function by listing all dentries
 */
int test_dir_read(){
    uint8_t buf[FILENAME_LENGTH];
    int cnt;

    /* directory name */
    int8_t* filename = ".";

    /* check if valid directory */
    if (dir_open((uint8_t*)filename) == 0){

        /* print each filename */
        while (0 != (cnt = dir_read (0, buf, FILENAME_LENGTH))) {
            int i;
            for (i = 0; i < cnt; i++){
                putc(buf[i]);
            }
            putc('\n');
        }
        return PASS;
    }
    return FAIL;

}

/*
 * test_file_read()
 * Description: tests the file read function by read a file
 */
int test_file_read(){
    int32_t buf_length = 10000; // length of buffer for test

    uint8_t buf[buf_length];

    /* file name */
    int8_t* filename = "frame0.txt";

    /* check if valid file */
    if (file_open((uint8_t*)filename) == 0){

        /* read from file */
        int num_bytes = file_read(0, buf, buf_length);

        int i;

        /* print file to console */
        for (i = 0; i < num_bytes; i++){
            putc(buf[i]);
        }
        return PASS;
    }
    return FAIL;
}

/*
 * launch_tests_checkpoint_1()
 * 
 * Description: Launches tests for the 1st checkpoint
 */
void launch_tests_checkpoint_1(){
	// TEST_OUTPUT("idt_test", idt_test());
    // TEST_OUTPUT("divide_by_zero_test", divide_by_zero_test());
    // TEST_OUTPUT("breakpoint_test", breakpoint_test());
    // TEST_OUTPUT("overflow_test", overflow_test()); 
    // TEST_OUTPUT("bound_exceeded_test", bound_exceeded_test()); 
    // TEST_OUTPUT("invalid_opcode_test", invalid_opcode_test());
    // TEST_OUTPUT("device_not_available_test", device_not_available_test()); 
    // TEST_OUTPUT("double_fault_test", double_fault_test()); 
    // TEST_OUTPUT("invalid_tss_test", invalid_tss_test()); 
    // TEST_OUTPUT("segment_not_present_test", segment_not_present_test()); 
    // TEST_OUTPUT("stack_segment_test", stack_segment_test());
    // TEST_OUTPUT("general_protection_test", general_protection_test());
    // TEST_OUTPUT("page_fault_test", page_fault_test());
    // TEST_OUTPUT("math_fault_test", math_fault_test());
    // TEST_OUTPUT("alignment_check_test", alignment_check_test());
    // TEST_OUTPUT("machine_check_test", machine_check_test());
    // TEST_OUTPUT("simd_check_test", simd_check_test());
    // TEST_OUTPUT("system_call_test", system_call_test());
	// launch your tests here


    TEST_OUTPUT("test_file_read", test_file_read());
    // TEST_OUTPUT("test_dir_read", test_dir_read());
        

    


}
/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

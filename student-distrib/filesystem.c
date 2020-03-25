/* 
 * 
 * filesystem.c - functions to open/close/read files 
 * 
 */

#include "lib.h"
#include "filesystem.h"
#include "i8259.h"


/* file_open
 * DESCRIPTION: opens the file
 * INPUT: nothing
 * OUTPUT: nothing
 * SIDE EFFECT: 
 */
int32_t file_open(const uint8_t* filename) {

}

int32_t file_close(int32_t fd) {

}

int32_t file_read() {

}

int32_t file_write() {

}

int32_t dir_open() {

}

int32_t dir_close() {

}
int32_t dir_read() {

}

int32_t dir_write() {

}

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {

}

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {

}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {

}

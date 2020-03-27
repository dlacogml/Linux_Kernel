/* 
 * 
 * idt.h - Defines used in interactions with the idt interrupt
 * 
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include "lib.h"

#define FILENAME_LENGTH     32
#define BLOCK_SIZE          4096
#define EOF                 -1
#define EOS                 0
#define FILE_CODE           2
#define DIR_CODE            1

int8_t* filesys_start;

typedef struct dentry {
    uint8_t filename[FILENAME_LENGTH];
    int32_t filetype;
    int32_t inode_num;
    int8_t reserved[24];
} dentry_t;

typedef struct inode {
    int32_t length;
    int32_t data_block_num[1023];
} inode_t;

typedef struct boot_block {
    int32_t dir_count;
    int32_t inode_count;
    int32_t data_count;
    int8_t reserved[52];
    dentry_t dir_entries[63];
} boot_block_t;

typedef struct data_block {
    int8_t byte[4096];
} data_block_t;


int32_t file_open(const uint8_t* filename);
int32_t file_close(int32_t fd);
int32_t file_read(int32_t fd, uint8_t* buf, int32_t nbytes);
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t dir_open(const uint8_t* filename);
int32_t dir_close(int32_t fd);
int32_t dir_read(int32_t fd, uint8_t* buf, int32_t nbytes);
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

void init_filesystem();

#endif

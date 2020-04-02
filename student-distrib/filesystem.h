/* 
 * 
 * idt.h - Defines used in interactions with the idt interrupt
 * 
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include "lib.h"
#include "keyboard.h"

#define FILENAME_LENGTH     32
#define BLOCK_SIZE          4096
#define EOF                 -1
#define EOS                 0
#define FILE_CODE           2
#define DIR_CODE            1
#define RTC_CODE            0
#define NUM_DATA_BLOCKS     1023
#define NUM_DENTRIES        63
#define NUM_DENTRY_RESERVED 24
#define NUM_BOOT_RESERVED   52
#define NUM_FD              8
#define FILE_OPEN           0
#define FILE_CLOSED         1

/* starting address for filesystem */
int8_t* filesys_start;



/* struct for a directory entry */
typedef struct dentry {
    uint8_t filename[FILENAME_LENGTH];
    int32_t filetype;
    int32_t inode_num;
    int8_t reserved[NUM_DENTRY_RESERVED];
} dentry_t;

/* struct for inode */
typedef struct inode {
    int32_t length;
    int32_t data_block_num[NUM_DATA_BLOCKS];
} inode_t;

/* struct for boot block */
typedef struct boot_block {
    int32_t dir_count;
    int32_t inode_count;
    int32_t data_count;
    int8_t reserved[NUM_BOOT_RESERVED];
    dentry_t dir_entries[NUM_DENTRIES];
} boot_block_t;

/* struct for data block */
typedef struct data_block {
    int8_t byte[BLOCK_SIZE];
} data_block_t;

typedef struct f_ops_table{
    //pointers to read
    int32_t (*read)(int32_t fd, uint8_t* buf, int32_t nbytes);
    //pointer to write
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
    //pointer to open
    int32_t (*open)(const uint8_t* filename);
    //pointer to close
    int32_t (*close)(int32_t fd);

} f_ops_table_t;

typedef struct file_descriptor {
    f_ops_table_t* f_ops_pointer;
    int32_t inode;
    int32_t file_pos;
    int32_t flags;
} file_descriptor_t;

file_descriptor_t fdarray[NUM_FD];

/* all functions for filesystems, function interfaces in .c file */
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

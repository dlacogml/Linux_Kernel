/* 
 * 
 * filesystem.c - functions to open/close/read files 
 * 
 */

#include "lib.h"
#include "filesystem.h"
#include "i8259.h"


inode_t* inodes;
boot_block_t* boot_block;
data_block_t* data_blocks;
int flag;

void init_filesystem(){
    boot_block = (boot_block_t*) filesys_start;
    inodes = (inode_t*) (filesys_start + sizeof(boot_block_t));
    data_blocks = (data_block_t*) (filesys_start + sizeof(boot_block_t) + sizeof(inode_t) * boot_block->inode_count);
    flag = 0;
}

/* file_open
 * DESCRIPTION: opens the file
 * INPUT: nothing
 * OUTPUT: nothing
 * SIDE EFFECT: 
 */
int32_t file_open(const uint8_t* filename) {
    dentry_t* dentry;
    if (read_dentry_by_name(filename, dentry) == 0){
        if (dentry->filetype == 2 && dentry->inode_num < boot_block->inode_count && dentry->inode_num >= 0 && flag == 0){
            flag = 1;
            return 0;
        }
    }
    return -1;
}

int32_t file_close(int32_t fd) {
    if (flag == 1){
        flag = 0;
        return 0;
    }
    return -1;
}

int32_t file_read(int32_t fd, void* buf, int32_t nbytes) {

}

int32_t file_write(int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}

int32_t dir_open(const uint8_t* filename) {
    dentry_t* dentry;
    if (read_dentry_by_name(filename, dentry) == 0){
        if (dentry->filetype == 1 && dentry->inode_num < boot_block->inode_count && dentry->inode_num >= 0){
            return 0;
        }
    }
    return -1;
}

int32_t dir_close(int32_t fd) {
    return 0;
}
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes) {

}

int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {
    int i;
    for (i = 0; i < boot_block->dir_count; i++){
        if (boot_block->dir_entries[i].filename == fname){
            strcpy(dentry->filename, boot_block->dir_entries[i].filename);
            dentry->filetype = boot_block->dir_entries[i].filetype;
            dentry->inode_num = boot_block->dir_entries[i].inode_num;
            return 0;
        }
    }
    return -1;
}

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {
    int i;
    for (i = 0; i < boot_block->dir_count; i++){
        if (boot_block->dir_entries[i].inode_num == index){
            strcpy(dentry->filename, boot_block->dir_entries[i].filename);
            dentry->filetype = boot_block->dir_entries[i].filetype;
            dentry->inode_num = boot_block->dir_entries[i].inode_num;
            return 0;
        }
    }
    return -1;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    if (inode >= boot_block->inode_count && inode < 0){
        return -1;
    }
    inode_t file_inode = inodes[inode];
    int32_t data_block_idx = offset / 4096;
    if (data_block_idx >= file_inode.length){
        return -1;
    }
    data_block_t* block = &data_blocks[file_inode.data_block_num[data_block_idx]];
    int32_t data_offset = offset % 4096;
    int i;
    for (i = data_offset; i < data_offset + length; i++){
        if (*buf == -1){ // end of file
            return 0;
        }
        *buf = block->byte[i];
        buf++;
    }
}

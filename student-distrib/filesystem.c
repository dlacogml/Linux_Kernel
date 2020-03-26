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
int open_flag;
int dir_index;

void init_filesystem(){
    /* init bootblock */
    boot_block = (boot_block_t*) filesys_start;

    /* init inode array */
    inodes = (inode_t*) (filesys_start + sizeof(boot_block_t));

    /* init data block array */
    data_blocks = (data_block_t*) (filesys_start + sizeof(boot_block_t) + sizeof(inode_t) * boot_block->inode_count);
    open_flag = 0;
}

/* file_open
 * DESCRIPTION: opens the file
 * INPUT: nothing
 * OUTPUT: nothing
 * SIDE EFFECT: 
 */
int32_t file_open(const uint8_t* filename) {
    dentry_t* dentry;
    /* check if file exists */
    if (read_dentry_by_name(filename, dentry) == 0){
        /* check if valid file */
        if (dentry->filetype == 2 && dentry->inode_num < boot_block->inode_count && dentry->inode_num >= 0 && open_flag == 0){
            open_flag = 1;
            return 0;
        }
    }
    return -1;
}

int32_t file_close(int32_t fd) {
    if (open_flag == 1){
        open_flag = 0;
        return 0;
    }
    return -1;
}

int32_t file_read(uint32_t inode_num, uint32_t position, void* buf, int32_t nbytes) {
    /* read data from file and store in buf */
    return read_data(inode_num, position, buf, nbytes);
}

int32_t file_write(int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}

int32_t dir_open(const uint8_t* filename) {
    dentry_t* dentry;
    /* check if dir exists */
    if (read_dentry_by_name(filename, dentry) == 0){
        /* check if valid dir */
        if (dentry->filetype == 1 && dentry->inode_num < boot_block->inode_count && dentry->inode_num >= 0){
            /* start from first dentry */
            dir_index = 0;
            return 0;
        }
    }
    return -1;
}

int32_t dir_close(int32_t fd) {
    return 0;
}

int32_t dir_read(int32_t fd, void* buf, int32_t nbytes) {
    uint8_t* buffer = (uint8_t*) buf;
    dentry_t dentry;

    /* read dentry, if fail return 0 */
    if (read_dentry_by_index(dir_index, &dentry) == -1){
        return 0;
    }

    /* number of bytes copied */
    int num_copied = 0;

    /* index into filename string */
    int filename_index = 0;

    /* copy filename into buffer until either limit is reached or end of filename */
    while (dentry.filename[filename_index] != '\0' && num_copied < nbytes){
        *buffer = dentry.filename[filename_index];
        buffer++;
        num_copied++;
        filename_index++;
    }

    /* go to next dentry */
    dir_index++;

    return num_copied;
}

int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {
    int i;
    /* search for file name in dentry list, then copy all fields if found, else return -1 */
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
    /* check for valid index */
    if (index >= boot_block->dir_count || index < 0){
        return -1;
    }

    /* copy all fields */
    strcpy(dentry->filename, boot_block->dir_entries[index].filename);
    dentry->filetype = boot_block->dir_entries[index].filetype;
    dentry->inode_num = boot_block->dir_entries[index].inode_num;
    return 0;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {

    /* check for valid inode index */
    if (inode >= boot_block->inode_count || inode < 0){
        return -1;
    }

    /* inode for given inode index */
    inode_t file_inode = inodes[inode];

    /* data block index in inode */
    int32_t data_block_idx = offset / BLOCK_SIZE;

    /* check for valid data block */
    if (data_block_idx >= file_inode.length){
        return -1;
    }

    /* pointer to current block in data block array*/
    data_block_t* block = &data_blocks[file_inode.data_block_num[data_block_idx]];

    /* offset from start of block */
    int32_t data_offset = offset % BLOCK_SIZE; 

    /* number of bytes copied already */
    int num_copied = 0;

    /* loop to copy bytes, stop when num_copied = length */
    while (num_copied < length){
        /* if end of file, break */
        if (block->byte[data_offset] == EOF){
            break;
        }

        /* if bytes go past current block, go to next block */
        if (data_offset == BLOCK_SIZE){
            data_offset = 0;
            block = &data_blocks[file_inode.data_block_num[data_block_idx + 1]];
        }

        /* copy from block to buffer, increment indices */
        *buf = block->byte[data_offset];
        buf++;
        data_offset++;
        num_copied++;
    }

    return num_copied;
}

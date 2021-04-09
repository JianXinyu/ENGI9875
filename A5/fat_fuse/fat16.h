/*
    Wojciech Pratkowiecki II UWr
    FAT16 FUSE driver
*/
#ifndef fat16_h
#define fat16_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>

#define min(x, y) ((x) < (y) ? (x) : (y))
#define INITIAL_CAPACITY 512
/*
    FAT16 tutorials:
        http://www.maverick-os.dk/FileSystemFormats/FAT16_FileSystem.html
        http://www.tavi.co.uk/phobos/fat.html

    boot_block - structure describing the file system's boot block
*/
typedef struct boot_block
{
    uint8_t     bootstrap_prog [3];
    char        manufacturer [8];
    uint16_t    bytes_per_block;
    uint8_t     blocks_per_claster;
    uint16_t    reserved_blocks;
    uint8_t     fats;
    uint16_t    root_entries;
    uint16_t    blocks_in_disk;
    uint8_t     media_descriptor;
    uint16_t    blocks_per_fat;
    uint16_t    blocks_per_track;
    uint16_t    num_heads;
    uint32_t    num_hidden_sectors;
    uint32_t    blocks_in_disk_large;
    uint16_t    drive_num;
    uint8_t     extended_boot_rec;
    uint32_t    vol_serial_num;
    char        volume_label [11];
    char        file_sys_id [8];
    uint8_t     bootstrap_code[448];
    uint8_t     boot_block_signature [2];
    
}__attribute__((packed)) boot_block;

/*
    root_directory - directory entry structure
*/

typedef struct root_directory
{
    char        filename [8];
    char        filename_extension [3];
    uint8_t     file_attributes;
    char        nt_bits;
    uint8_t     create_time_ms;
    uint16_t    create_time;
    uint16_t    create_date;
    uint16_t    access_date;
    uint16_t    reserved_fat;
    uint16_t    modify_time;
    uint16_t    modify_date;
    uint16_t    starting_cluster;
    uint32_t    file_size;

}__attribute__((packed)) root_directory;

/*
    fat16_attr - file attributes
*/

typedef struct fat16_attr
{
    uint8_t     read_only;
    uint8_t     hidden;
    uint8_t     system;
    uint8_t     volume_name;
    uint8_t     directory;
    uint8_t     achieve_flag;

} fat16_attr;

typedef struct inode_list inode_list;
/*
inode_entry - structure holding the main information about a given file:
     - inode number
     - attributes
     - directory entry
     - information about prior visit (applies to catalgae)
     - list of files in a directory (applies to directories)
*/
typedef struct inode_entry
{
    uint64_t         inode;
    fat16_attr       *inode_attr;
    root_directory   *dir;

    uint8_t          visited;
    inode_list       *entries_list;

} inode_entry;

/*
    inode_list - list of inode_entry structures
*/

struct inode_list
{
    inode_entry *inode;
    struct inode_list *next;
};

/*
inode_vector - structure representing the vector of inodes.
     All visited files are stored in this vector
*/

typedef struct inode_vector
{
    uint32_t    capacity;
    uint32_t    size;
    inode_entry *vector;

} inode_vector; 

/*
fat16_filesystem - main program structure holding:
     - pointer to filesystem
     - boot_block structure appearance
     - FATs
     - data to be calculated from the boot block
     - vector of inodes
     - inode_index - the number of the last inode assigned
*/

typedef struct fat16_filesystem
{
    FILE         *fs;
    boot_block   fs_boot_block;
    uint16_t     fat_offset;
    uint16_t     fat_size;
    uint16_t     root_dir_start;
    uint16_t     root_dir_entries;
    uint16_t     root_dir_size;
    uint16_t     root_dir_blocks;
    uint16_t     data_offset;
    uint16_t     cluster_size;

    uint16_t     *file_allocation_table;

    inode_vector vector;
    uint64_t     inode_index;

} fat16_filesystem;

/*
    Initial functions to initiate and complete structures that hold information about the system to be mounted
*/
void open_filesystem ( fat16_filesystem *fs );
fat16_attr *read_entry_attr ( root_directory *dir );
/*
    Supplementing information about pointers for a given file
*/
nlink_t set_nlink( inode_entry *entry );
/*
    Support for the vector of inode entry structures and single structure instances
*/
inode_entry *init_inode ( uint64_t inode_num, root_directory *dir );
void init_vector ( inode_vector *vector );
void vector_push ( inode_vector *vector, inode_entry *entry );
inode_entry *get_inode_entry ( uint64_t ino, fat16_filesystem *fs );
inode_entry *inode_lookup ( inode_entry *parent, const char *name );
void set_readdir_entries ( fat16_filesystem *fs, inode_entry *dir_entry );
inode_list *list_push ( inode_list *list, inode_entry *ino );
/*
    Navigating the file that is the fat16 system
*/
int seek_to_root_dir ( fat16_filesystem *fs );
int seek_to_cluster ( fat16_filesystem *fs, uint16_t cluster );

void read_file ( fat16_filesystem *fs, inode_entry *entry, char *buff, size_t size, off_t off );
/*
    Support for fat16 compliant formats
*/
void read_date ( struct tm *tm, uint16_t fat16_date );
void read_time ( struct tm *tm, uint16_t fat16_time );
void get_date ( struct stat *st, root_directory *dir );
char* format_filename( root_directory *dir );


#endif /* fat16_h */

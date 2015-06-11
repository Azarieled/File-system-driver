#ifndef FILESYSTEMDRIVER
#define FILESYSTEMDRIVER

#include <stdint.h>

// device properties
#define VOLUME_SIZE     (uint64_t) 512 * 1024        // 512  KiB
#define DEVICE_FD_COUNT VOLUME_SIZE / 4096           // why not?
#define BLOCK_SIZE      4096                         // 4    KiB
#define BLOCK_COUNT     (VOLUME_SIZE / BLOCK_SIZE)   // 2^17

// filesystem properties
#define FS_ID                       0xACDC007
#define BAD_FD_ID                   UINT32_MAX
#define ROOT_FD_ID                  0
#define HEADER_BLOCK_NUM            0
#define HEADER_BLOCK_COUNT          1

#define MAX_FILE_SIZE               ((uint64_t) 1) << 44
#define MAX_FILE_COUNT              UINT32_MAX
#define MAX_BLOCK_COUNT             UINT32_MAX
#define MAX_FILES_IN_DIRECTORY      UINT32_MAX
#define MAX_HARDLINK_COUNT          1 << 26
#define MAX_ABSOLUTE_FILE_NAME_SIZE 4095          // symlink can be written in 1 block
#define MAX_FILE_NAME_SIZE          256 - 32 - 1  // 256 - 20 - 1, 20 - descriptor_id, 1 - \0 symbol

typedef enum { FREE_DESCRIPTOR, FILE_DESCRIPTOR, DIRECTORY_DESCRIPTOR, SYMLINK_DESCRIPTOR } fd_type_t;

typedef struct
{
  uint64_t  fs_id;
  uint64_t  block_count;
  uint32_t  block_size;
  uint32_t  fd_count;
  uint64_t  bitmap_block_num;
  uint64_t  fd_block_num;
  uint64_t  data_block_num;
} fs_header_t;

typedef struct
{
  uint64_t    creation_date;
  uint64_t    modification_date;
  uint64_t    size;
  fd_type_t   type            : 2;
  uint64_t    hard_link_count : 38;
  uint8_t     owner_mode;
  uint8_t     group_mode;
  uint8_t     other_mode;
  uint32_t    additional_block_num;

  union
  {
    // for file
    uint32_t file_data_links [7];

    // for dir
    struct
    {
      uint32_t parent_fd_id;
      uint32_t data_links [6];
    };
  };

} fd_t;

typedef struct numeric_fd_t
{
  uint32_t number;
  uint32_t fd_id;
  struct numeric_fd_t *next;
} numeric_fd_t;

typedef struct
{
  char     name [MAX_FILE_NAME_SIZE + 1]; // +1 for \0
  uint32_t fd_id;
} hard_link_t;

typedef char sym_link_t [MAX_ABSOLUTE_FILE_NAME_SIZE + 1]; // +1 for \0

/*
 * Returns 0 if FS mounted from existing file, 1 if new file created
 * and -1 if FS could not mount.
 */
int
mount ();

int
umount ();

int
filestat (uint32_t fd_id);

int
ls ();

int
create (char *name);

int
open(char *name, uint32_t *numeric_descriptor);

int
close (uint32_t fd);

int
read (uint32_t fd, uint64_t offset, uint64_t size, char *buffer);

int
write (uint32_t fd, uint64_t offset, uint64_t size, char *data);

int
link (char *file_name, char *link_name);

int
unlink(char *link_name);

int
truncate (char *name, uint64_t size);

int
mkdir (char *dir_name);

int
rmdir (char *dir_name);

int
cd (char *dir_name);

int
pwd ();

int
symlink (char *path_name, char *link_name);

#endif // FILESYSTEMDRIVER

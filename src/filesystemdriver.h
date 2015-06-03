#ifndef FILESYSTEMDRIVER
#define FILESYSTEMDRIVER
#include <string.h>
#include <limits.h>
#include <inttypes.h>
#include <math.h>
#include "bitmap.h"

// device properties
#define VOLUME_SIZE (uint64_t) 512 * 1024 * 1024           // 512  MiB
#define BLOCK_SIZE             4096                        // 4    KiB
#define BLOCK_COUNT            (VOLUME_SIZE / BLOCK_SIZE)  // 2^17

// filesystem properties
#define MAX_FILE_SIZE               4096          // file can be written in 1 block
#define MAX_FILES_IN_DIRECTORY      16            // directory can be written in 1 block
#define MAX_ABSOLUTE_FILE_NAME_SIZE 4095          // symlink can be written in 1 block
#define MAX_FD_COUNT                1024 * 1024
#define MAX_BLOCK_COUNT             BLOCK_COUNT
#define MAX_FILE_NAME_SIZE          251           // 256 - 20 - 1, 20 - descriptor_id, 1 - \0 symbol
#define MAX_HARDLINK_COUNT          256

typedef enum { FILE_DESCRIPTOR, DIRECTORY_DESCRIPTOR, SYMLINK_DESCRIPTOR } file_type_t;

typedef struct
{
  file_type_t type            : 2;
  uint32_t    size            : (int) ceil(log2(MAX_FILE_SIZE));
  uint32_t    hard_link_count : (int) ceil(log2(MAX_HARDLINK_COUNT));
  uint32_t    block_number    : (int) ceil(log2(MAX_BLOCK_COUNT));
} fd_t;

typedef struct
{
  char     name [MAX_FILE_NAME_SIZE + 1]; // +1 for \0
  uint32_t fd_id                    : (int) ceil(log2(MAX_FD_COUNT));
} dir_link_t;

typedef struct
{
  fd_t links [MAX_FILES_IN_DIRECTORY];
} dir_t;

typedef char symlink_t [MAX_ABSOLUTE_FILE_NAME_SIZE + 1]; // +1 for \0

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
open (char *name, uint32_t *digit_descriptor);

int
close (uint32_t fd);

int
read (uint32_t fd, uint64_t offset, uint64_t size, char **buffer);

int
write (uint32_t fd, uint64_t offset, uint64_t size, char *data);

int
link (char *file_name, char *link_name);

int
unlink (char *link_name);

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


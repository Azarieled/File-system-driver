#ifndef FILESYSTEMDRIVER
#define FILESYSTEMDRIVER
#include <string.h>
#include <limits.h>
#include <inttypes.h>
#include "bitmap.h"

// device properties
#define VOLUME_SIZE (uint64_t) 512 * 1024 * 1024           // 512  MiB
#define BLOCK_SIZE             4096                        // 4    KiB
#define BLOCK_COUNT            (VOLUME_SIZE / BLOCK_SIZE)  // 2^17

// filesystem properties
#define MAX_FD                 UINT32_MAX
#define MAX_FILE_SIZE          UINT64_MAX >> 2
#define MAX_FILE_COUNT         1024 * 1024
#define MAX_FILES_IN_DIRECTORY 34

typedef enum { FILE_DESCRIPTOR, DIRECTORY_DESCRIPTOR, SYMLINK_DESCRIPTOR } file_type_t;

typedef struct
{
  file_type_t type           : 2;
  uint64_t    size           : 62;
  uint32_t    hard_link_count;
} file_descriptor_t;

typedef struct
{

} directory_t;

void
mount ();

void
umount ();

void
filestat (uint32_t inode);

void
ls ();

void
create (char *name);

uint32_t
open (char *name);

void
close (uint32_t fd);

char *
read (uint32_t fd, uint64_t offset, uint64_t size);

void
write (uint32_t fd, uint64_t offset, uint64_t size, char *data);

void
link (char *file_name, char *link_name);

void
unlink (char *link_name);

void
truncate (char *name, uint64_t size);


#endif // FILESYSTEMDRIVER


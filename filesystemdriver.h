#ifndef FILESYSTEMDRIVER
#define FILESYSTEMDRIVER
#include <string.h>
#include <limits.h>
#include "bitmap.h"

// device properties
#define VOLUME_SIZE (unsigned long long) 16 * 1024 * 1024 * 1024  // 16 GiB
#define BLOCK_SIZE      4096                                      // 4  KiB
#define BLOCK_COUNT     (VOLUME_SIZE / BLOCK_SIZE)                // 2^22

#define DESCRIPTOR_COUNT 512 * 1024 * 1024

typedef enum { FILE_DESCRIPTOR, DIRECTORY_DESCRIPTOR, SYMLINK_DESCRIPTOR } file_type_t;

typedef struct
{
  file_type_t type;
  unsigned int hard_link_count;
  unsigned long long size;

} file_descriptor_t;

void
mount ();

void
umount ();

void
filestat (unsigned int inode);

void
ls ();

void
create (char *name);

unsigned int
open (char *name);

void
close (unsigned int fd);

char *
read (unsigned int fd, unsigned long long offset, unsigned long long size);

void
write (unsigned int fd, unsigned long long offset, unsigned long long size, char *data);

void
link (char *file_name, char *link_name);

void
unlink (char *link_name);

void
truncate (char *name, unsigned long long size);


#endif // FILESYSTEMDRIVER


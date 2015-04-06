#include <stdio.h>
#include "filesystemdriver.h"

char block_bitmap [BITNSLOTS(BLOCK_COUNT)];


void
mount ()
{
  puts ("kinda mount");
}


void
umount ()
{
  puts ("kinda umount");
}


void
filestat(unsigned int inode)
{
  if (inode >= DESCRIPTOR_COUNT)
    {
      puts ("There is no file with such inode.");
      return;
    }

  printf ("kinda filestat %d", inode);
}


void
ls ()
{

}


void
create(char *name)
{

}


unsigned int
open(char *name)
{

}


void
close(unsigned int fd)
{

}


char *
read(unsigned int fd, unsigned long long offset, unsigned long long size)
{

}


void
write(unsigned int fd, unsigned long long offset, unsigned long long size, char *data)
{

}


void
link(char *file_name, char *link_name)
{

}


void
unlink(char *link_name)
{

}


void
truncate(char *name, unsigned long long size)
{

}

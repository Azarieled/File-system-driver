#include <stdio.h>
#include "filesystemdriver.h"
#define IMG_NAME "root.img"

char g_block_bitmap [BITNSLOTS(BLOCK_COUNT)];
g_cursor;
FILE *g_img_file;

void
mount ()
{
    g_img_file = fopen(IMG_NAME, "w+");
    fread(g_block_bitmap)
}


void
umount ()
{
    fclose(g_img_file);
}


void
filestat(uint32_t inode)
{
  if (inode >= MAX_FILE_COUNT)
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


uint32_t
open(char *name)
{

}


void
close(uint32_t fd)
{

}


char *
read(uint32_t fd, uint64_t offset, uint64_t size)
{

}


void
write(uint32_t fd, uint64_t offset, uint64_t size, char *data)
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
truncate(char *name, uint64_t size)
{

}

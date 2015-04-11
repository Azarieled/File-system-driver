#include <stdio.h>
#include <stdlib.h>
#include "filesystemdriver.h"


const char *IMG_NAME = "root.img";
const char *FILE_TYPES_STR [] = {"file", "directory", "symlink"};

char *g_block_bitmap = NULL;
FILE *g_img_file = NULL;

void
mount ()
{
    g_img_file = fopen(IMG_NAME, "rb+");
    if (!g_img_file)
      {
        g_img_file = fopen(IMG_NAME, "wb+");

        //fwrite();
        return;
      }
    uint32_t size; //aray size
    fread (&size, sizeof (uint32_t), 1, g_img_file);
    g_block_bitmap = (char *) malloc ((size_t) size);
    fread (g_block_bitmap, sizeof (char), size, g_img_file);
}


void
umount ()
{
  if (fclose(g_img_file)) // if returns 0, than all is ok
    {
      puts ("Problem occured during FS unmount.");
    }
  else
    {
      puts ("FS successfully unmounted.");
    }
}


void
filestat(uint32_t fd_id)
{
  if (fd_id >= MAX_FD_COUNT)
    {
      puts ("There is no file with such inode.");
      return;
    }

  printf ("kinda filestat %d", fd_id);
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


void
mkdir (char *dir_name)
{

}


void
rmdir (char *dir_name)
{

}


void
cd (char *dir_name)
{

}


void
pwd ()
{

}


void
symlink (char *path_name, char *link_name)
{

}

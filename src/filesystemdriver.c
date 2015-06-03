#include <stdio.h>
#include <stdlib.h>
#include "filesystemdriver.h"

#define CHECK_IS_MOUNTED \
  if (!g_img_file)       \
    {                    \
      return -1;         \
    }


const char *DEFAULT_IMG_NAME = "root.img";
const char *FILE_TYPES_STR [] = {"file", "directory", "symlink"};

uint32_t  fd_count = 0;
char     *g_block_bitmap = NULL;
FILE     *g_img_file = NULL;


int
create_img ()
{

}

int
mount ()
{
  //TODO umount
  g_img_file = fopen (DEFAULT_IMG_NAME, "rb+");
  if (!g_img_file)
    {
      puts ("No FS file found, a new one created.");
      g_img_file = fopen (DEFAULT_IMG_NAME, "wb+");
      fd_count = 0;

      // some file creation problems
      if (!g_img_file)
        {
          puts ("Problem occured during FS mount.");
          return -1;
        }
      //fwrite();
      return 1;
    }
  uint32_t size; //aray size
  fread (&size, sizeof (uint32_t), 1, g_img_file);
  g_block_bitmap = (char *) malloc ((size_t) size);
  fread (g_block_bitmap, sizeof (char), size, g_img_file);
  return 0;
}


int
umount ()
{
  if (fclose (g_img_file)) // if returns 0, than all is ok
    {
      //EXCEPTION
      puts ("Problem occured during FS unmount.");
      return -1;
    }
  else
    {
      g_img_file = 0;
      puts ("FS successfully unmounted.");
      return 0;
    }
}


int
filestat(uint32_t fd_id)
{
  if (fd_id >= fd_count)
    {
      puts ("There is no file descriptor with such id.");
      return -1;
    }

  printf ("kinda filestat %d", fd_id);
}


int
ls ()
{

}


int
create(char *name)
{

}


int
open(char *name, uint32_t *digit_descriptor)
{

}


int
close(uint32_t fd)
{

}


int
read(uint32_t fd, uint64_t offset, uint64_t size, char **buffer)
{

}


int
write(uint32_t fd, uint64_t offset, uint64_t size, char *data)
{

}


int
link(char *file_name, char *link_name)
{

}


int
unlink(char *link_name)
{

}


int
truncate(char *name, uint64_t size)
{

}


int
mkdir (char *dir_name)
{

}


int
rmdir (char *dir_name)
{

}


int
cd (char *dir_name)
{

}


int
pwd ()
{

}


int
symlink (char *path_name, char *link_name)
{

}

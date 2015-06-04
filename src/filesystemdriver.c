#include <stdio.h>
#include <stdlib.h>
#include "filesystemdriver.h"

#define CHECK_IS_MOUNTED \
  if (!g_img_file)       \
    {                    \
      return -1;         \
    }                    \


const char *DEFAULT_IMG_NAME = "root.img";
const char *CURRENT_DIRECTORY = ".";
const char *PARENT_DIRECTORY  = "..";
const char *FILE_TYPES_STR [] = {"file", "directory", "symlink"};

uint32_t  fd_count = 0;
char     *g_block_bitmap = NULL;
FILE     *g_img_file = NULL;

/**
 * Creates clear img file.
 *
 * @brief create_img
 * @return -1 if fails, 1 if ok
 */
int
create_img ()
{
  g_img_file = fopen (DEFAULT_IMG_NAME, "wb+");

  // some file creation problems
  if (!g_img_file)
    {
      puts ("Problem occured during FS mount.");
      return -1;
    }

  // global header
  uint64_t *header_buffer = malloc (BLOCK_SIZE);
  *(header_buffer) = FILESYSTEM_IDENTIFIER;
  *(++header_buffer) = BLOCK_SIZE;
  *(++header_buffer) = BLOCK_COUNT;
  *(++header_buffer) = BLOCK_COUNT;
  fwrite (header_buffer, BLOCK_SIZE, 1, g_img_file);

  void *clear_buffer = calloc (1, BLOCK_SIZE);

  // bitmap

  // rounding (BLOCK_COUNT / BLOCK_SIZE) to the bigger int
  for (int i = 0; i < (BLOCK_COUNT + BLOCK_SIZE - 1) / BLOCK_SIZE ; ++i)
    {
      fwrite (clear_buffer, BLOCK_SIZE, 1, g_img_file);
    }

  // descriptors

  for (int i = 0; i < (BLOCK_COUNT + BLOCK_SIZE - 1) / BLOCK_SIZE ; ++i)
    {
      fwrite (clear_buffer, BLOCK_SIZE, 1, g_img_file);
    }

  // root directory

  fd_t root_descriptor;

  // free blocks

  for (int i = 0; i < BLOCK_COUNT - 1; ++i)
    {
      fwrite (clear_buffer, BLOCK_SIZE, 1, g_img_file);
    }
  return 1;
}

int
mount ()
{
  int result_status = 0;

  //TODO auto umount
  g_img_file = fopen (DEFAULT_IMG_NAME, "rb+");
  if (!g_img_file)
    {
      puts ("No FS file found, a new one created.");
      result_status = create_img ();
    }
  uint32_t size; //aray size
  fread (&size, sizeof (uint32_t), 1, g_img_file);
  g_block_bitmap = (char *) malloc ((size_t) size);
  fread (g_block_bitmap, sizeof (char), size, g_img_file);
  return result_status;
}


int
umount ()
{
  CHECK_IS_MOUNTED

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
  CHECK_IS_MOUNTED

  if (fd_id >= fd_count)
    {
      puts ("There is no file descriptor with such id.");
      return -1;
    }

  printf ("Descriptor %lu info:\n"
          "size: %d"
          "has %lu hardlinks", fd_id);
}


int
ls ()
{
  CHECK_IS_MOUNTED
}


int
create(char *name)
{
  CHECK_IS_MOUNTED
}


int
open(char *name, uint32_t *digit_descriptor)
{
  CHECK_IS_MOUNTED
}


int
close(uint32_t fd)
{
  CHECK_IS_MOUNTED
}


int
read(uint32_t fd, uint64_t offset, uint64_t size, char **buffer)
{
  CHECK_IS_MOUNTED
}


int
write(uint32_t fd, uint64_t offset, uint64_t size, char *data)
{
  CHECK_IS_MOUNTED
}


int
link(char *file_name, char *link_name)
{
  CHECK_IS_MOUNTED

}


int
unlink(char *link_name)
{
  CHECK_IS_MOUNTED
}


int
truncate(char *name, uint64_t size)
{
  CHECK_IS_MOUNTED
}


int
mkdir (char *dir_name)
{
  CHECK_IS_MOUNTED
}


int
rmdir (char *dir_name)
{
  CHECK_IS_MOUNTED
}


int
cd (char *dir_name)
{
  CHECK_IS_MOUNTED
}


int
pwd ()
{
  CHECK_IS_MOUNTED
}

int
symlink (char *path_name, char *link_name)
{
  CHECK_IS_MOUNTED
}

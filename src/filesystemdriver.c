#include <stdio.h>
#include <stdlib.h>
#include "filesystemdriver.h"

#define FILESYSTEM_IDENTIFIER       0xACDC007
#define ROOT_FD_ID                  1

#define CHECK_IS_MOUNTED \
if (!g_img_file)         \
  {                      \
    return -1;           \
  }                      \


// string literals
const char *DEFAULT_IMG_NAME      = "drivion.img";
const char *ROOT_DIRECTORY_STR    = "/";
const char *CURRENT_DIRECTORY_STR = ".";
const char *PARENT_DIRECTORY_STR  = "..";
const char *FILE_TYPES_STR []     = {"file", "directory", "symlink"};

// fs file ptr
FILE     *g_img_file = NULL;

// fs cached data
uint64_t  block_count;
uint32_t  block_size;
uint32_t  fd_count;
char     *g_block_bitmap;

// session data
int       working_directory_fd_id;
dir_t     working_directory;
char      working_directory_name [MAX_ABSOLUTE_FILE_NAME_SIZE + 1];

// shared buffer
const int buffer_size = BUFSIZ;
const char buffer [BUFSIZ];

// helping functions

int
create_img ();

fd_t
get_fd (int fd_id);

dir_t
get_dir (int fd_id);

void *
get_data ();

void
clear_buffer ();


/**
 * Creates clean img file.
 *
 * @brief create_img
 * @return -1 if fails, 1 if ok
 */
int
create_img ()
{
  g_img_file = fopen (DEFAULT_IMG_NAME, "wb+");
  if (!g_img_file)
    {
      // EXCEPTION
      puts ("Problem occured during FS mount.");
      return -1;
    }

  block_count = BLOCK_COUNT;
  block_size = BLOCK_SIZE;
  uint32_t  fd_count = MAX_FD_COUNT;

  // global header
  clear_buffer ();
  uint64_t *header_buff_ptr = buffer;
  *(header_buff_ptr) = FILESYSTEM_IDENTIFIER;
  *(++header_buff_ptr) = BLOCK_SIZE;
  *(++header_buff_ptr) = BLOCK_COUNT;
  *(++header_buff_ptr) = MAX_FD_COUNT;
  fwrite (header_buff_ptr, BLOCK_SIZE, 1, g_img_file);

  // rounding (BLOCK_COUNT / BLOCK_SIZE) to the bigger int
  //int block_count = (BLOCK_COUNT + BLOCK_SIZE - 1) / BLOCK_SIZE;

  // bitmap
  //TODO add occupied blocks
  clear_buffer ();
  int bitmap_bytes_count = (block_count + CHAR_BIT - 1) / CHAR_BIT;
  int bitmap_buffers_count = (bitmap_bytes_count + buffer_size - 1) / buffer_size;
  for (int i = 0; i < bitmap_buffers_count ; ++i)
    {
      fwrite (buffer, BLOCK_SIZE, 1, g_img_file);
    }

  // descriptors

  for (int i = 0; i < (BLOCK_COUNT + BLOCK_SIZE - 1) / BLOCK_SIZE ; ++i)
    {
      fwrite (buffer, BLOCK_SIZE, 1, g_img_file);
    }

  // root directory

  fd_t root_descriptor;

  // free blocks

  for (int i = 0; i < BLOCK_COUNT - 1; ++i)
    {
      fwrite (buffer, BLOCK_SIZE, 1, g_img_file);
    }
  return 1;
}

fd_t
get_fd (int fd_id)
{
  //TODO
  fd_t fd;
  fd.size = 4096;
  fd.type = DIRECTORY_DESCRIPTOR;
  fd.hard_link_count = 4;
  return fd;
}

dir_t
get_dir (int fd_id)
{
  //TODO
}

void *
get_data ()
{
  //TODO
}

void
clear_buffer ()
{
  size_t *buffer_ptr = buffer;

  *(buffer_ptr) = 0;
  for (int i = 0; i < buffer_size; ++i)
    {
      *(++buffer_ptr) = 0;
    }
}

// implementation of header functions

int
mount ()
{
  int result_status = 0;

  // auto umount
  if (g_img_file)
    {
      umount ();
    }

  // trying to open file
  g_img_file = fopen (DEFAULT_IMG_NAME, "rb+");
  if (!g_img_file)
    {
      puts ("No FS file found, a new one created.");
      result_status = create_img ();
    }
  g_img_file = fopen (DEFAULT_IMG_NAME, "rb+");
  if (!g_img_file)
    {
      return -1;
    }

  // initializing working directory
  working_directory_fd_id = ROOT_FD_ID;
  working_directory = get_dir (working_directory_fd_id);
  strcpy (working_directory_name, ROOT_DIRECTORY_STR);

  //TODO
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
      // file ptr
      g_img_file = NULL;

      // fs cached data
      block_count = 0;
      block_size = 0;
      fd_count = 0;
      g_block_bitmap = NULL;

      // session data
      working_directory_fd_id = 0;
      memset (&working_directory, 0, sizeof (dir_t));;
      memset (working_directory_name, 0, MAX_ABSOLUTE_FILE_NAME_SIZE + 1);

      clear_buffer ();
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

  fd_t fd = get_fd (fd_id);
  char *file_type = FILE_TYPES_STR[fd.type];
  printf ("File descriptor %lu is a %s.\n"
          "Size: %lu\n"
          "Has %lu hardlinks.\n",
          fd_id, file_type, fd.size, fd.hard_link_count);
  return 0;
}


int
ls ()
{
  CHECK_IS_MOUNTED
  puts (CURRENT_DIRECTORY_STR);
  puts (PARENT_DIRECTORY_STR);
  uint32_t link_count = working_directory.file_count;
  dir_link_t *links = get_data (working_directory_fd_id);
  for (int i = 0; i < link_count; ++i)
    {
      puts ((links++)->name);
    }
}


int
create(char *name)
{
  CHECK_IS_MOUNTED
  //TODO
}


int
open(char *name, uint32_t *digit_descriptor)
{
  CHECK_IS_MOUNTED
  //TODO
}


int
close(uint32_t fd)
{
  CHECK_IS_MOUNTED
  //TODO
}


int
read(uint32_t fd, uint64_t offset, uint64_t size, char **buffer)
{
  CHECK_IS_MOUNTED
  //TODO
}


int
write(uint32_t fd, uint64_t offset, uint64_t size, char *data)
{
  CHECK_IS_MOUNTED
  //TODO
}


int
link(char *file_name, char *link_name)
{
  CHECK_IS_MOUNTED

  //TODO
}


int
unlink(char *link_name)
{
  CHECK_IS_MOUNTED

  //TODO
}


int
truncate(char *name, uint64_t size)
{
  CHECK_IS_MOUNTED
  //TODO
}


int
mkdir (char *dir_name)
{
  CHECK_IS_MOUNTED
  //TODO
}


int
rmdir (char *dir_name)
{
  CHECK_IS_MOUNTED
  //TODO
}


int
cd (char *dir_name)
{
  CHECK_IS_MOUNTED

  if (*dir_name == '/')
    {
      //TODO check if exist
      strcpy (working_directory_name, dir_name);
    }
  else
    {
    //TODO
      if (*dir_name == '.')
        {
          if (*(++dir_name) == '.')
            {

            }
          // else just the same directory
        }
    }
}


int
pwd ()
{
  CHECK_IS_MOUNTED
  puts (working_directory_name);
}

int
symlink (char *path_name, char *link_name)
{
  CHECK_IS_MOUNTED
  //TODO
}

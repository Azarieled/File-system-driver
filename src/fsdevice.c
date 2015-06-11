#include "fsdevice.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint-gcc.h>
#include "bitmap.h"

const char *DEFAULT_IMG_NAME      = "drivion.img";

// shared buffer
const int buffer_size = BUFSIZ;
const char buffer [BUFSIZ];

// fs file ptr
FILE     *g_img_file = NULL;

// fs cached data
fs_header_t  g_fs_header;
char        *g_block_bitmap;


int
mount_fs ()
{
  int result_status = 0;

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

  g_block_bitmap = malloc (BITNSLOTS (g_fs_header.block_count));

  return result_status;
}

bool is_mounted()
{
  return g_img_file;
}

int
umount_fs()
{
  if (g_img_file == NULL)
    {
      //EXCEPTION
      return 1;
    }

  if (fclose (g_img_file) != NULL) // if returns 0, than all is ok
    {
      //EXCEPTION
      return -1;
    }

  // file ptr
  g_img_file = NULL;

  // fs cached data

  free (g_block_bitmap);
  g_block_bitmap = NULL;
}

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
      puts ("Problem occured during creating FS image.");
      return -1;
    }

  g_fs_header.fs_id = FS_ID;
  g_fs_header.block_count = BLOCK_COUNT;
  g_fs_header.block_size = BLOCK_SIZE;
  g_fs_header.fd_count = DEVICE_FD_COUNT;

  // global header
  clear_buffer ();
  uint64_t *header_buff_ptr = buffer;
  *(header_buff_ptr) = FS_ID;
  *(++header_buff_ptr) = BLOCK_SIZE;
  *(++header_buff_ptr) = BLOCK_COUNT;
  *(++header_buff_ptr) = DEVICE_FD_COUNT;
  fwrite (header_buff_ptr, BLOCK_SIZE, 1, g_img_file);

  // rounding (BLOCK_COUNT / BLOCK_SIZE) to the bigger int
  //int block_count = (BLOCK_COUNT + BLOCK_SIZE - 1) / BLOCK_SIZE;

  // bitmap
  //TODO add occupied blocks
  clear_buffer ();
  int bitmap_bytes_count = BITNSLOTS(g_fs_header.block_count);
  int bitmap_buffers_count = (bitmap_bytes_count + buffer_size - 1) / buffer_size;
  for (int i = 0; i < bitmap_buffers_count; ++i)
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

fs_header_t
get_fs_header ()
{
  return g_fs_header;
}

char *
get_bit_map ()
{
  return g_block_bitmap;
}

fd_t *
get_fd (int fd_id)
{
  fd_t *fd = malloc(sizeof (fd_t) * 2);
  //TODO
  return fd;
}

void *
get_data (int fd_id)
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

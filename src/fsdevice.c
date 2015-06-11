#include "fsdevice.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitmap.h"

//TODO #include <time.h>

const char *DEFAULT_IMG_NAME      = "drivion.img";

// shared buffer
const int buffer_size = BLOCK_SIZE;
const char buffer [BLOCK_SIZE];

// fs file ptr
FILE     *g_img_file = NULL;

// fs cached data
fs_header_t  g_fs_header;
char        *g_block_bitmap;


// === header func impl ===


int
mount_fs ()
{
  int result_status = 0;

  if (is_mounted())
    {
      puts ("Not unmounted.");
      return 1;
    }

  // trying to open file
  g_img_file = fopen (DEFAULT_IMG_NAME, "rb+");
  if (g_img_file == NULL)
    {
      puts ("No FS file found, a new one created.");
      create_img ();
    }

  // trying to open again
  g_img_file = fopen (DEFAULT_IMG_NAME, "rb+");
  if (g_img_file == NULL)
    {
      puts ("Some problems occured during file system mount.");
      return -1;
    }

  // reading file header
  fread(&g_fs_header, sizeof g_fs_header, 1, g_img_file);
  if (g_fs_header.fs_id != FS_ID)
    {
      puts ("The passed file is not the file system image.");
      return -2;
    }

  // reading bitmap
  size_t bitmap_size = BITNSLOTS (g_fs_header.block_count);
  g_block_bitmap = malloc (bitmap_size);
  fread(&g_block_bitmap, sizeof bitmap_size, 1, g_img_file);

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

  if (fclose (g_img_file) != 0) // if returns 0, than all is ok
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

  // bitmap
  //TODO add occupied blocks
  g_fs_header.bitmap_block_num = HEADER_BLOCK_NUM + HEADER_BLOCK_COUNT;
  int bitmap_blocks_count = (BITNSLOTS(g_fs_header.block_count) + g_fs_header.block_size - 1) / g_fs_header.block_size;

  // descriptors
  g_fs_header.fd_block_num = bitmap_blocks_count + g_fs_header.bitmap_block_num;
  int fd_blocks_count = (g_fs_header.fd_count + g_fs_header.block_size - 1) / g_fs_header.block_size;

  // data blocks
  g_fs_header.data_block_num = g_fs_header.fd_block_num + fd_blocks_count;
  int data_blocks_count = g_fs_header.block_count - g_fs_header.data_block_num;

  // root descriptor
  fd_t root_descriptor = {
    //.creation_date = ,
    //.modification_date = ,
    .size = 0,
    .type = DIRECTORY_DESCRIPTOR,
    .hard_link_count = 2,
    .owner_mode = 7,
    .group_mode = 5,
    .other_mode = 5,
    .additional_block_num = g_fs_header.data_block_num
  };

  // writing files
  memset (&buffer, 0, buffer_size);

  // header
  *((fs_header_t *) buffer) = g_fs_header;
  fwrite (buffer, BLOCK_SIZE, 1, g_img_file);
  memset (&buffer, 0, sizeof g_fs_header);

  // bitmap
  for (int i = 0; i < bitmap_blocks_count; ++i)
    {
      fwrite (buffer, BLOCK_SIZE, 1, g_img_file);
    }

  // fd_blocks
  for (int i = 0; i < fd_blocks_count - 1 ; ++i)
    {
      fwrite (buffer, BLOCK_SIZE, 1, g_img_file);
    }

  // data_blocks
  for (int i = 0; i < data_blocks_count - 1; ++i)
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

int
get_fd (int fd_id, fd_t *fd)
{
  fd_t found_fd;
  //TODO
  *fd = found_fd;
  return 0;
}

uint32_t
create_fd (fd_t *fd)
{

}

int
update_fd (int fd_id, fd_t *dir)
{

}

int
append_data_to_fd (uint32_t fd_id, void *data, uint64_t size)
{
  //TODO
}

void *
get_data (fd_t fd/*, uint32_t block_num*/)
{
  //TODO
}

uint32_t
find_free_block ()
{

}

int
update_block (uint32_t block_id, void *data)
{
  
}

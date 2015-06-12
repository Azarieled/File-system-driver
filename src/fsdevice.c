#include "fsdevice.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitmap.h"

//TODO #include <time.h>

const char *DEFAULT_IMG_NAME      = "drivion.img";

// shared buffer
const unsigned int buffer_size = BLOCK_SIZE;
static char buffer [BLOCK_SIZE];

// fs file ptr
static FILE     *g_img_file = NULL;

// fs cached data
static fs_header_t  g_fs_header;
static char        *g_block_bitmap;

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
  fs_header_t fs_header =
  {
    .fs_id = FS_ID,
    .block_count = BLOCK_COUNT,
    .block_size = BLOCK_SIZE,
    .fd_count = DEVICE_FD_COUNT
  };

  // bitmap
  char block_bitmap [BITNSLOTS(fs_header.block_count)];
  memset (block_bitmap, 0, sizeof block_bitmap);
  fs_header.bitmap_block_num = HEADER_BLOCK_NUM + HEADER_BLOCK_COUNT;
  uint64_t bitmap_blocks_count = (BITNSLOTS(fs_header.block_count) + fs_header.block_size - 1) / fs_header.block_size;

  // descriptors
  fs_header.fd_block_num = bitmap_blocks_count + fs_header.bitmap_block_num;
  uint64_t fd_bytes_count = fs_header.fd_count * sizeof (fd_t);
  uint64_t fd_blocks_count = (fd_bytes_count + fs_header.block_size - 1) / fs_header.block_size;

  // data blocks
  fs_header.data_block_num = fs_header.fd_block_num + fd_blocks_count;
  uint64_t data_blocks_count = fs_header.block_count - fs_header.data_block_num;

  // bitmap finish
  for (uint32_t i = 0; i <= BITNSLOTS(fs_header.data_block_num); ++i)
    {
      BITSET (block_bitmap, i);
    }


  // root descriptor
  fd_t root_fd = {
    //.creation_date = ,
    //.modification_date = ,
    .size = 0,
    .type = DIRECTORY_DESCRIPTOR,
    .hard_link_count = 2,
    .owner_mode = 7,
    .group_mode = 5,
    .other_mode = 5,
    .additional_block_num = fs_header.data_block_num
  };

  // writing to img file
  g_img_file = fopen (DEFAULT_IMG_NAME, "wb+");
  if (!g_img_file)
    {
      // EXCEPTION
      puts ("Problem occured during creating FS image.");
      return -1;
    }
  memset (buffer, 0, buffer_size);

  // header
  *((fs_header_t *) buffer) = fs_header;
  fwrite (buffer, BLOCK_SIZE, 1, g_img_file);
  memset (buffer, 0, sizeof fs_header);

  // bitmap
  for (uint64_t i = 0; i < bitmap_blocks_count; ++i)
    {
      fwrite (block_bitmap + (BLOCK_COUNT / CHAR_BIT * i), BLOCK_SIZE, 1, g_img_file);
    }
  //TODO safer writing last iteration

  // fd_blocks
  *((fd_t *) buffer) = root_fd;
  fwrite (buffer, BLOCK_SIZE, 1, g_img_file);
  memset (buffer, 0, sizeof root_fd);
  for (uint64_t i = 0; i < fd_blocks_count - 1; ++i)
    {
      fwrite (buffer, BLOCK_SIZE, 1, g_img_file);
    }

  // data_blocks
  for (uint64_t i = 0; i < data_blocks_count; ++i)
    {
      fwrite (buffer, BLOCK_SIZE, 1, g_img_file);
    }
  return 0;
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
get_fd (uint32_t fd_id, fd_t *fd)
{
  //TODO check overflow
  long offset = g_fs_header.fd_block_num * g_fs_header.block_size;
  fseek (g_img_file, offset + (fd_id * sizeof (fd_t)), SEEK_SET);
  fread (fd, sizeof *fd, 1, g_img_file);
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

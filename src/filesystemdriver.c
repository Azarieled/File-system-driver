#include <stdio.h>
#include <string.h>
#include "filesystemdriver.h"
#include "fsdevice.h"

#ifndef StrRel
#define StrRel(s1, op, s2) (strcmp(s1, s2) op 0)
#endif //StrRel

#define CHECK_IS_MOUNTED \
if (!is_mounted ())         \
  {                      \
    return -1;           \
  }                      \


// string literals
const char *ROOT_DIRECTORY_STR    = "/";
const char *CURRENT_DIRECTORY_STR = ".";
const char *PARENT_DIRECTORY_STR  = "..";
const char *FILE_TYPES_STR []     = {"file", "directory", "symlink"};

// error literals
const char *NON_FILE_IO_NOT_SUPPORTED = "This operation is not supported for %.\n";

// session data
int       g_working_directory_fd_id;
dir_fd_t     g_working_directory;
char      g_working_directory_name [MAX_ABSOLUTE_FILE_NAME_SIZE + 1];

// opened fd
numeric_fd_t *g_numeric_fd_list = NULL;

uint32_t
generate_numeric_fd_id ()
{
  //TODO
}

numeric_fd_t
add_numeric_fd (uint32_t fd_id)
{
  numeric_fd_t fd;
  fd.number = generate_numeric_fd_id ();
  fd.fd_id = fd_id;
  fd.offset = 0;
  return fd;
}

numeric_fd_t
get_numeric_fd ()
{
 //TODO
}


int
get_fd_id_by_name (char *name, uint32_t *result)
{
  // C string validation
  if (*name == '\0')
    {
      return -1;
    }
  // absolute file name
  if (*name == '/')
    {
      //compound check
      if (*(++name) != '\0')
        {
          //TODO NOT SUPPORTED now
          puts("The compound file names not supported yet.");
          return -1;
        }
      //TODO go deep
      result = ROOT_FD_ID;
      return 0;
    }
  else
    {
      //compound check
      if (strchr(name, '/') != NULL)
        {
          //TODO NOT SUPPORTED now
          puts("The compound file names not supported yet.");
          return -1;
        }

      get_hard_link_fd_id (name, g_working_directory, );
    }

  //TODO NOT SUPPORTED now
  puts("The compound file names not supported yet.");

}

int
get_hard_link_fd_id (char *name, dir_fd_t dir, uint32_t *fd_id)
{
  // C string validation
  if (*name == '\0')
    {
      return -1;
    }
  else if (*name == '.' && *(name + 1) == '\0')
    {
      *fd_id = 0;//TODO
      return 0;
    }
  else if (*name == '.' && *(name + 1) == '.' && *(name + 2) == '\0')
    {
      *fd_id = dir.parent_fd_id;
      return 0;
    }
  else
    {
      hard_link_t *links = get_data (g_working_directory_fd_id);
      for (int i = 0; i < g_working_directory.file_count; ++i)
        {
          if (StrRel(name, ==, links->name))
            {
              //TODO
            }
          ++links;
          puts ((links++)->name);
        }

      // if we are here, then no such file
      puts("File doesn't exist.");
      return -1;
    }
}

// implementation of header functions

int
mount ()
{
  // auto umount
  if (is_mounted ())
    {
      umount ();
    }

  int mount_status = mount_fs ();

  if (mount_status == -1)
    {
      //EXCEPTION
      return -1;
    }

  // initializing working directory
  g_working_directory_fd_id = ROOT_FD_ID;
  g_working_directory = get_dir (g_working_directory_fd_id);
  strcpy (g_working_directory_name, ROOT_DIRECTORY_STR);

  return mount_status;
}


int
umount ()
{
  CHECK_IS_MOUNTED

  if (umount_fs ()) // if returns 0, than all is ok
    {
      //EXCEPTION
      puts ("Problem occured during FS unmount.");
      return -1;
    }
  else
    {      
      // session data
      g_working_directory_fd_id = 0;
      memset (&g_working_directory, 0, sizeof g_working_directory);
      memset (g_working_directory_name, 0, MAX_ABSOLUTE_FILE_NAME_SIZE + 1);
      puts ("FS successfully unmounted.");
      return 0;
    }

}


int
filestat(uint32_t fd_id)
{
  CHECK_IS_MOUNTED

  if (fd_id >= get_fs_header().fd_count)
    {
      //EXCEPTION
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
  uint32_t link_count = g_working_directory.file_count;
  hard_link_t *links = get_data (g_working_directory_fd_id);
  for (int i = 0; i < link_count; ++i)
    {
      puts ((links++)->name);
    }
  return 0;
}


int
create(char *name)
{
  CHECK_IS_MOUNTED
  //TODO
}


int
open(char *name, uint32_t *numeric_descriptor)
{
  CHECK_IS_MOUNTED

  //TODO finish this
  uint32_t fd;
  int find_file_status = get_fd_id_by_name (name, &fd);
  if (find_file_status != -1)
    {
      fd_t file_fd = get_fd (fd);
      if (file_fd.type == FILE_DESCRIPTOR)
        {
          numeric_fd_t numeric_fd = add_numeric_fd (file_fd);
          printf ("Numeric file descriptor: %ul", numeric_fd.number);
        }
      else
        {
          //EXCEPTION
          printf (NON_FILE_IO_NOT_SUPPORTED, FILE_TYPES_STR [file_fd.type]);
          return -1;
        }
    }
}


int
close(uint32_t fd)
{
  CHECK_IS_MOUNTED
  //TODO
}


int
read(uint32_t fd, uint64_t offset, uint64_t size, char *buffer)
{
  CHECK_IS_MOUNTED

  // asserting that opened file is simple file
  numeric_fd_t numeric_fd = get_numeric_fd (fd);
  fd_t file_fd = get_fd(numeric_fd.fd_id);
  if (file_fd.type == FILE_DESCRIPTOR)
    {
      get_data (numeric_fd.fd_id);
    }
  else
    {
      //EXCEPTION
      printf (NON_FILE_IO_NOT_SUPPORTED, FILE_TYPES_STR [file_fd.type]);
      return -1;
    }
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


//TODO implement this for more than 1 layer deep
int
cd (char *dir_name)
{
  CHECK_IS_MOUNTED

  uint32_t fd_id;
  int get_status = get_fd_id_by_name (dir_name, fd_id);
  fd_t fd = get_fd (fd_id);
  if (fd.type == DIRECTORY_DESCRIPTOR)
    {
      g_working_directory_fd_id = fd_id;
      g_working_directory = ();
      //TODO finish this
      strcat (g_working_directory_name, name);
    }
}


int
pwd ()
{
  CHECK_IS_MOUNTED
  puts (g_working_directory_name);
}

int
symlink (char *path_name, char *link_name)
{
  CHECK_IS_MOUNTED
  //TODO
}

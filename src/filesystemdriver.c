#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

// session data
uint32_t  g_working_directory_fd_id;
dir_fd_t  g_working_directory;
char      g_working_directory_name [MAX_ABSOLUTE_FILE_NAME_SIZE + 1];

//TODO hashmap
// opened fd
numeric_fd_t *g_numeric_fd_list = NULL;
uint32_t last_generated = 0;

uint32_t
generate_numeric_fd_id ();

uint32_t
add_numeric_fd (uint32_t fd_id);

numeric_fd_t *
get_numeric_fd (uint32_t number);

int
remove_numeric_fd (uint32_t number);

int
get_fd_id_by_name (char *name, uint32_t *result);


/**
 * @brief get_hard_link_fd_id
 * @param name file name
 * @param dir directory for finding hard link
 * @param fd_id is initialized with hard link fd id if 0 returned
 * @return -1 if fails, 1 if the result is current directory, 0 if successfuly found.
 * If 0 is returned, than fd_id is initialized with hard link fd id
 */
int
get_hard_link_fd_id (char *name, dir_fd_t dir, uint32_t *fd_id);

int
add_hard_link_fd_id (char *name, uint32_t hard_link_fd_id, dir_fd_t dir, uint32_t dir_fd_id);

uint32_t
calc_block_count ();

uint32_t
generate_numeric_fd_id ()
{
  return ++last_generated;
}

uint32_t
add_numeric_fd (uint32_t fd_id)
{
  g_numeric_fd_list = malloc(sizeof (numeric_fd_t));
  g_numeric_fd_list->number = generate_numeric_fd_id ();
  //TODO handle generation fail
  g_numeric_fd_list->fd_id = fd_id;
  g_numeric_fd_list->next = NULL;
  return g_numeric_fd_list->number;
}

numeric_fd_t *
get_numeric_fd (uint32_t number)
{
  numeric_fd_t *cur = g_numeric_fd_list;
  while (cur != NULL)
    {
      if (cur->number == number)
        {
          return cur;
        }
      cur = cur->next;
    }

  return NULL;
}

int
remove_numeric_fd (uint32_t number)
{
  numeric_fd_t *prev = NULL;
  numeric_fd_t *cur = g_numeric_fd_list;
  while (cur != NULL)
    {
      if (cur->number == number)
        {
          if (prev == NULL)
            g_numeric_fd_list = cur->next;
          else
            prev->next = cur->next;
          free (cur);
          return 0;
        }
      prev = cur;
      cur = cur->next;
    }

  return -1;
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
  else if (*name == '/')
    {
      //compound check
      if (*(++name) != '\0')
        {
          //TODO NOT SUPPORTED now
          puts("The compound file names not supported yet.");
          return -1;
        }
      //TODO compound implementation
      *result = ROOT_FD_ID;
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

      uint32_t fd_id;
      int hard_link_status = get_hard_link_fd_id (name, g_working_directory, &fd_id);
      if (hard_link_status != -1)
        {
          *result = fd_id;
        }
      return hard_link_status;
    }
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
      return 1;
    }
  else if (*name == '.' && *(name + 1) == '.' && *(name + 2) == '\0')
    {
      *fd_id = dir.parent_fd_id;
      return 0;
    }
  else
    {
      hard_link_t *links = get_data (dir.fd);
      for (uint32_t i = 0; i < dir.file_count; ++i)
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

int
add_hard_link_fd_id (char *name, uint32_t hard_link_fd_id, dir_fd_t dir, uint32_t dir_fd_id)
{
  unsigned int name_len = strlen (name);
  if (name_len > MAX_FILE_NAME_SIZE)
  {
    printf("File name too long:.%u bytes. Maximum available file name is \n", name_len);
    return -1;
  }
  hard_link_t hard_link;
  strcpy (hard_link.name, name);
  hard_link.fd_id = hard_link_fd_id;
  append_data_to_fd (dir_fd_id, hard_link, sizeof (hard_link_t));
  dir.fd.size += sizeof (hard_link_t);
  dir.file_count += 1;
  update_dir_fd (dir_fd_id, &dir);
}

uint32_t
calc_block_count (fd_t fd)
{
  uint32_t block_size = get_fs_header().block_size;
  return (fd.size + block_size - 1) / block_size;
}

// === implementation of header functions ===

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
      puts ("Some problems occured during file system mount.");
      return -1;
    }

  // initializing working directory
  g_working_directory_fd_id = ROOT_FD_ID;

  dir_fd_t *root_dir_ptr = (dir_fd_t *) get_fd (g_working_directory_fd_id);

  if (root_dir_ptr == NULL)
    {
      //EXCEPTION
      puts ("Couldn't get file system root.");
      return -1;
    }

  // for being sure that file system is not corrupted
  if (root_dir_ptr->fd.type != DIRECTORY_DESCRIPTOR)
    {
      //EXCEPTION
      puts ("File system root is corrupted.");
      return -1;
    }

  g_working_directory = *root_dir_ptr;
  free (root_dir_ptr);

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
      puts ("Some problems occured during file system unmount.");
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

  fd_t *fd = get_fd (fd_id);
  printf ("File descriptor %u is a %s.\n"
          "Size: %lu\n"
          "Has %u hardlinks.\n",
          fd_id, FILE_TYPES_STR[fd->type], fd->size, fd->hard_link_count);
  return 0;
}


int
ls ()
{
  CHECK_IS_MOUNTED
  printf ("%s %u", CURRENT_DIRECTORY_STR, g_working_directory_fd_id);
  printf ("%s %u", PARENT_DIRECTORY_STR, g_working_directory.parent_fd_id);
  uint32_t link_count = g_working_directory.file_count;
  hard_link_t *links = get_data (g_working_directory.fd);
  for (uint32_t i = 0; i < link_count; ++i)
    {
      printf ("%s - %u", links->name, links->fd_id);
      ++links;
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
  uint32_t fd_id;
  int find_file_status = get_fd_id_by_name (name, &fd_id);

  if (find_file_status != -1)
    {
      file_fd_t *file_fd_ptr = (file_fd_t *) get_fd (fd_id);
      if (file_fd_ptr->fd.type == FILE_DESCRIPTOR)
        {
          *numeric_descriptor = add_numeric_fd (fd_id);
          printf ("Numeric file descriptor: %u", *numeric_descriptor);
        }
      else
        {
          //EXCEPTION
          printf ("This operation is not supported for %s.\n", FILE_TYPES_STR [file_fd_ptr->fd.type]);
          return -1;
        }
    }

  return find_file_status;
}


int
close(uint32_t fd)
{
  CHECK_IS_MOUNTED
  if (remove_numeric_fd (fd) == 0)
    printf ("File descriptor with number %u closed.\n", fd);
  else
    printf ("File descriptor with number %u not found.\n", fd);
}


int
read(uint32_t fd, uint64_t offset, uint64_t size, char *buffer)
{
  CHECK_IS_MOUNTED

  // assuming that open checked that file is simple file
  numeric_fd_t *numeric_fd = get_numeric_fd (fd);
  uint32_t fd_id = numeric_fd->fd_id;

  file_fd_t *fd_ptr = (file_fd_t *) get_fd (fd_id);
  file_fd_t file_fd = *fd_ptr;
  free (fd_ptr);

  if (offset > file_fd.fd.size)
    {
      printf ("Impossible to read not from file. Offset (%llu) is bigger than file size (%llu).\n", offset, file_fd.fd.size);
      return -1;
    }
  else if (offset + size < offset || offset + size > file_fd.fd.size)
    {
      printf ("Impossible to read not from file. Trying to read with offset (%llu) size (%llu). Their summ is bigger than file size (%llu).\n", offset, size, file_fd.fd.size);
      return -1;
    }
  else
    {
      //TODO char *data = get_data();
    }
}


int
write(uint32_t fd, uint64_t offset, uint64_t size, char *data)
{
  CHECK_IS_MOUNTED

  // assuming that open checked that file is simple file
  numeric_fd_t *numeric_fd = get_numeric_fd (fd);
  uint32_t fd_id = numeric_fd->fd_id;

  file_fd_t *fd_ptr = (file_fd_t *) get_fd (fd_id);
  file_fd_t file_fd = *fd_ptr;
  free (fd_ptr);

  if (offset > file_fd.fd.size)
    {
      printf ("Impossible to read not from file. Offset (%llu) is bigger than file size (%llu).\n", offset, file_fd.fd.size);
      return -1;
    }
  else if (offset + size < offset || offset + size > file_fd.fd.size)
    {
      printf ("Impossible to read not from file. Trying to read with offset (%llu) size (%llu). Their summ is bigger than file size (%llu).\n", offset, size, file_fd.fd.size);
      return -1;
    }
  else
    {
      //TODO char *data = get_data();
    }
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
  int get_status = get_fd_id_by_name (dir_name, &fd_id);
  if (get_status == -1)
    {
      return get_status;
    }
  if (get_status == 1)
    {
      return 0;
    }

  fd_t *fd_ptr = get_fd (fd_id);
  if (fd_ptr->type != DIRECTORY_DESCRIPTOR)
    {
      free (fd_ptr);
      printf ("%s is not a directory.", dir_name);
      return -1;
    }

  g_working_directory_fd_id = fd_id;
  g_working_directory = *((dir_fd_t *) fd_ptr);
  free (fd_ptr);

  //TODO change this workaround if compound names in cd will be implemented
  if (*dir_name == '/')
    {
      *g_working_directory_name = '/';
      *(g_working_directory_name + 1) = '\0';
    }
  else if (*dir_name == '.' && *(dir_name + 1) == '\0')
    {
      // do nothing
    }
  else if (*dir_name == '.' && *(dir_name + 1) == '.' && *(dir_name + 2) == '\0')
    {
      char *last_slash = strrchr (g_working_directory_name, '/');
      *last_slash = '\0';
    }
  else
    strcat (g_working_directory_name, dir_name);

  return 0;

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
  symlink_fd_t symlink_fd;
  symlink_fd.fd.type = SYMLINK_DESCRIPTOR;
  symlink_fd.fd.size = 4096;
  symlink_fd.fd.hard_link_count = 1;
  uint32_t fd_id = new_symlink_fd (&symlink_fd);
  if (fd_id != BAD_FD_ID)
    {
      symlink_fd.fd.additional_block_num = find_free_block ();
      write_block (symlink_fd.fd.additional_block_num, path_name);
      add_hard_link_fd_id (link_name, fd_id, g_working_directory_fd_id, g_working_directory);
    }
}

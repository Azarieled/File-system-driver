#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "filesystemdriver.h"

#define INPUT_BUFFER_SIZE 5000
#define TOKEN_DELIMS " \r\n\t"

/*
 * Returns 0 if the command is an exit,
 * 1 if the command is interperted or -1 if not.
 */
int
interpret (char *statement);


int main (void)
{
  char buffer [INPUT_BUFFER_SIZE];
  do
    {
      printf ("\ndrivion> ");
    }
  while (fgets (buffer, INPUT_BUFFER_SIZE, stdin) && interpret(buffer));
  return 0;
}


/*
 * Returns -1 if token is absent, 0 in other case.
 */
int
getNextToken(char **to_string, char *fail_message)
{
  *to_string = strtok (NULL, TOKEN_DELIMS);
  if (*to_string == NULL)
    {
      puts (fail_message);
      return -1;
    }
  return 0;
}

/*
 * Returns -1 if failed, 0 if converted.
 */
int
to_uint32_t(char *from, uint32_t *to, char *format_error_message, char *length_error_message)
{
  char *error_symbol;
  unsigned long long value = strtoull (from, &error_symbol, 0);
  if (*error_symbol != '\0')
    {
      puts (format_error_message);
      return -1;
    }
  if (value >= UINT32_MAX)
    {
      puts (length_error_message);
      return -1;
    }
  *to = value;
  return 0;
}

/*
 * Returns -1 if failed, 0 if converted.
 */
int
to_uint64_t(char *from, uint64_t *to, char *format_error_message, char *length_error_message)
{
  char *error_symbol;
  unsigned long long value = strtoull (from, &error_symbol, 0);
  if (*error_symbol != '\0')
    {
      puts (format_error_message);
      return -1;
    }
  if (value >= UINT64_MAX)
    {
      puts (length_error_message);
      return -1;
    }
  *to = value;
  return 0;
}

int
interpret (char *statement)
{
  char *command = strtok (statement, TOKEN_DELIMS);
  int status = 1;

  // no command
  if (command == NULL)
    {
      puts ("Please, enter any of supported commands. It's imposible to execute an empty command.");
      return -1;
    }

  // mount
  else if (strcmp (command, "mount") == 0)
      mount ();

  // umount
  else if (strcmp (command, "umount") == 0)
      umount ();

  // filestat
  else if (strcmp (command, "filestat") == 0)
    {
      char *param;
      uint32_t inode;

      status |= getNextToken (&param, "filestat: usage: filestat inode");
      status |= to_uint32_t (param, &inode, "Invalid inode format. A number expected.", "No file with such inode.");
      if (status != -1)
        filestat (inode);
    }

  // ls
  else if (strcmp (command, "ls") == 0)
      ls ();

  // create
  else if (strcmp (command, "create") == 0)
    {
      char *file_name;
      status |= getNextToken (&file_name, "create: usage: create file_name");
      if (status != -1)
        create (file_name);
    }

  // open
  else if (strcmp (command, "open") == 0)
    {
      char *name;
      status |= getNextToken (&name, "open: usage: open name");
      if (status != -1)
        open (name);
    }

  // close
  else if (strcmp (command, "close") == 0)
    {
      char *param;
      uint32_t fd;

      status |= getNextToken (&param, "close: usage: close fd");
      status |= to_uint32_t (param, &fd,"Invalid fd format. A number expected.", "No descriptor with such fd.");
      if (status != -1)
        close (fd);
    }

  // read
  else if (strcmp (command, "read") == 0)
    {
      // buffer
      char *param;
      //params
      uint32_t fd;
      uint64_t offset;
      uint64_t size;

      status |= getNextToken (&param, "read: usage: read fd offset size");
      status |= to_uint32_t (param, &fd,"Invalid fd format. A number expected.", "No descriptor with such fd.");
      status |= getNextToken (&param, "read: usage: read fd offset size");
      status |= to_uint64_t (param, &offset,"Invalid offset format. A number expected.", "Invalid offset. Driver supports file size < 2^60.");
      status |= getNextToken (&param, "read: usage: read fd offset size");
      status |= to_uint64_t (param, &size,"Invalid size format. A number expected.", "Invalid size. Driver supports file size < 2^60.");
      if (status != -1)
        read (fd, offset, size);
    }

  // write
  else if (strcmp (command, "write") == 0)
    {
      // buffer
      char *param;
      //params
      uint32_t fd;
      uint64_t offset;
      uint64_t size;
      char *data;

      status |= getNextToken (&param, "write: usage: write fd offset size data");
      status |= to_uint32_t (param, &fd,"Invalid fd format. A number expected.", "No descriptor with such fd.");
      status |= getNextToken (&param, "write: usage: write fd offset size data");
      status |= to_uint64_t (param, &offset,"Invalid offset format. A number expected.", "Invalid offset. Driver supports file size < 2^60.");
      status |= getNextToken (&param, "write: usage: write fd offset size data");
      status |= to_uint64_t (param, &size,"Invalid size format. A number expected.", "Invalid size. Driver supports file size < 2^60.");
      status |= getNextToken (&data, "write: usage: write fd offset size data");
      if (status != -1)
        write (fd, offset, size, data);
    }

  //link
  else if (strcmp (command, "link") == 0)
    {
      char *file_name;
      char *link_name;

      status |= getNextToken (&file_name, "link: usage: link file_name link_name");
      status |= getNextToken (&link_name, "link: usage: link file_name link_name");
      if (status != -1)
        link(file_name, link_name);
    }

  //unlink
  else if (strcmp (command, "unlink") == 0)
    {
      char *link_name;
      status |= getNextToken (&link_name, "unlink: usage: link link_name");
      if (status != -1)
        unlink(link_name);
    }

  // truncate
  else if (strcmp (command, "truncate") == 0)
    {
      // buffer
      char *param;
      // params
      char *file_name;
      uint64_t size;

      status |= getNextToken (&file_name, "truncate: usage: truncate name size");
      status |= getNextToken (&param, "truncate: usage: truncate name size");
      status |= to_uint64_t (param, &size,"Invalid size format. A number expected.", "Invalid size. Driver supports file size < 2^60.");

      if (status != -1)
        truncate (file_name, size);
    }

  // exit
  else if (strcmp(command, "exit") == 0)
    {
      puts ("Come again, later.");
      status = 0;
    }

  // NOT SUPPORTED COMMAND
  else
    {
      puts ("Sorry, but this command is not supported yet.\n"
            "But you may use any of supported driver commands.");
      status = -1;
    }

  return status;
}


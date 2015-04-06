#include <stdio.h>
#include <stdlib.h>
#include "filesystemdriver.h"

#define INPUT_BUFFER_SIZE 5000
#define TOKEN_DELIMS " \r\n\t"

/*
 * Returns 0 if the command is an exit,
 * 1 if the command is interperted or -1 if not.
 */
int
interpret (char *statement)
{
  char *command = strtok(statement, TOKEN_DELIMS);

  // mount
  if (strcmp(command, "mount") == 0)
      mount();

  // umount
  else if (strcmp(command, "umount") == 0)
      umount();

  // filestat
  else if (strcmp(command, "filestat") == 0)
    {
      char *param = strtok(NULL, TOKEN_DELIMS);
      if (param == NULL)
        {
          puts ("filestat: usage: filestat inode");
          return -1;
        }

      char *error_symbol;
      unsigned long id = strtoul(param, &error_symbol, 0);
      if (*error_symbol != '\0')
        {
          puts ("Invalid inode format. A number expected.");
          return -1;
        }
      if (id >= UINT_MAX)
        {
          puts ("Driver supports inode < 2^32.");
          return -1;
        }

      // if we got here, validation is ok
      filestat((unsigned int) id);
    }

  // ls
  else if (strcmp(command, "ls") == 0)
      ls();

  // create
  else if (strcmp(command, "create") == 0)
    {
      char *name = strtok(NULL, TOKEN_DELIMS);
      if (name == NULL)
        {
          puts ("create requires file name as parameter.");
          return -1;
        }

      // if we got here, validation is ok
      create(name);
    }

  // open
  else if (strcmp(command, "open") == 0)
    {
      char *name = strtok(NULL, TOKEN_DELIMS);
      if (name == NULL)
        {
          puts ("create requires file name as parameter.");
          return -1;
        }

      // if we got here, validation is ok
      open(name);
    }

  // close
  else if (strcmp(command, "close") == 0)
    {
      char *param = strtok(NULL, TOKEN_DELIMS);
      if (param == NULL)
        {
          puts ("close: usage: close fd");
          return -1;
        }

      char *error_symbol;
      unsigned long fd = strtoul(param, &error_symbol, 0);
      if (*error_symbol != '\0')
        {
          puts ("Invalid fd format. A number expected.");
          return -1;
        }
      if (fd >= UINT_MAX)
        {
          puts ("Driver supports fd < 2^32.");
          return -1;
        }

      // if we got here, validation is ok
      close((unsigned int) fd);
    }

  // read
  else if (strcmp(command, "read") == 0)
    {
      //params
      unsigned long fd;
      unsigned long long offset;
      unsigned long long size;

      // validation buffers
      char *error_symbol;
      char *param = strtok(NULL, TOKEN_DELIMS);

      if (param == NULL)
        {
          puts ("close: usage: close fd");
          return -1;
        }

      fd = strtoul(param, &error_symbol, 0);
      if (*error_symbol != '\0')
        {
          puts ("Invalid fd format. A number expected.");
          return -1;
        }


      // number outer validation
      if (fd >= UINT_MAX)
        {
          puts ("Driver supports fd < 2^32.");
          return -1;
        }



    }

  // write
  else if (strcmp(command, "write") == 0)
    {

    }

  //link
  else if (strcmp(command, "link") == 0)
    {

    }

  //unlink
  else if (strcmp(command, "unlink") == 0)
    {

    }

  // truncate
  else if (strcmp(command, "truncate") == 0)
    {

    }

  else if (strcmp(command, "exit") == 0)
    {
      return 0;
    }
  // NOT SUPPORTED COMMAND
  else
    {
      puts ("Sorry, but this command is not supported yet.\n"
            "But you may use any of supported driver commands.");
      return -1;
    }

  return 1;
}

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


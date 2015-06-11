#ifndef FSDEVICE_H
#define FSDEVICE_H

#include "filesystemdriver.h"
#include "stdbool.h"

int
mount_fs ();

bool
is_mounted ();

int
umount_fs ();

int
create_img ();

fs_header_t
get_fs_header ();

char *
get_bit_map ();

fd_t *
get_fd (int fd_id);

void *
get_data (fd_t fd/*, uint32_t block_num*/);

#endif // FSDEVICE_H


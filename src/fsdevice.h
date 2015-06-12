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

/**
 * Creates clean img file.
 *
 * @brief create_img
 * @return -1 if fails, 1 if ok
 */
int
create_img ();

fs_header_t
get_fs_header ();

char *
get_bit_map ();

int
get_fd (uint32_t fd_id, fd_t *fd);

uint32_t
create_fd (fd_t *fd);

int 
update_fd (int fd_id, fd_t *dir);

int
append_data_to_fd (uint32_t fd_id, void *data, uint64_t size);

void *
get_data (fd_t fd/*, uint32_t block_num*/);

uint32_t
find_free_block ();

int
update_block (uint32_t block_id, void *data);

#endif // FSDEVICE_H


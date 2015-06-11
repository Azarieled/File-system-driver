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

uint32_t
new_file_fd (file_fd_t *fd);

uint32_t
new_dir_fd (dir_fd_t *fd);

int 
update_dir_fd (int fd_id, dir_fd_t *dir);

uint32_t
new_symlink_fd(symlink_fd_t *fd);

int
append_data_to_fd (uint32_t fd_id, void *data, uint64_t size);

void *
get_data (fd_t fd/*, uint32_t block_num*/);

uint32_t
find_free_block ();

int
write_block (uint32_t block_id, void *data);

#endif // FSDEVICE_H


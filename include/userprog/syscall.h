#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "threads/synch.h"

struct lock filesys_lock;
void syscall_init (void);
void close(int fd);
struct page *check_address(void *addr);
#endif /* userprog/syscall.h */

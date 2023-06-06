#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "threads/synch.h"

struct lock sys_lock;
void syscall_init (void);

#endif /* userprog/syscall.h */

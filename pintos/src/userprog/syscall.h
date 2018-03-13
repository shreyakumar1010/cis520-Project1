#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include <user/syscall.h>
#include "devices/input.h"
#include "devices/shutdown.h"
#include "filesys/file.h"
#include "filesys/filesys.h"

void syscall_init (void);
bool valid (const void *vaddr);
void sys_halt (void);
void sys_exit(int status);
pid_t sys_exec(const char * cmd_line);
int sys_wait (pid_t pid);
bool sys_create(const char * file, unsigned initial_size);
bool sys_remove(const char *file);
int sys_open(const char * file);
struct file * get_file(int fd);
int sys_filesize(int fd);
int sys_read(int fd, void *buffer, unsigned size);
int sys_write(int fd, const void * buffer, unsigned size);
void sys_seek (int fd, unsigned position);
unsigned sys_tell (int fd);
void sys_close(int fd);
void pull_args(struct intr_frame *f, int *arg, int n);
int check_get_page(const void * vaddr);
struct child_process* get_child (int pid);


struct lock syscall_lock;

struct file_desc
{
  struct file *file;
  int fd;
  struct list_elem file_elem;
};

#endif /* userprog/syscall.h */

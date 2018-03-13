#include "userprog/syscall.h"
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

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  lock_init(&syscall_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  //printf ("system call!\n");
  //thread_exit ();
  int arguments[3];
  if (!valid ((const void *) f->esp))
  {
    sys_exit(-1);
  }
  switch (*(int *) f->esp)
  {
      case SYS_HALT:
      {
        sys_halt();
        break;
      }
      case SYS_EXIT:
      {
        pull_args(f, &arguments[0], 1);
        sys_exit(arguments[0]);
        break;
      }
      case SYS_EXEC:
      {
        pull_args(f, &arguments[0], 1);
        arguments[0] = check_get_page((const void *) arguments[0]);
        f->eax = sys_exec((const char *) arguments[0]);
        break;
      }
      case SYS_WAIT:
      {
        pull_args(f, &arguments[0], 1);
        f->eax = sys_wait(arguments[0]);
        break;
      }
      case SYS_CREATE:
      {
        pull_args(f, &arguments[0], 2);
        arguments[0] = check_get_page((const void *) arguments[0]);
        f->eax = sys_create((const char *)arguments[0], (unsigned) arguments[1]);
        break;
      
      }
      case SYS_REMOVE:
      {
        pull_args(f, &arguments[0], 1);
        arguments[0] = check_get_page((const void *) arguments[0]);
        f->eax = sys_remove((const char *)arguments[0]);
        break;
      }
      case SYS_OPEN:
      {
        pull_args(f, &arguments[0], 1);
        arguments[0] = check_get_page((const void *) arguments[0]);
        f->eax = sys_open((const char *)arguments[0]);
        break;
      }
      case SYS_FILESIZE:
      {
        pull_args(f, &arguments[0], 1);
        f->eax = sys_filesize((const char *)arguments[0]);
        break;
      }
      case SYS_READ: 
      {
        pull_args(f, &arguments[0], 3);
        char * local_buffer = (char *) arguments[1];
        int i = 0;
        while (i < arguments[2])
        {
          valid((const void *) local_buffer);
          local_buffer ++;
          i ++;
        }
        arguments[1] = check_get_page((const void *) arguments[1]);
        f->eax = sys_read(arguments[0], (void *) arguments[1], (unsigned) arguments[2]);
        break;
      }
      case SYS_WRITE:
      {
        pull_args(f, &arguments[0], 3);
        char * local_buffer = (char *) arguments[1];
        int i = 0;
        while (i < arguments[2])
        {
          valid((const void *) local_buffer);
          local_buffer ++;
          i ++;
        }
        arguments[1] = check_get_page((const void *) arguments [1]);
        f->eax = sys_write(arguments[0], (const void *) arguments[1], (unsigned) arguments[2]);
        break;
      }
      case SYS_SEEK: 
      {
        pull_args(f, &arguments[0], 2);
        sys_seek(arguments[0], (unsigned) arguments[1]);
        break;
      }
      case SYS_TELL:
      {
        pull_args(f, &arguments[0], 1);
        f->eax = sys_tell(arguments[0]);
        break;
      }
      case SYS_CLOSE:
      {
        pull_args(f, &arguments[0], 1);
        close(arguments[0]);
        break;
      }
  }
}

bool valid (const void *vaddr)
{
  if(!is_user_vaddr(vaddr))
    return false;
  else if (vaddr < (void *) 0x08048000)
    return false;
  else 
    return true;
}

void sys_halt (void)
{
  shutdown_power_off();
}

void sys_exit(int status)
{
  struct thread * t = thread_current();
  if(t->parent->status != THREAD_DYING)//BOOPITY
    t->child->status = status;
  printf("%s: exit(%d)\n", t->name, status);
  thread_exit();
}

pid_t sys_exec(const char * cmd_line)
{
  pid_t pid = process_execute(cmd_line);
  struct child_process * child = get_child(pid);
  while(child->loadflag == false)
    barrier();
  if(!child->loadflag)
    return -1;
  return pid;
}

int sys_wait (pid_t pid)
{
  return process_wait(pid);
}

bool sys_create(const char * file, unsigned initial_size)
{
  lock_acquire(&syscall_lock);
  bool temp = filesys_create(file, initial_size);
  lock_release(&syscall_lock);
  return temp;
}

bool sys_remove(const char *file)
{
  lock_acquire(&syscall_lock);
  bool temp = filesys_remove(file);
  lock_release(&syscall_lock);
  return temp;
}

int sys_open(const char * file)
{
  struct thread * t = thread_current();
  lock_acquire(&syscall_lock);
  struct file * tempfile = filesys_open(file);
  if(tempfile != NULL)
  {
    struct file_desc *proc = malloc(sizeof(struct file_desc));
    proc->file = tempfile;
    proc->fd = t->fd;
    t->fd++;
    list_push_back(&t->files, &proc->file_elem);
    lock_release(&syscall_lock);
    return proc->fd;
  }
  lock_release(&syscall_lock);
  return -1;
}

struct file * get_file(int fd)
{
  struct thread * t = thread_current();
  struct list_elem *e;
  e = list_begin(&t->files);
  while(e!= list_end(&t->files))
  {
    struct file_desc * filed = list_entry(e, struct file_desc, file_elem);
    if (fd == filed->fd)
    {
      return filed->file;
    }
    e = list_next(e);
  }   
  return (NULL);
}

int sys_filesize(int fd)
{
  lock_acquire(&syscall_lock);
  struct file *f = get_file(fd);
  if(f != NULL)
  {
    int size = file_length(f);
    lock_release(&syscall_lock);
    return size;
  }
  lock_release(&syscall_lock);
  return -1;
}

int sys_read(int fd, void *buffer, unsigned size)
{
  lock_acquire(&syscall_lock);
  struct file * f = get_file(fd);
  if(fd == STDIN_FILENO)
  {
    unsigned i = 0;
    uint8_t *local_buffer = (uint8_t *) buffer;
    while(i <size)
    {
      local_buffer[i] = input_getc();
      i++;
    }
    lock_release(&syscall_lock);
    return size;
  }
  if(f != NULL)
  {
    int temp = file_read(f, buffer, size);
    lock_release(&syscall_lock);
    return temp;
  }
  lock_release(&syscall_lock);
  return -1;
}
        
int sys_write(int fd, const void * buffer, unsigned size)
{
  lock_acquire(&syscall_lock);
  struct file *f = get_file(fd);
  if (fd == STDIN_FILENO)
  {
    putbuf(buffer, size);
    lock_release(&syscall_lock);
    return size;
  }
  if (f != NULL)
  {
    int temp = file_write(f, buffer, size);
    lock_release(&syscall_lock);
    return temp;
  }
  lock_release(&syscall_lock);
  return -1;
}

 void sys_seek (int fd, unsigned position)
 {
   lock_acquire(&syscall_lock);
   struct file *f = get_file(fd);
   if(f != NULL)
   {
     file_seek(f, position);
     lock_release(&syscall_lock);
   }
 }
        
unsigned sys_tell (int fd)
{
  lock_acquire(&syscall_lock);
  struct file *f = get_file(fd);
  if(f != NULL)
  {
    off_t pos = file_tell(f);
    lock_release(&syscall_lock);
    return pos;
  }
  lock_release(&syscall_lock);
  return -1;
}
        
void sys_close(int fd)
{
  lock_acquire(&syscall_lock);
  struct thread * t = thread_current();
  struct list_elem *next = list_begin(&t->files);
  struct list_elem *e = next;
  while(e != list_end(&t->files))
  {
    struct file_desc *f = list_entry(e, struct file_desc, file_elem);
    if(fd == f->fd)
    {
      file_close(f->file);
      list_remove(&f->file_elem);
      free(f);
      lock_release(&syscall_lock);
      return;
    }
    else if (fd == -1)
    {
      file_close(f->file);
      list_remove(&f->file_elem);
      free(f);
    }
    e = list_next(e);
  }
  lock_release(&syscall_lock);
}        
        
void pull_args(struct intr_frame *f, int *arg, int n)
{
  int i = 0, *ptr;
  while(i < n)
  {
     ptr = (int *) f->esp + i +1;
     valid((const void *) ptr);
    arg[i] = *ptr;
  }
}
        
int check_get_page(const void * vaddr)
{
  struct thread * t = thread_current();
  valid(vaddr);
  void * ptr = pagedir_get_page(t->pagedir, vaddr);
  if(ptr != NULL)
    return (int) ptr;
  else 
    sys_exit(-1);
    return -1;
}
        
struct child_process * get_child(int id)
{
  struct thread * t = thread_current();
  struct list_elem * e = list_begin(&t->children);
  while(e != list_end(&t->children))
  {
    struct child_process temp = list_entry(e, struct child_process, child_elem);
    
    e = list_next (e);
    
    if(id == temp->pid)
        return (temp);
    
  }
        return (NULL);
}
        
        
        
        

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
#include "decides/shutdown.h"
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
  if (!valid(const void*) f->esp))
  {
    sys_exit(-1);
  }
  switch (*(int *) f->esp)
  {
      case SYS_HALT:
      {
        halt();
        break;
      }
      case SYS_EXIT:
      {
      
      }
      case SYS_EXEC:
      {
      
      }
      case SYS_WAIT:
      {
      
      }
      case SYS_CREATE:
      {
      
      }
      case SYS_REMOVE:
      {
      
      }
      case SYS_OPEN:
      {
        
      }
      case SYS_FILESIZE:
      {
      
      }
      case SYS_READ: 
      {
      
      }
      case SYS_WRITE:
      {
      
      }
      case SYS_SEEK: 
      {
      
      }
      case SYS_TELL:
      {
      
      }
      case SYS_CLOSE:
      {
        
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

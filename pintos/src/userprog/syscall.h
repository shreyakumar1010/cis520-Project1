#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);

struct lock syscall_lock;

struct process_file
{
  struct file *file;
  int fd;
  struct list_elem elem;
};

int process_add_file (struct file *f);
struct file* process_get_file (int fd);

static void syscall_handler (struct intr_frame *);
int user_to_kernel_ptr(const void *vaddr);
void get_arg (struct intr_frame *f, int *arg, int n);
void check_valid_ptr (const void *vaddr);
void check_valid_buffer (void* buffer, unsigned size);



#endif /* userprog/syscall.h */

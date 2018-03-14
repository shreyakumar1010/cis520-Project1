#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

#define NOT_LOADED 0 //rwadd
#define LOAD_SUCCESS 1
#define LOAD_FAIL 2

struct child_process {
  int pid;
  int loadflag;  //not loaded is false, loaded is true and load fail is null
  bool waiting;
  bool exiting;
  int status;
  struct list_elem child_elem;
};

#endif /* userprog/process.h */

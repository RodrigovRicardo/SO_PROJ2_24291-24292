#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define exit_on_error(s,m)	if (s<0) {perror(m);exit(1);}

int create_semaphore(int key);
void destroy_semaphore(int id);

int create_mutex(int key);
void destroy_mutex(int id);

void down(int id);
void up(int id);

#endif
#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include "types.h"


typedef u64 File;


void *
malloc(u64);

void *
realloc(void *, u64);

void
free(void *);

u32
getpid();

File *
fopen(char *, char *);

void
fclose(File *);

u64
fwrite(const void *, u64, u64, File *);


File *
get_stdout_fd();


#endif
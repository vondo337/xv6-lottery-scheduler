#ifndef _PINFO_H_
#define _PINFO_H_

#define NPROC 64

typedef unsigned long uint64;

struct pinfo {
  int inuse[NPROC];
  int pid[NPROC];
  int tickets[NPROC];
  uint64 runtime_ticks[NPROC];
};

#endif
#ifndef _PINFO_H_
#define _PINFO_H_

#include "param.h"
#include "types.h"

struct pinfo {
  int inuse[NPROC];
  int pid[NPROC];
  int tickets[NPROC];
  uint64 runtime_ticks[NPROC];
};

#endif
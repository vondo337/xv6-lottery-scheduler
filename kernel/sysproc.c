#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
#include "pinfo.h" // Modification to add pinfo struct

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


// Modification to add sys_settickets function
uint64
sys_settickets(void){
  int tickets;

  argint(0, &tickets); // first argument from user space

  // process needs to have at least 1 ticket
  if(tickets < 1)
    return -1;
  
  // changing the ticket count of the current process
  struct proc *p = myproc();

  acquire(&p->lock);
  p->tickets = tickets;
  release(&p->lock);

  return 0;
}

// Modification to add sys_getpinfo function
uint64
sys_getpinfo(void){
  uint64 user_addr;
  struct pinfo info;

  argaddr(0, &user_addr); // first argument from user space

  // Gathering info from the process table
  if(getpinfo(&info) < 0)
    return -1;
  
  // copying the info from the kernel to the user space
  if(copyout(myproc()->pagetable, myproc()->sz, user_addr, (char *)&info, sizeof(info)) < 0)
    return -1;
  return 0;
}
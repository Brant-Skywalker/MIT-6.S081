#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;

  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 s_va;
  int n_scan;
  uint64 dst_va;
  uint buf = 0;
  pagetable_t pgtbl = myproc()->pagetable;  // Get the current process's page table.

  // Parse input arguments from registers.
  if (0 > argaddr(0, &s_va) || 0 > argint(1, &n_scan) || 0 > argaddr(2, &dst_va)) {
    return -1;
  }

  n_scan = n_scan > MAXSCAN ? MAXSCAN : n_scan;  // Truncate n_scan if necessary.
  for (int i = 0; i < n_scan; ++i) {
    pte_t* curr = walk(pgtbl, s_va + PGSIZE * i, 0);
    if (!curr || (~(*curr) & (PTE_V | PTE_U))) { return -1; }  // Sanity check: PTE_V and PTE_U must both be set.
    if (*curr & PTE_A) {  // Accessed!
      buf |= (1UL << i);  // Record in the buffer.
      *curr &= (~PTE_A);  // Recover the accessed bit.
    }
  }

  // Now copyout to the destination buffer.
  return copyout(pgtbl, dst_va, (char*) &buf, sizeof(buf));
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
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

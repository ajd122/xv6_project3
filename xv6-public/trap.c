#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

#ifndef VERBOSE_PRINT
#define VERBOSE_PRINT 0
#endif /*end VERBOSE_PRINT */

#ifndef SELECTION
#define SELECTION "FIFO"
#endif /*end SELECTION */


//#define ZERO_DEFINED 0
//#DEFINE ONE_DEFINED 1

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;
int MAX_TOTAL_PAGES = 30;
int MAX_PSYC_PAGES = 15;
//uint PGSIZE = 4096;
<<<<<<< HEAD
uint placeOnFile;

//int mappages(pde_t *pgdir, void *va, uint size, uint pa, int perm);
char* selectVictimPage();
//void writeToSwapFile(struct proc * p, char* pte, uint placeOnFile, uint size);
//void readFromSwapFile(struct proc * p, char* pte, uint placeOnFile, uint size);
int strcmp(const char *s1, const char *s2);

=======

static int mappages(pde_t *pgdir, void *va, uint size, uint pa, int perm);
>>>>>>> e0fde562f06c2f0d4d53cfc3fd5839fe57b048f8

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);

  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(myproc()->killed)
      if(VERBOSE_PRINT){
	procdump();
      }
      exit();
    myproc()->tf = tf;
    syscall();
    if(myproc()->killed)
      if(VERBOSE_PRINT){
	procdump();
      }
      exit();
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(myproc() && myproc()->state == RUNNING){
	
	//record PTEs with PTE_A set
	//update LRU data structure
	//reset PTE_A for all PTEs
    }
    if(cpuid() == 0){
      acquire(&tickslock);
      ticks++;
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpuid(), tf->cs, tf->eip);
    lapiceoi();
    break;
  case T_PGFLT:
    myproc()->page_faults++;
<<<<<<< HEAD
    char *pte = selectVictimPage();
=======
>>>>>>> e0fde562f06c2f0d4d53cfc3fd5839fe57b048f8
    if(myproc()->total_pages > MAX_TOTAL_PAGES){
      myproc()->killed = 1;
    } else {
//      if(*pte & PTE_PG){
//	myproc()->total_pages++;
//      }
      if(myproc()->total_physical_pages <  MAX_PSYC_PAGES){
	myproc()->total_physical_pages++;
        char *frame = kalloc();
<<<<<<< HEAD
	mappages(myproc()->pgdir, (char*) rcr2(), PGSIZE, V2P(frame), PTE_W|PTE_U); 
=======
	mappages(myproc()->pgdir, rcr2(), PGSIZE, V2P(frame), PTE_W|PTE_U); 
>>>>>>> e0fde562f06c2f0d4d53cfc3fd5839fe57b048f8
	memset(frame, 0, PGSIZE);
	//add to FIFO
      } else {
	
	//GET PLACE ON FILE
	placeOnFile = myproc()->page_location[0];
        myproc()->paged_out++;
	myproc()->total_page_outs++;
	
	writeToSwapFile(myproc(), pte, placeOnFile, PGSIZE);
	//PTE_P -> ZERO_DEFINED 0;
        *pte &= -PTE_P;
	//PTE_PG -> ONE_DEFINED 1;
	//*pte |= PTE_PG;
	mappages(myproc()->pgdir, (char*) rcr2(), PGSIZE, PTE_ADDR(*pte), PTE_W|PTE_U);
        //add pte to FIFO
      }
      if(*pte == rcr2()){
	myproc()->paged_out--;
	//GET PLACE ON FILE
	//placeOnFile = SOMETHING;
        placeOnFile = myproc()->page_location[0];
	readFromSwapFile(myproc(), pte, placeOnFile, PGSIZE);
      } else {
	memset(pte, 0, PGSIZE);	
      }
    }
    break;
  //PAGEBREAK: 13
  default:
    if(myproc() == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpuid(), tf->eip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            myproc()->pid, myproc()->name, tf->trapno,
            tf->err, cpuid(), tf->eip, rcr2());
    myproc()->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    if(VERBOSE_PRINT){
	procdump();	
    }
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(myproc() && myproc()->state == RUNNING &&
     tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

  // Check if the process has been killed since we yielded
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    if(VERBOSE_PRINT){
	procdump();
    }
    exit();
}

char*
selectVictimPage(){
  char *pte;
  pte = myproc()->kstack;
//  if(strcmp(SELECTION, "FIFO")){
//	//GONNA DO FIFO AS A LINKED LIST I GUESS
//	uint pte_target = myproc()->page_location[0];
//	//fifo_head = fifo_head->next;
//	//iterate through ptes looking for target
//	return pte;
//  }else if(SELECTION == "RAND"){
//	//do RAND shit
//	uint rand_num = rand_generator(myproc()->total_pages - myproc()->paged_out);
//	//get the rand_numth pte
//	return pte
//  }else if(SELECTION == "LRU"){
//	//LRU IS A STACK, RIGHT?
//	uint pte_target = myproc()->lru[myproc()->lru_bottom];
//	//find that pte
  return pte;
}
static unsigned long next = 1;

int rand_generator(int max){
	unsigned long rand1 = 1103515245;
	unsigned long rand2 = 12345;
	next = rand1  * next + rand2;
	return (((unsigned int)(next/65536) % 32768) % max + 1);
}

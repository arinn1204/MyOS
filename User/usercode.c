#include "usercode.h"
#include "print.h"

char *cmd[]={"getpid", "ps", "chname", "kfork", "switch", "wait", "exit", 0};

int show_menu()
{
   printf("***************** Menu *******************\n");
   printf("*  ps  chname  kfork  switch  wait  exit *\n");
   printf("******************************************\n");
}

int find_cmd(char *name)
{
   int i=0;   
   char *p=cmd[0];

   while (p){
         if (strncmp(p, name, strlen(p))==0)
            return i;
         i++;  
         p = cmd[i];
   } 
   return(-1);
}

int getpid()
{
   syscall(0,0,0);
}

int ps()
{
   syscall(1, 0, 0);
}

int chname()
{
    char s[64];
    printf("input new name : ");
    gets(s);
    syscall(2, s, 0);
}

int kfork()
{
  #ifndef _LAB_3_
    int pid;
    printf("proc %d enter kernel to kfork a child\n", getpid());
    pid = syscall(3, 0, 0);
    if (pid>0)
      printf("proc %d kforked a child %d\n", getpid(), pid);
    else
      printf("kforked failed\n");
  #else
    int pid;
    printf("proc ");  getpid(); printf("enter kernel to kfork a child\n");
    syscall(3, 0, 0);
    //printf("proc %d kforked a child %d\n", getpid(), pid);
  #endif
}    

int kswitch()
{
    #ifndef _LAB_3_
      printf("proc %d enter Kernel to switch proc\n", getpid());
      syscall(4,0,0);
      printf("proc %d back from Kernel\n", getpid());
    #else
      printf("proc "); getpid(); printf(" enter Kernel to switch proc\n");
      syscall(4,0,0);
      printf("proc "); getpid(); printf(" back from Kernel\n");
    #endif
}

int wait()
{
  #ifndef _LAB_3_
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(5, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d ", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n"); 
  #else
    int exitValue;
    printf("Proc "); getpid(); printf(" enter kernel to wait for child to die.\n");
    syscall(5,&exitValue, 0);
    printf("Proc "); getpid(); printf(" back from wait\n");
  #endif
} 

int exit()
{
   int exitValue;
   printf("\nenter an exitValue (0-9) : ");
   exitValue=(getc()&0x7F) - '0';
   printf("enter kernel to die with exitValue=%d\n", exitValue);
   _exit(exitValue);
}

int _exit(int exitValue)
{
  syscall(6,exitValue,0);
}

int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
}


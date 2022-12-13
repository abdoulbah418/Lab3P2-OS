#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void ParentProcess(int []);
void ClientProcess(int []);


void ParentProcess(int SharedMemory[]) {
  int i, account, randnum;
  srand(getpid()*2);
  
  for ( i = 0; i < 25; i++){
    sleep(rand() % 6);
    
    while(SharedMemory[1] != 0);

    account = SharedMemory[0];
    if (account <= 100) {
      randnum = rand() % 101;
      
      if (randnum % 2 == 0) { 
        account += randnum;
        
        printf("Dear old Dad: Deposits $%d / Balance = $%d\n", randnum, account);
        
      }
      else {
        printf("Dear old Dad: Doesn't have any money to give\n");
      }
      SharedMemory[0] = account;
      SharedMemory[1] = 1;
    } 
    else {
      printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
      // The instructions do not say to set Turn = 1 if account is <= 100.
      // Only in the condition that is is depositing money.
      // SharedMemory[1] = 1;
    }
  }
}

void  ChildProcess(int SharedMemory[]) {
  int i, account, randnum;
  srand(getpid()*3);
  
  for(i = 0; i < 25; i++) {
    sleep(rand() % 6);
    
    while (SharedMemory[1] != 1);

    account = SharedMemory[0];

    randnum = rand() % 51;
    printf("Poor Student needs $%d\n", randnum);
    
    if (randnum <= account) {
      account -= randnum;
      printf("Poor Student: Withdraws $%d / Balance = $%d\n", randnum, account);
      SharedMemory[0] = account;
      
    }
    else {
      printf("Poor Student: Not Enough Cash ($%d)\n", account );
    }
    SharedMemory[1] = 0;
  }
}

int  main(int  argc, char *argv[])
{
     int    ShmID;
     int    *ShmPTR;
     pid_t  pid;
     int    status;

    //  if (argc != 5) {
    //       printf("Use: %s #1 #2 #3 #4\n", argv[0]);
    //       exit(1);
    //  }

     ShmID = shmget(IPC_PRIVATE, 4*sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
    //  printf("Server has received a shared memory of four integers...\n");

     ShmPTR = (int *) shmat(ShmID, NULL, 0);
     if (*ShmPTR == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     printf("Server has attached the shared memory...\n");

     ShmPTR[0] = 0;
     ShmPTR[1] = 0;
    //  ShmPTR[0] = atoi(argv[1]);
    //  ShmPTR[1] = atoi(argv[2]);
    //  ShmPTR[2] = atoi(argv[3]);
    //  ShmPTR[3] = atoi(argv[4]);
    //  printf("Server has filled %d %d %d %d in shared memory...\n",
    //         ShmPTR[0], ShmPTR[1], ShmPTR[2], ShmPTR[3]);

     printf("Server is about to fork a child process...\n");
     pid = fork();
     if (pid < 0) {
          printf("*** fork error (server) ***\n");
          exit(1);
     }
     else if (pid == 0) {
          ChildProcess(ShmPTR);
          exit(0);
     } else {
          ParentProcess(ShmPTR);
     }

     wait(&status);
     printf("Server has detected the completion of its child...\n");
     shmdt((void *) ShmPTR);
     printf("Server has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Server has removed its shared memory...\n");
     printf("Server exits...\n");
     exit(0);
}

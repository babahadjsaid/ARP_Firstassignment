//TODO switch to terminator

#define MAIN 2
#include "mu.h"

/*                                      Signal Handler                                   */
void handler(int sig) { 
  for (int i = 0; i < NUM_PROC; i++)
  {
    PrintLog("Killing the process %s",CAA[i]);
    kill(ReadPID(CAA[i]), SIGINT);
  }

  fclose(LogFile);
  sleep(2);
  exit(EXIT_SUCCESS);
} 
/*                                      End Signal Handler                               */

int main(){
    CreateLog(ProcessNAme);
    PrintLog("Setting up Signal Handler...\n");
    signal(SIGINT, handler);
    WritePID(ProcessNAme);

    int status;
    char* tmp;
    for (int  i = 0; i < NUM_PROC; i++)
    {

      PrintLog("Creating the Process %s...\n",CAA[i]);
      sprintf(tmp,"./bin/%s",CAA[i]);
      if (i > FIRST_BACKGROUND_P)
      {
        args[0] = tmp;
        args[1] = NULL;
        spawn(V tmp,args,&children[i]);
      }else
      {
        args[2] = tmp;
        spawn(V"konsole",args,&children[i]); 
      }   
    }
    
    printf("Finished Creating Processes\n");fflush(stdout);
   /*
    TODO : Fix bugs of watch dog 
    while (1)
    {
      sleep(9);
      int min = INT32_MAX;
      for (int  i = 0; i < NUM_PROC; i++)
      {
        int tmp = GetTimeNow() - GetLastEdit(CAA[i]);
          if(tmp<=min){
            min = tmp;
          }
      }
      printf("%d",min);
        if (min+1 >= 20){
          kill(getpid(),SIGINT);
          sleep(1);
          goto waiting;

        }
      sleep(1);
      
    }
     waiting:
    */
    pid_t wpid;
    while ((wpid = wait(&status)) > 0){
        PrintLog("The process %d exited with status: %d\n",wpid,status);
    }
  exit(EXIT_SUCCESS);

}


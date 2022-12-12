//TODO switch to terminator

#define MASTER 2
#include "./../include/mu.h"


/*                                      Signal Handler                                   */
void handler(int sig) { 
  
  for (int i = 0; i < NUM_PROC; i++){
      PrintLog("Killing the process %s\n",CAA[i]);
      kill(ReadPID(CAA[i]), SIGINT);
    }
  pid_t wpid;
  int status;
  while ((wpid = wait(&status)) > 0){
      printf("The process %d exited with status: %d\n",wpid,status);
      if (status == -3)
      {
        kill(getpid(), SIGINT);
      }

  }
  fclose(LogFile);
  exit(EXIT_SUCCESS);
} 
/*                                      End Signal Handler                               */

int main(){
    CreateLog(ProcessNAme);
    PrintLog("Setting up Signal Handler...\n");
    signal(SIGINT, handler);
    WritePID(ProcessNAme);// Can be replaced with a database

    
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

    /*                             Start Watch Dog                                       */
    while (1)
    {
      sleep(WD_P);
      int min = INT32_MAX;
      for (int  i = 0; i < NUM_PROC; i++)
      {
        int tmp = GetTimeNow() - GetLastEdit(CAA[i]);
          if(tmp<=min){
            min = tmp;
          }
      }
      PrintLog("%d\n",min);
        if (min+1 >= WD_T){
          kill(ReadPID(CMDF),SIGUSR2);
        }
    }
    /*                             End Watch Dog                                       */
    return 0;

}

//TODO: 
// * start an error code schema

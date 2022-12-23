#define WatchDog 
#define PROCESS_MANAGMENT 
#include "include/mu.h"


/*                                      Signal Handler                                   */

void handler(int sig) { 
  fclose(LogFile);
  exit(EXIT_SUCCESS);
  
} 
/*                                      End Signal Handler                               */



int main(){
    CreateLog(ProcessNAme);
    PrintLog("Setting up Signal Handler...\n");
    signal(SIGINT, handler);
    WritePID(ProcessNAme);// Can be replaced with a database

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
        if (min+1 >= WD_T){
          PrintLog("%d seconds of being inactive Rewinding.\n",min);
          kill(ReadPID(CMDF),SIGUSR2);
        }
    }
    
    return 0;

}


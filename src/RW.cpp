#define RW 
#include "include/mu.h"

/*                                      Signal Handler                                   */
void signaleHandler(int sig){
    GarbgeCollection(NUM_PIPES);
    
}
/*                                      End Signal Handler                               */


int main(){
    CreateLog(ProcessNAme);
    PrintLog("Setting up Signal Handler...\n");
    signal(SIGINT,signaleHandler);
    WritePID(ProcessNAme);
    CreatePipes(3);
    Data data;
    float x = 0,y = 0;

    while(1){
        PipeToSelect(2);
        if (choice[0]) {
            ReceiveData(fd[0],&x);
            data.p[0] = x + x*0.05*(rand()*2)/(RAND_MAX+0.0f);
            SendData(fd[2],&data);
            goto printing;
            }
        if (choice[1]) { 
            ReceiveData(fd[1],&y);
            data.p[1] = y + y*0.005*(rand()*2)/(RAND_MAX+0.0f);
            SendData(fd[2],&data);
            goto printing;
            }
        continue;
        printing:PrintLog("Received true x: %2f calc: %2f, true y: %2f calc: %2f \r",x,data.p[0],y,data.p[1]);
    
    }
    return 0;
}
    

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
        /* select the pip to Read from M1 or M2 if both ready 
                the function will randomally select a pipe*/
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
        /*If no pipe was selected continue */
        continue;
        printing:PrintLog("Noisy X value: %2f, noisy Y value: %2f \r",data.p[0],data.p[1]);
    
    }
    return 0;
}
    

#define M1 1
#include "./../include/mu.h"


/*                                      Signal Handler                                   */
void signaleHandler(int sig){
    GarbgeCollection(NUM_PIPES);
}
/*                                      End Signal Handler                               */

int main(){
    // Set up signal handler
    CreateLog(ProcessNAme);
    PrintLog("Setting up Signal Handler...\n");
    signal(SIGINT,signaleHandler);
    //Write the P ID
    WritePID(ProcessNAme);
    //Create The Pipes For communication
    CreatePipes(2);
    // Define Variables
    float speed = 0;
    float x = 0;
    char ch;

    while(1){
        // if there is new speed to apply Read it and update the Speed Var
        PipeToSelect(1);
        if (choice[0])
        {
            ReceiveData(fd[0],&speed);   
        }
        
        if (speed == 0)
        {
            continue;
        } 
        
        // if the hoist reaches the limit of its track signal an emergency stop
        if (x + speed*SAMPLING_PERIODE US_S>=100 or x + speed*SAMPLING_PERIODE US_S<0)
        {
            kill(ReadPID(CMDF),SIGUSR1);
            goto Send;
        }
        // else update the postion according to the current speed 
        x += speed*SAMPLING_PERIODE US_S;
        // send the new X location to the real world to apply noise to it 
        
        Send:SendData(fd[1],&x);
        // Repeat this process indefinitly
        
    }
}

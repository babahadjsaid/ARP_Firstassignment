#define M2 1
#include "mu.h"

/*                                      Signal Handler                                   */
void signaleHandler(int sig){
    GarbgeCollection(NUM_PIPES);
}
/*                                      End Signal Handler                               */

int main(){
    CreateLog(ProcessNAme);
    // Set up signal handler
    PrintLog("Setting up Signal Handler...\n");
    signal(SIGINT,signaleHandler);
    //Write the P ID
    WritePID(ProcessNAme);
    //Create The Pipes For communication
    CreatePipes(2);
    // Define Variables
    float speed = 0;
    float y = 0;
    char ch;
    while(1){
        // if there is new speed to apply Read it and update the Speed Var
        int* choice = PipeToSelect(1);
        if (choice[0])
        {
            ReceiveData(fd[0],&speed);
            
        }
        //for memory Managment
        delete(choice);
        if (speed == 0)
        {
            continue;
        }
        // if the hoist reaches the limit of its track signal an emergency stop
        if (y + speed*0.1f>=100 or y + speed*0.1f<0)
        {
            kill(ReadPID(V"cmd"),SIGUSR1);
            goto Send;
        }
        // else update the postion according to the current speed 

        y += speed*0.1f;
        
        // send the new Y Coordinate to the real world to apply noise to it 
        Send:SendData(fd[1],&y);
        // Repeat this process indefinitly

    }
}

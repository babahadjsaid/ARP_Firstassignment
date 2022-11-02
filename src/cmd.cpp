#define CMD 1
#include "mu.h"


/*                                      Signal Handlers                                   */
void signaleHandler(int sig){
    GarbgeCollection(NUM_PIPES);
    kill(getppid(),SIGKILL);
}
void Stop(int num , siginfo_t* info, void* vp){
    // if the signal comes from M1 or inspect means it concerns X axis
    if(ReadPID(V"M1") == info->si_pid or ReadPID(V"inspect") == info->si_pid){
        //printf("Emergency Stop for x-axis...\r");//this will be printed into the log file
        // if it is from M1 means that the hoist hitted the limits and we should 
        // update the no hit variable
        if (ReadPID(V"M1") == info->si_pid)
        {
            nohitx = false;
        }// otherwise don't update the variable because it is just a stop command
        // in both cases set the speed to zero 
        speed1 = 0.0f;
        // and send the speed value 
        SendData(fd[0],&speed1);
        } 

    // if the signal comes from M2 or inspect means it concerns Y axis
    if (ReadPID(V"M2") == info->si_pid or ReadPID(V"inspect") == info->si_pid){
        //printf("Emergency Stop for y-axis...\r");//this will be printed into the log file
        // if it is from M1 means that the hoist hitted the limits and we should 
        // update the no hit variable
        if (ReadPID(V"M2") == info->si_pid)
        {
        nohity = false;
        }// otherwise don't update the variable because it is just a stop command
        // in both cases set the speed to zero 
        speed2 = 0.0f;
        // and send the speed value
        SendData(fd[1],&speed2);
        }
}
void Rewind(int num){
    //When receiving the rewind signal start moving the hoist to the initial position 0,0
    //while the hoist didn't hit the limit of both axis keep repeating 
    while(nohitx or nohity){
        //if it didn't hit x axis keep moving back
        if (nohitx)
        {
            // negative speed will get it back to its initial pos
            // Note here if we had a sort of a position feedback 
            // we can implement a closed loop controller.. 
            // this could be a possible improvement
            speed1 = -5;
            SendData(fd[0],&speed1);
        }
        //if it didn't hit x axis keep moving back
        if(nohity)
        {
            speed2 = -5;
            SendData(fd[1],&speed2);
        }
        // print the current speed
        printf("speed1:%2f, speed2:%2f\r",speed1,speed2);fflush(stdout);
        // sleep for REWIND_PERIODE
        usleep(REWIND_PERIODE);
    }
    printf("\nRewind Completed Please Wait a bit... \n");
    sleep(2);
    // update the variable to no hit because the hoist is ready to move
    nohitx = true;
    nohity = true;
}
/*                                      End Signal Handlers                               */


int main(){
    CreateLog(ProcessNAme);
    // Set up signal handler
    PrintLog("Setting up Signal Handler...\n");
    signal(SIGINT,signaleHandler);
    SignalWithInfo(SIGUSR1, Stop);
    signal(SIGUSR2,Rewind);
    
    //Write the P ID
    WritePID(ProcessNAme);

    //Create The Pipes For communication
    CreatePipes(NUM_PIPES);

    // Define Variables
    char ch;

    while(1){
        if ( kbhit() ) {
            //Check the pressed key and perform the coressponding action
            ch = getch();
            switch (ch)
            {
            case 'z': speed2+=0.1;SendData(fd[1],&speed2);
                goto PNdC;
            case 'e': speed2=0;SendData(fd[1],&speed2);
                goto PNdC;
            case 's': speed2 -=0.1;SendData(fd[1],&speed2);
                goto PNdC;
            case 'd': speed1+=0.1;SendData(fd[0],&speed1);
                goto PNdC;
            case 'a': speed1 = 0;SendData(fd[0],&speed1);
                goto PNdC; 
            case 'q': speed1-=0.1;SendData(fd[0],&speed1);
                goto PNdC;
            default:
                goto Continue;

            }
            
            PNdC://Print And Continue
            printf("speed1:%2f, speed2:%2f\r",speed1,speed2);
            //update the Boolean Var status no hit
            nohitx = true;
            nohity = true;

            Continue:;//Continue
        }
        usleep(SAMPLING_PERIODE_KEYS);//increasing the delay further makes a strange behavior when the key is pressed constantly 
    }
}


/*
void MainExit(int num){
    sleep(10);
    remove("/tmp/CMD_m1");
    remove("/tmp/CMD_m2");
    close(fd[0]); 
    close(fd[1]);
    
    exit(num);
}

*/
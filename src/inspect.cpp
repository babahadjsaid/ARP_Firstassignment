#define Inspect 1
#include "mu.h"




/*                                      Signal Handler                                   */
void signaleHandler(int sig){
    GarbgeCollection(NUM_PIPES);
    kill(getppid(),SIGINT);
}
/*                                      End Signal Handler                               */



int main(){
    //Create the Log file
    CreateLog(ProcessNAme);
    //Set up signal handler
    PrintLog("Setting up Signal Handler...\n");
    signal(SIGINT,signaleHandler);
    //Save PID for process communication through signals
    WritePID(ProcessNAme);
    //Create Pipes for process communication
    CreatePipes(1);
    // variable that holds the pressed key
    char ch;
    //Data to transfer through the pipe
    Data data;

    while (1)
    {
        if ( kbhit() ) {
            ch = getch();
            if (ch=='s')
            {
                //if the pressed key is 's' Stop the hoist
                PrintLog("Stoping the hoist!!!!\n");
                kill(ReadPID(V"cmd"),SIGUSR1);
            }else if (ch=='r')
            {
                // if the pressed key is 'r' rewind the hoist to the initial pos
                PrintLog("Rewinding!!!!\n");
                kill(ReadPID(V"cmd"),SIGUSR2);
            }
        }
        //if there is data in the channel display it 
        int* choice = PipeToSelect(1);
        if(choice[0]){
            ReceiveData(fd[0],&data);
            printf("positions x:%.2f,y:%.2f\r",data.p[0],data.p[1]);
        }
        //collect the variable choice as it is a variable in the heap area
        // it is bad for real time operation (non deterministic property) 
        // I will be replacing it by a global variable
        delete(choice);
        
    }
    
}



/*

void signal_handler(int signal_num)
{
    sleep(10);
    remove("/tmp/rw_in");
    close(fd);
    
    exit(signal_num);
}



template <class T>
void myfunction(T* f){
    printf("size of float is %d and it is %d\n",sizeof(*f),*f);
}
struct tt
{
    int p[12];
    char name ;
};


int main(){
    float speeds[2] = {0.1,0.2};
    int g = 121;
    char t = '1';
    tt y = {{1,1,1,1,1,1,1,1,1,1,1,1},'r'};
    while (1)
    {
        printf("speeds it actualy is %d ",(int)sizeof(speeds));
        myfunction(speeds);
        printf("speed it actualy is %d ",(int)sizeof(speeds[0]));
        myfunction(&speeds[0]);
        printf("g it actualy is %d ",(int)sizeof(g));
        myfunction(&g);
        printf("t it actualy is %d ",(int)sizeof(t));
        myfunction(&t);
        printf("y it actualy is %d ",(int)sizeof(y));
        myfunction(&y);
        
        
    }
    
}
*/


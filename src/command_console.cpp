#define CMD 1
#include "./../include/mu.h"

#include "./../include/command_utilities.h"


/*                                      Signal Handlers                                   */
void signaleHandler(int sig){
    endwin();
    GarbgeCollection(NUM_PIPES);
    
}
void Stop(int num , siginfo_t* info, void* vp){
    // if the signal comes from M1 or inspect means it concerns X axis
    if(ReadPID(M1F) == info->si_pid or ReadPID(InspectF) == info->si_pid){
        //printf("Emergency Stop for x-axis...\r");//this will be printed into the log file
        // if it is from M1 means that the hoist hitted the limits and we should 
        // update the no hit variable
        if (ReadPID(M1F) == info->si_pid)
        {
            nohitx = false;
        }// otherwise don't update the variable because it is just a stop command
        // in both cases set the speed to zero 
        speed1 = 0.0f;
        // and send the speed value 
        SendData(fd[0],&speed1);
        PrintLog("Signal Received From M1\n");
        } 

    // if the signal comes from M2 or inspect means it concerns Y axis
    if (ReadPID(M2F) == info->si_pid or ReadPID(InspectF) == info->si_pid){
        //printf("Emergency Stop for y-axis...\r");//this will be printed into the log file
        // if it is from M1 means that the hoist hitted the limits and we should 
        // update the no hit variable
        PrintLog("Signal Received From M2\n");
        if (ReadPID(M2F) == info->si_pid) //TODO redendent function call to optimise
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
        PrintLog("speed1:%2f, speed2:%2f\r",speed1,speed2);fflush(stdout);
        // sleep for REWIND_PERIODE
        usleep(REWIND_PERIODE);
    }
    PrintLog("\nRewind Completed Please Wait a bit... \n");
    sleep(2);
    // update the variable to no hit because the hoist is ready to move
    nohitx = true;
    nohity = true;
}
/*                                      End Signal Handlers                               */

int main(int argc, char const *argv[])
{
    CreateLog(ProcessNAme);
    // Set up signal handler
    PrintLog("Setting up Signal Handler...\n");
    signal(SIGINT,signaleHandler);
    SignalWithInfo(SIGUSR1, Stop);
    signal(SIGUSR2,Rewind);
    
    //Write the PID
    WritePID(ProcessNAme);

    //Create The Pipes For communication
    CreatePipes(NUM_PIPES);

    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initialize User Interface 
    init_console_ui();
    
    // Infinite loop
    while(TRUE)
	{	
        // Get mouse/resize commands in non-blocking mode...
        int cmd = getch();

        // If user resizes screen, re-draw UI
        if(cmd == KEY_RESIZE) {
            if(first_resize) {
                first_resize = FALSE;
            }
            else {
                reset_console_ui();
            }
        }
        // Else if mouse has been pressed
        else if(cmd == KEY_MOUSE) {

            // Check which button has been pressed...
            if(getmouse(&event) == OK) {

                // Vx++ button pressed
                if(check_button_pressed(vx_incr_btn, &event)) {
                    speed1+=0.1;
                    SendData(fd[0],&speed1);
                }

                // Vx-- button pressed
                else if(check_button_pressed(vx_decr_btn, &event)) {
                    speed1-=0.1;
                    SendData(fd[0],&speed1);
                }

                // Vx stop button pressed
                else if(check_button_pressed(vx_stp_button, &event)) {
                    speed1 = 0;
                    SendData(fd[0],&speed1);
                }

                // Vz++ button pressed
                else if(check_button_pressed(vz_incr_btn, &event)) {
                    speed2+=0.1;
                    SendData(fd[1],&speed2);
                }

                // Vz-- button pressed
                else if(check_button_pressed(vz_decr_btn, &event)) {
                    speed2 -=0.1;
                    SendData(fd[1],&speed2);
                }

                // Vz stop button pressed
                else if(check_button_pressed(vz_stp_button, &event)) {
                    speed2=0;
                    SendData(fd[1],&speed2);
                }
                PrintLog("speed1:%2f, speed2:%2f\r",speed1,speed2);
                //update the Boolean Var status no hit
                nohitx = true;
                nohity = true;
            }

        }

        refresh();
        usleep(SAMPLING_PERIODE_KEYS);//increasing the delay further makes a strange behavior when the key is pressed constantly 
	}

    // Terminate
    
    return 0;
}

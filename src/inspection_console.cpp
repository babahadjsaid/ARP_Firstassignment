#define Inspect 1
#include "./../include/mu.h"
#include "./../include/inspection_utilities.h"




/*                                      Signal Handler                                   */
void signaleHandler(int sig){
    endwin();
    GarbgeCollection(NUM_PIPES);
    
}
/*                                      End Signal Handler                               */


int main(int argc, char const *argv[]){
    //Create the Log file
    CreateLog(ProcessNAme);
    //Set up signal handler
    PrintLog("Setting up Signal Handler...\n");
    signal(SIGINT,signaleHandler);
    //Save PID for process communication through signals
    WritePID(ProcessNAme);
    //Create Pipes for process communication.
    CreatePipes(NUM_PIPES);
    //Data to transfer through the pipe.
    Data data;
    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // End-effector coordinates
    float ee_x, ee_y;

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

                // STOP button pressed
                if(check_button_pressed(stp_button, &event)) {
                    PrintLog("Stoping the hoist!!!!\n");
                    kill(ReadPID(CMDF),SIGUSR1);
                }

                // RESET button pressed
                else if(check_button_pressed(rst_button, &event)) {
                    PrintLog("Rewinding!!!!\n");
                    kill(ReadPID(CMDF),SIGUSR2);
                }
            }
        }
        //if there is data in the channel display it 
        PipeToSelect(1);
        if(choice[0]){
            ReceiveData(fd[0],&data);
            ee_x = floor(data.p[0]);
            ee_y = floor(data.p[1]/4);
            
        }
        //collect the variable choice as it is a variable in the heap area
        // it is bad for real time operation (non deterministic property) 
        // I will try to replace it by a global variable
        

        // To be commented in final version...
        switch (cmd)
        {
            case KEY_LEFT:
                ee_x --;
                break;
            case KEY_RIGHT:
                ee_x ++;
                break;
            case KEY_UP:
                ee_y--;
                break;
            case KEY_DOWN:
                ee_y++;
                break;
            default:
                break;
        }
        
        // Update UI
        update_console_ui(&ee_x, &ee_y);
	}

    // Terminate
    
    return 0;
}

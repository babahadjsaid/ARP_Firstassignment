#define UTILS 


#include "include/mu.h"


/**
 * @brief this function is used to create a new process. 
 * 
 * @param comands commands to execute.
 * @param args the arguments to the newly created process.
 * @param pid pointer to the pid of the created process it might contain the error status in case of failure of the fork sys call.
 * @return int 
 */

int spawn(char* comands,char* args[],int *pid){
    pid_t pd = fork();
    *pid = pd;
    if (pd != 0)
    {
        return pd;
    }
    else
    {
        execvp(comands,args);
        PrintLog("Failed to execute the process: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief //create/modify(if file exist) a file to write the pid of the calling process.
 * 
 * @param Fname name of the File
 */
void WritePID(char* Fname){ 
     
    PrintLog("Saving the %s Process ID...\n",Fname);
    // if the dolder .data is not already created,create it.
    mkdir("./.data/",0666);
    char ff[20];
    sprintf(ff,"./.data/%s.txt",Fname);
    
    FILE *f;
    if ((f = fopen(ff,"w"))==NULL)
    {
        PrintLog("Error Writing PID: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    

    int pid = getpid();
    fprintf(f,"%d",pid);
    //close file
    fclose(f);
}


/**
 * @brief Read the pid from a specific file 
 * 
 * @NOTE: This could be later interfaced with a database also.
 * @param Fname name of the file from which we get the pid. 
 * @return int the pid of in the requested file.
 */
int ReadPID(char* Fname){
    char ff[30];
    PrintLog("Reading PID of %s From %s...\n",Fname,ProcessNAme);
    sprintf(ff,"./.data/%s.txt",Fname);
    FILE* f;
    if((f = fopen(ff,"r"))== NULL){
        PrintLog("Error opening PID file: %s\n",strerror(errno));
        kill(ReadPID(MASTERF),SIGKILL);
    }
    int pid;
    fscanf(f,"%d",&pid);
    //close file
    fclose(f);
    return pid;
}
/**
 * @brief Create a Pipes from a process specific pip array.
 * 
 * @param numPipes the number of pipes to create.
 */
void CreatePipes(int numPipes){
    PrintLog("Starting process %s,creating %d pipes\n",ProcessNAme,numPipes);fflush(stdout);
    PrintLog("Starting pipes..\r");
    for (int i = 0; i < numPipes; i++)
    {
        char filename[15];
        char lastfilename[30];
        int mode;
        
        sscanf(PipeFN[i],"%s %d",filename,&mode);
        sprintf(lastfilename,"/tmp/%s",filename);
        if((mkfifo(lastfilename, 0666)==-1) ){
            if (access(lastfilename,F_OK)==-1)
            {
                PrintLog("A problem in makefifo pipe: %s\n",strerror(errno));
                kill(ReadPID(MASTERF),SIGINT);
            }
        }
        
        if ((fd[i] = open(lastfilename,openmode[mode]))==-1)
        {
        
            PrintLog("Couldn't open the pipe: %s\n",strerror(errno));
            kill(ReadPID(MASTERF),SIGINT);
        }
       
    }
    PrintLog("Starting pipes..All set.\n");
}

/**
 * @brief This function is to be called at the end of each process to free all the held resources.
 * 
 * @param numFiles number of pipes used by this process.
 */
void GarbgeCollection(int numFiles)
{
    PrintLog("Removing and Closing all temp files \n");
    for (int i = 0; i < numFiles; i++)
    {
        char filename[15];
        char ff[20];
        int mode;
        sscanf(PipeFN[i],"%s %d",filename,&mode);

        sprintf(ff,"/tmp/%s",filename);
        PrintLog("removing file %s\n",ff);
        //Check if the file exist before removing
        if(close(fd[i])==-1 ){
            PrintLog("Couldn't Close pip %d The error is: %s\n",fd[i],strerror(errno));
        }
        if (access(ff,F_OK)!=-1)
        {
            int status;
            if ((status = remove(ff))==-1)
            {
                PrintLog("Couldn't remove pip file %d The error is: %s\n",status,strerror(errno));
            }
            
        }
    }

    PrintLog("Closed Everything Quiting...\n");
    if(fclose(LogFile)==-1 ){
            printf("Couldn't Close Log file The error is: %s\n",strerror(errno));
        }
    exit(EXIT_SUCCESS);
    
}

/**
 * @brief a function used to set the handler such that it holds information like the pid of
 * the process issueing the signal.
 * 
 * @param signum Signal number.
 * @param handler the handler of the signal.
 */
void SignalWithInfo(int signum, InfoHandler* handler)
{
  struct sigaction action, old_action;

  memset(&action, 0, sizeof(struct sigaction));
  action.sa_sigaction = handler;  
  sigemptyset(&action.sa_mask); /* block sigs of type being handled */
  action.sa_flags = SA_RESTART|SA_SIGINFO; /* restart syscalls if possible */

  if (sigaction(signum, &action, &old_action) < 0)
            PrintLog("Signal error: %s\n",strerror(errno));
  return;
}

/**
 * @brief an implemetation of random select when multiple pipes and single pipes are to be read from.
 * 
 * @param numPipes number of pipes to be read.
 */
void PipeToSelect(int numPipes){
    Timeout.tv_usec = SAMPLING_PERIODE; 
    Timeout.tv_sec = 0; 
    fd_set fds;
    int tmp = 0;
    int maxfd;
    FD_ZERO(&fds); 
    for (int i = 0; i < numPipes; i++)
    {
        FD_SET(fd[i], &fds);

        if(fd[i]>maxfd){
            maxfd = fd[i];        
            }
    }
    select(maxfd + 1, &fds, NULL, NULL, &Timeout);
    for (int i = 0; i < numPipes; i++){
        choice[tmp] = FD_ISSET(fd[i], &fds);
        tmp++;
        
    }
    // We can optimize this by using linked list to save the index of the fd that are not 0 
}


/// @brief This function creates a log file of the process calling it.
///  
/// @param Fname the name of the Log file.
void CreateLog(char* Fname){
    //create/modify(if file exist) a file to write the pid of the main process 
    mkdir("Logs/",0666);
    char ff[20];
    // 
    sprintf(ff,"Logs/%s.log",Fname);

    if((LogFile = fopen(ff,"w") ) == NULL){
        perror("Error in Creating Log File ...");//you can redirect stderror to here..
        kill(ReadPID(MASTERF),SIGINT);
    } 
    PrintLog("Created Log File...\n");
}
/// @brief To get the current time.
/// @return the current time in micro seconds.
long GetTimeNow(){
    auto end = chrono::high_resolution_clock::now();
    return chrono::system_clock::to_time_t(end);
}

/**
 * @brief Get the Last time the Log file was Edited.
 * 
 * @param Fname name of the log file.
 * @return long returns the number of seconds since the log file was edited.
 */
long GetLastEdit(char* Fname){
    char* filename;
    sprintf(filename,"Logs/%s.log",Fname);
    struct stat result;
    if(stat(filename, &result)==0)
    {
        return result.st_mtime; 
    }
    return -1;
}
/**
 * @brief Print a log message to the log file.
 * 
 * @param fmt formated message.
 * @param ... parammeters to format the message.
 */
void PrintLog(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsprintf(Printable, fmt ,args);
    string Timestring = string("[%ld] ") + Printable;
    fprintf(LogFile,Timestring.c_str(),GetTimeNow());FLF
    va_end(args);
}

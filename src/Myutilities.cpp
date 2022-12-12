#define UTILS 2


#include "./../include/mu.h"



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
        perror("Failed to execute");
        exit(EXIT_FAILURE);
    }
}


void WritePID(char* Fname){ 
    //create/modify(if file exist) a file to write the pid of the main process 
    fprintf(LogFile,"[%ld] Saving the %s Process ID...\n",GetTimeNow(),Fname);
    mkdir("./.data/",0666);
    char ff[20];
    // 
    sprintf(ff,"./.data/%s.txt",Fname);
    
    FILE *f;
    if ((f = fopen(ff,"w"))==NULL)
    {
        perror("Error Writing PID");
        exit(-3);
    }
    

    int pid = getpid();
    fprintf(f,"%d",pid);
    //close file
    fclose(f);
}



int ReadPID(char* Fname){
    char ff[30];
    fprintf(LogFile,"[%ld] Reading PID of %s From %s...\n",GetTimeNow(),Fname,ProcessNAme);
    sprintf(ff,"./.data/%s.txt",Fname);
    FILE* f;
    if((f = fopen(ff,"r"))== NULL){
        kill(ReadPID(MASTERF),SIGKILL);
    }
    int pid;
    fscanf(f,"%d",&pid);
    //close file
    fclose(f);
    return pid;
}

void CreatePipes(int numPipes){
    PrintLog("Starting process %s,creating %d pipes\n",ProcessNAme,numPipes);fflush(stdout);
    PrintLog("Starting pipes..\r");
    for (int i = 0; i < numPipes; i++)
    {
        char filename[15];
        char lastfilename[20];
        int mode;
        
        sscanf(PipeFN[i],"%s %d",filename,&mode);
        sprintf(lastfilename,"/tmp/%s",filename);
        if((mkfifo(lastfilename, 0666)==-1) ){//check if it worked properly
            if (access(lastfilename,F_OK)==-1)
            {
                PrintLog("a problem in makefifo pipe \n");
                kill(ReadPID(MASTERF),SIGINT);
            }
        }
        if ((fd[i] = open(lastfilename,openmode[mode]))==-1)
        {
        
            PrintLog("Couldn't open the pipe\n");
            kill(ReadPID(MASTERF),SIGINT);
        }
        
       
    }
    PrintLog("Starting pipes..All set.\n");
}


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

InfoHandler* SignalWithInfo(int signum, InfoHandler* handler)
{
  struct sigaction action, old_action;

  memset(&action, 0, sizeof(struct sigaction));
  action.sa_sigaction = handler;  
  sigemptyset(&action.sa_mask); /* block sigs of type being handled */
  action.sa_flags = SA_RESTART|SA_SIGINFO; /* restart syscalls if possible */

  if (sigaction(signum, &action, &old_action) < 0)
            perror("Signal error");fflush(stdout);
  return (old_action.sa_sigaction);
}

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



void CreateLog(char* Fname){
    //create/modify(if file exist) a file to write the pid of the main process 
    mkdir("./.Logs/",0666);
    char ff[20];
    // 
    sprintf(ff,"./.Logs/%s.log",Fname);

    if((LogFile = fopen(ff,"w") ) == NULL){
        perror("Error in Creating Log File ...");//you can redirect stderror to here..
        kill(ReadPID(MASTERF),SIGINT);
    } 
    PrintLog("Created Log File...\n");
}
 
long GetTimeNow(){
    auto end = chrono::high_resolution_clock::now();
    return chrono::system_clock::to_time_t(end);
}

long GetLastEdit(char* Fname){
    char* filename;
    sprintf(filename,"./.Logs/%s.log",Fname);
    struct stat result;
    if(stat(filename, &result)==0)
    {
        return result.st_mtime; 
    }
    return -1;
}

void PrintLog(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsprintf(Printable, fmt ,args);
    string Timestring = string("[%ld] ") + Printable;
    fprintf(LogFile,Timestring.c_str(),GetTimeNow());FLF
    va_end(args);
}

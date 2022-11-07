#define UTILS 2


#include "mu.h"


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
    FILE *f = fopen(ff,"w");
    int pid = getpid();
    fprintf(f,"%d",pid);
    //close file
    fclose(f);
}


void updateGrid(){
    for (int i = 0; i < 25; i++)
    {
        for (int j = 0; j < 100; j++)
        {
            printf("%c",grid[i][j]);
        }
        printf("\n");fflush(stdout);
    }
    printf("\n\n");fflush(stdout);
}
void initGrid(){
    for (int i = 0; i < 25; i++)
    {
        for (int j = 0; j < 100; j++)
        {
            grid[i][j] = '.';
        }
    }
}
int ReadPID(char* Fname){
    char ff[30];
    fprintf(LogFile,"[%ld] Reading PID of %s From %s...\n",GetTimeNow(),Fname,ProcessNAme);
    sprintf(ff,"./.data/%s.txt",Fname);
    FILE *f = fopen(ff,"r");
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
        
        sscanf(filenames[i],"%s %d",filename,&mode);
        sprintf(lastfilename,"/tmp/%s",filename);
        mkfifo(lastfilename, 0666);
        fd[i] = open(lastfilename,openmode[mode]);
       
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
        sscanf(filenames[i],"%s %d",filename,&mode);

        sprintf(ff,"/tmp/%s",filename);
        printf("removing file %s\n",ff);
        remove(ff);
        close(fd[i]);
    }

    fclose(LogFile);
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

int* PipeToSelect(int numPipes){
    timeout.tv_usec = SAMPLING_PERIODE; 
    fd_set fds;
    int* ret = new int[numPipes];
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
    select(maxfd + 1, &fds, NULL, NULL, &timeout);
    for (int i = 0; i < numPipes; i++){
        ret[tmp] = FD_ISSET(fd[i], &fds);
        tmp++;
        
    }
  

    return ret;
    // We can optimize this by using linked list to save the index of the fd that are not 0 
}



void CreateLog(char* Fname){
    //create/modify(if file exist) a file to write the pid of the main process 
    mkdir("./.Logs/",0666);
    char ff[20];
    // 
    sprintf(ff,"./.Logs/%s.log",Fname);
    LogFile = fopen(ff,"w"); 
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


/*
long GetLastEdit(char* Fname){
    string filename = "./.Logs/M1.log";
    ifstream Logfile;
    Logfile.open(filename);
    Logfile.seekg(-2,ios_base::end);

    bool keepLooping = true;
    bool intermidiare = true;
    while(keepLooping) {
        char ch;
        Logfile.get(ch);                            
        if((int)Logfile.tellg() <= 1) {             
            Logfile.seekg(0);                       
            keepLooping = false;                
        }
        else if(ch == '\n') {                  
            keepLooping = false;                
        }
        else {                                  
            Logfile.seekg(-2,ios_base::cur);        
        }
    }

    string lastLine;            
    getline(Logfile,lastLine);                      // Read the current line
    long time;
    sscanf(lastLine.c_str(),"[%ld]",&time);
    cout << "Result: " << lastLine.c_str()<<" + " <<time << '\n'; fflush(stdout);    // Display it

    Logfile.close();

    return -1;
}


*/
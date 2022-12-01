#include <unistd.h>
#include <iostream>
#include <cstring>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <wait.h>
#include <fstream>
#include <sstream>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <chrono>
#include <math.h>


using namespace std;



/*                                       Start Macros                                     */
#define MS *1000
#define NM MS MS MS
#define V (char*)
#define NUM_PROC 5
#define SAMPLING_PERIODE 100 MS
#define BUFF_SIZE 8192
#define FLF fflush(LogFile);
#define CMDF V"command"
#define InspectF V"inspection"
#define MASTERF V"master"
#define M1F V"M1" 
#define M2F V"M2" 
#define RWF V"RW" 
/*                                       End Macros                                       */

/*                                       Data Struct                                      */
typedef void InfoHandler(int, siginfo_t *, void *);
struct Data{ float p[2] = {0.0f,0.0f};};

/*                                       End Data Struct                                  */




/*                                       Start Functions declaration                      */

InfoHandler* SignalWithInfo(int signum, InfoHandler* handler);
void WritePID(char* Fname);
void print(string p);
void Closing();
int spawn(char* comands,char* args[],int *pid);
int ReadPID(char* Fname);
void CreateLog(char* Fname);
long GetTimeNow();
void PrintLog(const char *fmt, ...);
long GetLastEdit(char* Fname);
void updateGrid();
void initGrid();
/*                                       End Functions declaration                        */




/*                                       Start Templates                                  */
template <class t>
void SendData(int fd,t *d){
    
    write(fd, d, sizeof(*d));
}
template <class t>
void ReceiveData(int fd,t *d){
    #ifdef M2 
    
    #endif
    read(fd, d, sizeof(*d)); 
}
/*                                      End Templates                                     */



/*                                        start                                           */

/*                                             UTILS                                      */
#ifndef UTILS
#include <iostream>
struct timeval Timeout;

#ifndef MASTER
void GarbgeCollection(int numFiles);
void CreatePipes(int numPioes);
void PipeToSelect(int numPipes);

#endif

#else
extern struct timeval Timeout;
extern int fd[];
extern char PipeFN[][20];
extern char* ProcessNAme;
extern int ReadyPipes[];
extern FILE *LogFile;
extern char Printable[100];
extern int choice[3];

int openmode[2] = {O_RDONLY,O_WRONLY};
#endif

/*                                             End UTILS                                  */


/*                                             MASTER                                      */
#ifdef MASTER 
#define FIRST_BACKGROUND_P 1
FILE *LogFile;
char Printable[100];
char* ProcessNAme = MASTERF;
int fd[0];
char* PipeFN[0];
char *CAA[NUM_PROC] = {CMDF,InspectF,M1F,M2F,RWF};//Commands And Arguments 
int signals[NUM_PROC];
char *args[4] ={V"konsole",V"-e",V"tmp" ,V NULL};
int children[NUM_PROC];
bool SIGFLAG = false;
int choice;
#endif
/*                                             End MASTER                                  */



/*                                             CMD                                       */

#ifdef CMD

#define NUM_PIPES 2
#define SAMPLING_PERIODE_KEYS 10 MS
#define REWIND_PERIODE 1 MS MS
FILE *LogFile;
char Printable[100];
char* ProcessNAme = CMDF;
int fd[NUM_PIPES];
bool nohitx = true;
bool nohity = true;
int choice[NUM_PIPES];
char PipeFN[NUM_PIPES][20] = {"CMD_m1 1","CMD_m2 1"};
float speed1 = 0.0f,speed2 = 0.0f;
#endif
/*                                             End CMD                                   */





/*                                             M1                                        */
#ifdef M1
FILE *LogFile;
char Printable[100];
#define NUM_PIPES 2 

char *ProcessNAme = M1F;
int fd[2];
char PipeFN[NUM_PIPES][20] = {"CMD_m1 0","m1_rw 1"};
int choice[NUM_PIPES];
#endif
/*                                             End M1                                    */



/*                                             M2                                        */
#ifdef M2

FILE *LogFile;
char Printable[100];
#define NUM_PIPES 2

char* ProcessNAme = M2F;
int fd[2];
char PipeFN[2][20] = {"CMD_m2 0","m2_rw 1"};
int choice[NUM_PIPES];
#endif
/*                                             End M2                                    */



/*                                             RW                                        */
#ifdef RW
FILE *LogFile;
char Printable[100];
#define NUM_PIPES 3
char* ProcessNAme = RWF;
int fd[3];
char PipeFN[3][20] = {"m1_rw 0","m2_rw 0","rw_in 1"};
int choice[NUM_PIPES];
#endif
/*                                             End RW                                    */


/*                                             Inspect                                   */
#ifdef Inspect
FILE *LogFile;
char Printable[100];
#define NUM_PIPES 1
char* ProcessNAme = InspectF;
int fd[1];
char PipeFN[1][20] = {"rw_in 0"};
char grid[25][100];
int choice[NUM_PIPES];
#endif
/*                                             End Inspect                                */

/*                                         End                                            */

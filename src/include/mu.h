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
/**
 * @brief Time conversions
 * 
 */
#define US_MS *1000
#define S_NS US_MS US_MS US_MS
#define US_S /1000000
/**
 * @brief Helper MACROS
 * 
 */
#define V (char*)
#define FLF fflush(LogFile);
/**
 * @brief General variables
 * 
 */
#define SAMPLING_PERIODE 100 US_MS
#define NUM_PROC 6

/**
 * @brief Process names
 * 
 */
#define CMDF V"command"
#define InspectF V"inspection"
#define MASTERF V"master"
#define M1F V"M1" 
#define M2F V"M2" 
#define RWF V"RW"
#define WatchDogF V"WatchDog" 

/*                                       End Macros                                       */

/*                                       Data Struct                                      */

typedef void InfoHandler(int, siginfo_t *, void *);
struct Data{ float p[2] = {0.0f,0.0f};};

/*                                       End Data Struct                                  */




/*                                       Start Functions declaration                      */

void SignalWithInfo(int signum, InfoHandler* handler);
void WritePID(char* Fname);
int ReadPID(char* Fname);

int spawn(char* comands,char* args[],int *pid);

void CreateLog(char* Fname);
long GetTimeNow();
void PrintLog(const char *fmt, ...);
long GetLastEdit(char* Fname);

/*                                       End Functions declaration                        */




/*                                       Start Templates                                  */
template <class t>
void SendData(int fd,t *d){
    write(fd, d, sizeof(*d));
}
template <class t>
void ReceiveData(int fd,t *d){
    read(fd, d, sizeof(*d)); 
}
/*                                      End Templates                                     */



/*                                        start                                           */

/*                                             UTILS                                      */
#ifndef UTILS
#include <iostream>
struct timeval Timeout;
char Printable[100];
FILE *LogFile;


void GarbgeCollection(int numFiles);
void CreatePipes(int numPioes);
void PipeToSelect(int numPipes);



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

#ifdef PROCESS_MANAGMENT

char *CAA[NUM_PROC] = {CMDF,InspectF,M1F,M2F,RWF,WatchDogF};//Commands And Arguments 
char *args[4] ={V"konsole",V"-e",V"tmp" ,V NULL};
int children[NUM_PROC];
#endif

/*                                             End UTILS                                  */


/*                                             WatchDog                                      */
#ifdef WatchDog 
#define FIRST_BACKGROUND_P 1
// WD_P is  WATCH DOG PERIOD
#define WD_P 15
// WD_T is  WATCH DOG TIMEOUT 
#define WD_T 60    

char* ProcessNAme = WatchDogF;

int fd[0];
char* PipeFN[0];

int choice;
#endif
/*                                             End WatchDog                                  */

/*                                             MASTER                                      */
#ifdef MASTER 
#define FIRST_BACKGROUND_P 1
#define NUM_PIPES 0

char* ProcessNAme = MASTERF;

int fd[NUM_PIPES];
char* PipeFN[NUM_PIPES];


pid_t wpid;
int status;

int choice;
#endif
/*                                             End MASTER                                  */



/*                                             CMD                                       */

#ifdef CMD

#define NUM_PIPES 3
#define SAMPLING_PERIODE_KEYS 10 US_MS
#define REWIND_PERIODE 1 US_MS US_MS
#define INCREMENT 1

// RS is Rewind SPEED
#define RS 2

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
#define NUM_PIPES 3 

char *ProcessNAme = M1F;

int fd[2];
char PipeFN[NUM_PIPES][20] = {"CMD_m1 0","m1_rw 1"};
int choice[NUM_PIPES];
#endif
/*                                             End M1                                    */



/*                                             M2                                        */
#ifdef M2

#define NUM_PIPES 3

char* ProcessNAme = M2F;
int fd[NUM_PIPES];
char PipeFN[NUM_PIPES][20] = {"CMD_m2 0","m2_rw 1"};
int choice[NUM_PIPES];

#endif
/*                                             End M2                                    */



/*                                             RW                                        */
#ifdef RW

#define NUM_PIPES 4
char* ProcessNAme = RWF;

int fd[NUM_PIPES];
char PipeFN[NUM_PIPES][20] = {"m1_rw 0","m2_rw 0","rw_in 1"};
int choice[NUM_PIPES];

#endif
/*                                             End RW                                    */


/*                                             Inspect                                   */
#ifdef Inspect
// WR is WIDTHRATIO of the Grid
#define WR 0.39
// HR is HEIGHT RATIO of the Grid 
#define HR 0.09

#define NUM_PIPES 2

char* ProcessNAme = InspectF;

int fd[NUM_PIPES];
char PipeFN[NUM_PIPES][20] = {"rw_in 0"};
int choice[NUM_PIPES];
#endif
/*                                             End Inspect                                */

/*                                         End                                            */

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

using namespace std;



/*                                       Start Macros                                     */
#define MS *1000
#define NM MS MS MS
#define V (char*)
#define NUM_PROC 5
#define SAMPLING_PERIODE 100 MS
#define BUFF_SIZE 8192
#define FLF fflush(LogFile);
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
#include <conio.h>
#include <iostream>
struct timeval timeout;

#ifndef MAIN
void GarbgeCollection(int numFiles);
void CreatePipes(int numPioes);
int* PipeToSelect(int numPipes);
#endif

#else
extern struct timeval timeout;
extern int fd[];
extern char filenames[][20];
extern char* ProcessNAme;
extern int ReadyPipes[];
extern FILE *LogFile;
extern char Printable[100];

int openmode[2] = {O_RDONLY,O_WRONLY};
#endif

/*                                             End UTILS                                  */


/*                                             MAIN                                      */
#ifdef MAIN 
#define FIRST_BACKGROUND_P 1
FILE *LogFile;
char Printable[100];
char* ProcessNAme = V"main";
int fd[0];
char* filenames[0];
char *CAA[NUM_PROC] = {V"cmd",V"inspect",V"M1",V"M2",V"RW"};//Commands And Arguments 
int signals[NUM_PROC];
char *args[4] ={V"konsole",V"-e",V"tmp" ,V NULL};
int children[NUM_PROC];
#endif
/*                                             End MAIN                                  */



/*                                             CMD                                       */

#ifdef CMD

#define NUM_PIPES 2
#define SAMPLING_PERIODE_KEYS 10 MS
#define REWIND_PERIODE 1 MS MS
FILE *LogFile;
char Printable[100];
char* ProcessNAme = V"cmd";
int fd[NUM_PIPES];
bool nohitx = true;
bool nohity = true;
char filenames[NUM_PIPES][20] = {"CMD_m1 1","CMD_m2 1"};
float speed1 = 0.0f,speed2 = 0.0f;
#endif
/*                                             End CMD                                   */





/*                                             M1                                        */
#ifdef M1
FILE *LogFile;
char Printable[100];
#define NUM_PIPES 2 

char *ProcessNAme = V"M1";
int fd[2];
char filenames[2][20] = {"CMD_m1 0","m1_rw 1"};

#endif
/*                                             End M1                                    */



/*                                             M2                                        */
#ifdef M2

FILE *LogFile;
char Printable[100];
#define NUM_PIPES 2

char* ProcessNAme = V"M2";
int fd[2];
char filenames[2][20] = {"CMD_m2 0","m2_rw 1"};
#endif
/*                                             End M2                                    */



/*                                             RW                                        */
#ifdef RW
FILE *LogFile;
char Printable[100];
#define NUM_PIPES 3
char* ProcessNAme = V"RW";
int fd[3];
char filenames[3][20] = {"m1_rw 0","m2_rw 0","rw_in 1"};
#endif
/*                                             End RW                                    */


/*                                             Inspect                                   */
#ifdef Inspect
FILE *LogFile;
char Printable[100];
#define NUM_PIPES 1
char* ProcessNAme = V"inspect";
int fd[1];
char filenames[1][20] = {"rw_in 0"};
#endif
/*                                             End Inspect                                */

/*                                         End                                            */

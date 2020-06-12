/*------------------------------INCLUDES-------------------------------------*/
#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h> /* DIR* */
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/file.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h> 
#include <sys/time.h>

/*---------------------------END OF INCLUDES----------------------------------*/



/*-------------------------------GLOBALS--------------------------------------*/


/*----------------------------END OF GLOBALS----------------------------------*/

int opened = 1;

/*-------------------------FUNCTION PROTOTYPES--------------------------------*/

void thread_handler(union sigval sv);
void signalHandler(int s);
/*----------------------END OF FUNCTION PROTOTYPES----------------------------*/
        



/*-------------------------------MAIN-----------------------------------------*/
int main(int argc,char ** argv){
srand(time(NULL)); 
    signal(SIGINT,&signalHandler);

    if(argc != 2){
        printf("Error: Usage is ./Bank <#secs>\n");
        exit(1);
    }
    

  char info[] = "5 seconds elapsed.";
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec trigger;

struct stat buffer;
    int exist = stat("Banka.log",&buffer);
    if(exist == 0){
        remove("Banka.log");
    }


    FILE * fp;
    fp = fopen ("Banka.log","w");
    fprintf(fp,"clientPid\t processNo\t Para\t islem bitis zamanı\n****\t**************\t******\t*********************\n");
    fclose(fp);


    memset(&sev, 0, sizeof(struct sigevent));
    memset(&trigger, 0, sizeof(struct itimerspec));



    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = &thread_handler;
    sev.sigev_value.sival_ptr = &info;

   timer_create(CLOCK_REALTIME, &sev, &timerid);


    int slope = 0;
    slope = atoi(argv[1]);

    trigger.it_value.tv_sec = slope;
    timer_settime(timerid, 0, &trigger, NULL);




 




    pid_t mainChild = getpid();

    char qqq[4][16];
    char garbage[1024][16];
    int garb = 0;
    for(int i = 0 ; i<4; ++i){
        strcpy(qqq[i],"a");
    }
    for(int i = 0 ; i<64; ++i){
        strcpy(garbage[i],"a");
    }

    int hist = 0;

	int fd1; int job = 0; int totalJob = 0; int doneJob = 0;
    // FIFO file path 
    char * myfifo = "/tmp/myfifo"; 

    // Creating the named file(FIFO) 
    // mkfifo(<pathname>,<permission>) 
    mkfifo(myfifo, 0666); 
    fd1 = open(myfifo,O_RDONLY | O_NONBLOCK); 
    
    
    int bankAtm[4][2];
    int atmBank[4][2];
    int jobHistory[4];

    for(int i=0; i<4; ++i){
        jobHistory[i]= 0;
    }
    int myid;

    for(int i=0; i<4; ++i){

        if(mainChild != 0){
            pipe(bankAtm[i]);
            pipe(atmBank[i]);
            fcntl(atmBank[i][0], F_SETFL, O_NONBLOCK);

            mainChild = fork();
            if(mainChild == 0){
                myid = i;
                close(bankAtm[i][1]); /* close pipe reading side */
                close(atmBank[i][0]); 
               
            }
            else{
                close(bankAtm[i][0]); 
                close(atmBank[i][1]); /* close pipe reading side */

            }
        }

    }
        int que = 0;





struct timespec start, end;
clock_gettime(CLOCK_MONOTONIC_RAW, &start);


    while (opened == 1){
        int pidG = -1;

        if(mainChild > 0){
            

                int serbest[4];

                for(int i=0; i<4; ++i){
                    int y = 0;
                    int t =read(atmBank[i][0],&y,sizeof(y)); /*write to pipe */
                    if( t != -1 && t!= 0){

                        if(y == 0){
                            serbest[i] = 1;
                        }else{

                            serbest[i] = 1;
                            --que;
                            ++jobHistory[i];
                            ++doneJob;
                            
                            int ppid = 1;
                            read(atmBank[i][0],&ppid,sizeof(ppid));

                           
                            

                            clock_gettime(CLOCK_MONOTONIC_RAW, &end);

                            u_int64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
                            int sure = delta_us /1000;
                            fp = fopen ("Banka.log","a");
                            fprintf(fp,"%d\t process%d\t \t%d\t \t%d msec\n",ppid,i,y,sure );
                            fclose(fp);

                        }
                    }
                }

                int t = 0;
                if(job == 0 && que <4){
                    t = read(fd1, &pidG, sizeof(pidG));
                }  
                   
                    if( pidG != -1  && t != 0){
                        
                        ++que;
                        ++job;
                        ++totalJob;
                    
                        ++hist;


                    }

                    for(int i=0; i<4; ++i){
                        if(serbest[i] == 1 && job >0){
                            --job;
                            serbest[i] = -1;
                            char pig[16];
                            sprintf(pig,"%d",pidG);
                            
                        
                            strcpy(garbage[garb],pig);
                            ++garb;
                        
                            write(bankAtm[i][1],&pidG,sizeof(pidG)); /*write to pipe */
                            break;
                        }
                    }
                
        }
        int flag = 0;


        if(mainChild == 0){
    
            if(myid == 0){
                int x = 1+rand()%100;
                int pid = 0;

                write(atmBank[0][1],&flag,sizeof(flag)); /*write to pipe */
                read(bankAtm[0][0],&pid,sizeof(pid)); /*write to pipe */
                if(pid == -1 ){
                    exit(0);
                }
                if(pid != 0){

                time_t secs =2;time_t startTime = time(NULL);
                while (time(NULL) - startTime < secs);
            

                char cfifo[16];
                sprintf(cfifo,"/tmp/%d",pid);
              
                int fd2 = open(cfifo, O_WRONLY); 
                write(fd2, &x,sizeof(x)); 
                close(fd2);
                write(atmBank[0][1],&x,sizeof(x)); /*write to pipe */
                write(atmBank[0][1],&pid,sizeof(pid)); /*write to pipe */
                }

                
            }

            if(myid == 1){
                int x = 1+rand()%100;
                
                int pid = 0;

                write(atmBank[1][1],&flag,sizeof(flag)); /*write to pipe */
                read(bankAtm[1][0],&pid,sizeof(pid)); /*write to pipe */
                if(pid == -1 ){
                    exit(0);
                }
                if(pid != 0){

                time_t secs =2; 
                time_t startTime = time(NULL);
                while (time(NULL) - startTime < secs);
      

                char cfifo[16];
                sprintf(cfifo,"/tmp/%d",pid);
               

                int fd2 = open(cfifo, O_WRONLY); 
                write(fd2, &x,sizeof(x)); 
                close(fd2);
                write(atmBank[1][1],&x,sizeof(x)); /*write to pipe */
                write(atmBank[1][1],&pid,sizeof(pid)); /*write to pipe */
                }
            }

            if(myid == 2){
                int x = 1+rand()%100;
                int pid = 0;


                write(atmBank[2][1],&flag,sizeof(flag)); /*write to pipe */ 
                read(bankAtm[2][0],&pid,sizeof(pid)); /*write to pipe */
                if(pid == -1 ){
                    exit(0);
                }
                if(pid != 0){

                time_t secs = 2; time_t startTime = time(NULL);
                while (time(NULL) - startTime < secs);
            

                char cfifo[16];
                sprintf(cfifo,"/tmp/%d",pid);
              

                int fd2 = open(cfifo, O_WRONLY); 
                write(fd2, &x,sizeof(x)); 
                close(fd2);
                write(atmBank[2][1],&x,sizeof(x)); /*write to pipe */
                write(atmBank[2][1],&pid,sizeof(pid)); /*write to pipe */
                }

            }
            if(myid == 3){
                int x = 1+rand()%100;
                int pid = 0;


                write(atmBank[3][1],&flag,sizeof(flag)); /*write to pipe */
                read(bankAtm[3][0],&pid,sizeof(pid)); /*write to pipe */
                if(pid == -1 ){
                    exit(0);
                }
                if(pid != 0){

                time_t secs =2; time_t startTime = time(NULL);
                while (time(NULL) - startTime < secs);
              
                char cfifo[16];
                sprintf(cfifo,"/tmp/%d",pid);

              

                int fd2 = open(cfifo, O_WRONLY); 
                write(fd2, &x,sizeof(x)); 
                close(fd2);
                write(atmBank[3][1],&x,sizeof(x)); /*write to pipe */
                write(atmBank[3][1],&pid,sizeof(pid)); /*write to pipe */
                
            }
            }
        
        }

    

    }





   


    if(mainChild > 0){
        fp = fopen ("Banka.log","a");
        fprintf(fp,"\n");
        fclose(fp); 
        for(int i=0; i<4; ++i){

            int bye = -1;
                fcntl(bankAtm[i][1], F_SETFL, O_NONBLOCK);
                
                int ttt = write(bankAtm[i][1],&bye,sizeof(bye)); /*write to pipe */
                if( ttt ==-1){
                }

            fp = fopen ("Banka.log","a");
            fprintf(fp,"process%d %d hizmet etti\n",i, jobHistory[i]);
            fclose(fp); 
            
      
        }
    }



    if(mainChild == 0){
        exit(0);
    }

    while (wait(0)>0);





    
    for(int i=0; i<garb; ++i){
        if(strcmp(garbage[i],"a") == 0){

        }else{
            int sorry = -1;
            char closer[64];
            sprintf(closer,"/tmp/%s",garbage[i]);
            int finishFd = open(closer, O_WRONLY); 
            int cuk = write(finishFd, &sorry,sizeof(sorry)); 
            if(cuk >0){
                ++totalJob;
            }

            close(finishFd);
        }
    }

   
    int wh = 1;
    while(wh == 1){
        int nonfinish = 0;
        int finisher = read(fd1, &nonfinish, sizeof(nonfinish));
        if(finisher == 0 || finisher == -1){
            wh = 0;
        }else{

            int sorry = -1;
            ++totalJob;
            char cfifo[16];
            sprintf(cfifo,"/tmp/%d",nonfinish);
            int finishFd = open(cfifo, O_WRONLY); 
            write(finishFd, &sorry,sizeof(sorry)); 
            close(finishFd);
            
            
        }


    }



    close(fd1); 
    int hizmet = 0;
    for(int i =0; i<4; ++i){
        hizmet += jobHistory[i];
    }



    fp = fopen ("Banka.log","a");
    fprintf(fp,"%d kisiye hizmet verildi \n",hizmet );
    fclose(fp); 

    printf("BANKA BYE BYE\n");
   
    return 0; 
}
/*----------------------------END OF MAIN-------------------------------------*/


void signalHandler(int s){   

    if (s == SIGINT) {
        printf("Banka imha edildi.\n");
        exit(1);
    }
    
}


void thread_handler(union sigval sv) {
       opened = 0;
}



/*------------------------------INCLUDES-------------------------------------*/
#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h> /* DIR* */
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

#include <sys/file.h>
#include <fcntl.h>


/*---------------------------END OF INCLUDES----------------------------------*/



/*-------------------------------GLOBALS--------------------------------------*/


/*----------------------------END OF GLOBALS----------------------------------*/




/*-------------------------FUNCTION PROTOTYPES--------------------------------*/



/*----------------------END OF FUNCTION PROTOTYPES----------------------------*/




/*-------------------------------MAIN-----------------------------------------*/
int main(int argc,char ** argv){
	pid_t mainChild = getpid();
    if(argc != 2){
        printf("Error: Usage is ./Client <#childNumber>\n");
        exit(1);
    }
    int fdM,fdC;
    char * myfifo = "/tmp/myfifo"; 
    mkfifo(myfifo, 0666); 
    char pidFifo[16];
    int money = -1;
    int childNum = 0;

    childNum = atoi(argv[1]);


    fdM = open(myfifo, O_WRONLY); 
    for(int i=0; i<childNum; ++i){
        if(mainChild != 0){
            mainChild = fork(); 
        }
    }
        
    if(mainChild == 0){
        int spid = getpid();
        int t = write(fdM, &spid, sizeof(spid)); 
        printf("t: %d -- fdM: %d -- pid: %d\n",t,fdM,getpid() );
        sprintf(pidFifo,"/tmp/%d",spid);


        mkfifo(pidFifo,0666);
        fdC = open(pidFifo, O_RDONLY); 
        read(fdC, &money, sizeof(money)); 
        if( money == -1){
        printf("Musteri %d parasini alamadi :(\n",getpid()); 

        }else{
        printf("Musteri %d %d lira aldi :)\n",getpid(),money); 

        }


        close(fdC); 

        unlink(pidFifo);

    }
    if(mainChild>0){
        while(wait(0)>0);
        close(fdM); 

        
    return 0; 
    }
} 
/*----------------------------END OF MAIN-------------------------------------*/

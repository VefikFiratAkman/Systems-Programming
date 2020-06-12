/*###########################################################################*/
/*CSE344 SYSTEM PROGRAMMING HOMEWORK 4                                       */
/*________________________________                                           */
/*Written by Vefik Fırat Akman on MARCH,31 2019                               */
/*                                                                           */
/*USAGE: ./buNeDuFPF [Option] Path                                 		     */
/*                                                                           */
/*This program is rewritten "du" command.                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/


/*------------------------------INCLUDES-------------------------------------*/
#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h> /* DIR* */
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <fcntl.h>

/*---------------------------END OF INCLUDES----------------------------------*/



/*-------------------------------GLOBALS--------------------------------------*/
int argFlag = 0;
char * myfifo = "/tmp/151044031sizes"; 
/*----------------------------END OF GLOBALS----------------------------------*/


typedef enum {FALSE = 0, TRUE = 1} bool;


/*-------------------------FUNCTION PROTOTYPES--------------------------------*/
int depthFirstApply(char *path,int (pathFun)(char *path1));
int sizepathfun (char *path);
void printError(int type);
bool isDirectory(const char * directoryName);
void printer(char * mainPath,int size,int pid);
int specialCheck(char * dName);
void childs(int * pid,int size);
void removeFile();
/*----------------------END OF FUNCTION PROTOTYPES----------------------------*/



/*-------------------------------MAIN-----------------------------------------*/
int main(int argc,char ** argv){
 
    removeFile();
    pid_t mainChild;
    int fd;
    int mkres = mkfifo(myfifo,0666); /* make a fifo */
    if(mkres == -1){ /* check fifo created success or not */
    	printf("MKFIFO RETURNED -1 \n");
    	return -1;
    }
   	DIR *dirr = NULL;

  
    /* This if/else block check command line arguments */
        if(argc == 2){
	    	if (!(dirr = opendir(argv[1]))){ /* try open given path */
		        printError(2); /* if path is uncorrect call error */
		        unlink(myfifo); /* delete fifo */
		        return -1; /* Return -1 because of error */ 
	    	}
	    	else{
	    			closedir(dirr); /* close directory */
	    		}
        }

        else if(argc == 3){
            if(strcmp(argv[1],"-z")== 0){ /* check option is correct or not */
                argFlag = 1;
                if (!(dirr = opendir(argv[2]))){ /* try open given path */
			        printError(2); /* if path is uncorrect call error */
		        	unlink(myfifo); /* delete fifo */
			        return -1; /* Return -1 because of error */ 
	    		}
	    		else{
	    			closedir(dirr); /* close directory */
	    		}
            }
           
            else{
                printError(1); /* calling error message */
                return -1;
            }
        }

        else{
            printError(1); /* calling error message */
            return -1;

        }

    mainChild = fork(); /* fork child for task */
    if(mainChild == -1){
        printf("ERROR MAIN FORK\n");
        return -1;
    }
   

 	if(mainChild > 0){ /* only main process do this */
		fd = open(myfifo,O_RDONLY);
		if(fd == -1){
			printf("FIFO CANT OPEN 1\n");
		}  
    }
    if(mainChild == 0){
      
    	/* This if/else block check command line arguments */
        if(argFlag == 0){
            depthFirstApply(argv[1],sizepathfun);
        }
        else {
            depthFirstApply(argv[2],sizepathfun);
        }


        exit(1);
    }


    while (wait(0)>0); /*waiting child */
    
    char  buf[1024];
    printf("PID\t\tSIZE\t\tPATH\n");
    int p = 0;
    int pid[1024];

    /* read from  fifo */
    while(read(fd,buf,sizeof(buf))){
   		printf("%s",buf );
	    char str[1024];
	    sscanf(buf, "%s", str); /*take pid */
	    pid[p] = atoi(str);
	    ++p;
    }
    childs(pid,p); /*calculating childs number */
    unlink(myfifo); /* delete fifo */


    return 0;
}
/*----------------------------END OF MAIN-------------------------------------*/



/*-----------------------FUNCTION IMPLEMENTATION------------------------------*/

/* This function search files and directories in given path. When fuction 
found file check is reguler file or not. If is regular file function take size
of file and calculate directiroies size two different style.*/

int depthFirstApply(char *path,int (pathFun)(char *path1)){
    DIR *dir = NULL; /* Directory pointer */
    struct dirent *entry;
  	pid_t childPidIn;
    char newpath[4096];  /* Make allocation for given path */
    char *innerpath = (char*)malloc(sizeof(char)*PATH_MAX); /* Make allocation for given path */
    char *mainPath = (char*)malloc(sizeof(char)*PATH_MAX); /* Make allocation for given path */
  	

    strcpy(newpath,path); /* Take copy of given path. Use for add new path on path */
    strcpy(mainPath,path);/* Take copy of given path. Use for when need givenpath again. */


    int total = 0; /* size of directories */
     

    int mypipe[2]; /* integer array for pipe */


    if (!(dir = opendir(path))){ /* try open given path */
        printError(2); /* if path is uncorrect call error */
        free(innerpath);
        free(mainPath);
        closedir(dir);
        return -1; /* Return -1 because of error */ 
    }

    while ((entry = readdir(dir)) != NULL) { /* check opened directory is null or not */
        
        if (entry->d_type == DT_DIR) { /* Check file type: directory or not. */
           
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){ 
            	/*we dont want calling himself. It's make infinity loops so we checked this */
                continue;
            }

            if(argFlag == 1){
            pipe(mypipe);
            }
            childPidIn = fork();  /* fork for enter new directory */
            if(childPidIn == -1){
            	printf("CHILD FORK ERROR \n");
            }
            
            if (childPidIn == 0) { /* just child process can enter in tihs if */

                sprintf(newpath,"%s/%s",path, entry->d_name); /* creating newpath */

                free(innerpath); /*free allocated location */
                free(mainPath); /*free allocated location */
                closedir(dir); /* close opened file */
                int add = 0; 
                add += depthFirstApply(newpath, pathFun); /* recursie call */
                if(argFlag == 1){
                    char * ss = (char*)malloc(sizeof(char)*PATH_MAX);
                    sprintf (ss,"%d",add);
                    close(mypipe[0]); /* close pipe reading side */
                    write(mypipe[1],ss,strlen(ss)); /*write to pipe */
                    free(ss);
                }
                exit(1); /* exit for child process */


            }
            
            
        } else {

                sprintf(innerpath, "%s/%s",newpath, entry->d_name); /* we take file path */
                
              

                if(specialCheck(innerpath) != -1 ){ /* check file special or not */
                    total += pathFun(innerpath); /* we adding file size to total. */
                  
                }
               
                else{
                    printer(entry->d_name,-1,getpid());
                    
                }
 
        }

      
    }


    while (wait(0)>0); /* wait for child process */
                    
    
                   
    if(argFlag == 1){
	    close(mypipe[1]); /*close pipe writing side */
		char * ff = (char*)malloc(sizeof(char)*PATH_MAX);
	    read(mypipe[0],ff,sizeof(ff)); /* read from pipe */
	    total += atoi(ff); /* take size from string which reading in pipe */
	    free(ff);
    }

    printer(newpath,total,getpid()); /* print data to txt file */
    /* Buraya Fifo gelecek */




    free(innerpath);/* in the beginning, we made allocation for this variable
    now we need free this allocated locaiton for avoid memory leak */
    free(mainPath);/* in the beginning, we made allocation for this variable
    now we need free this allocated locaiton for avoid memory leak */
    closedir(dir); /* close opened file */

  
   
	return total; /* return total size */
    
}

/* This function take stats of given file. And return this file size in KB*/
int sizepathfun (char *path){
    struct stat stats;
    if(lstat(path,&stats) == -1){ /*  try open file stats */
        return 0;
    }
    else {
        return (stats.st_size/1024); /* return file stats in KB */
    }
}

/*This function check given file is (reguler file or directory) or not */
int specialCheck(char * dName){
    struct stat stats;
    lstat(dName,&stats); /* open stats of file */

 	/*check given file is (reguler file or directory) or not */
     if(S_ISREG(stats.st_mode) == 0 && S_ISDIR(stats.st_mode) == 0){ 

        return -1;
     }
     else{
   
        return 1;
     }   
}
/* this function check given file is directory or not */
bool isDirectory(const char * directoryName){

    struct stat statbuffer;

    if(stat(directoryName,&statbuffer) == -1){
      return FALSE;

    }
    else {
      return S_ISDIR(statbuffer.st_mode) ? TRUE : FALSE;
    }

  }
/* This function using for giving error message when  user mistake's in command line or cant open file */
void printError(int type){

    if(type == 0){
        printf("Error 0: Exe just have \"-z\" option. If you wanna the size of the directory contains the sizes of all subtrees that the directory contains write this option or empty it. \n");
    }
    else if(type == 1){
        printf("Error 1: Please follow the rules of use. You have 2 option:\ni)\"Exe Path\"\nii)\"Exe -z Path\" \n");
    }
    else if(type == 2){
        printf("Path is incorrect. Please try again.\n");
    }
}



/*This function write data to Fifo */
void printer(char * mainPath,int size,int pid){

    
    int fd;
    fd = open(myfifo,O_WRONLY);
      if(fd == -1){
            printf("FIFO CANT OPEN 2\n");
        }  

   /* write 10 lines of text into the file stream*/
    if(size != -1){        
        //printf ("%d\t\t%d\t\t%s\n",pid,size,mainPath);
        char buf[1024];
        sprintf(buf,"%d\t\t%d\t\t%s\n",pid,size,mainPath);
        write(fd,buf,sizeof(buf));
    }
    else{
        //printf ("%d\t\t-\t\t%s\n",pid,mainPath);
        char buf[1024];
        sprintf(buf,"%d\t\t-1\t\t%s\n",pid,mainPath);
        write(fd,buf,sizeof(buf));

    }
    /* close the file*/ 

    


}


/*This function calculating childs number and give main process pid */
void childs(int * pid,int size){
	int k,i;	

    /* calculating how many child created */
    for(k=0; k<size; ++k){
        for(i=0; i <size; ++i){
            if(pid[i]>pid[k]){
                int temp = pid[i];
                pid[i] = pid[k];
                pid[k] = temp;
            }
        }
    }
    /* append child count and main process pid end of file */
    printf("%d child processes created. Main process is %d.\n",pid[size-1]-pid[0]+1,getpid());

}



/* This function remove existing file */
void removeFile(){
    struct stat buffer;
    int exist = stat(myfifo,&buffer);
    if(exist == 0){
        remove(myfifo);
    }
}

/*-------------------END OF FUNCTION IMPLEMENTATION---------------------------*/

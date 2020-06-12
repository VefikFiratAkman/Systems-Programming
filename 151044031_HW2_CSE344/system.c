/*###########################################################################*/
/*CSE344 SYSTEM PROGRAMMING HOMEWORK 1                                       */
/*________________________________                                           */
/*Written by Vefik Fırat Akman on MARCH,9 2019                               */
/*                                                                           */
/*USAGE: ./buNeDu [Option] Path                                 		     */
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
/*----------------------------END OF GLOBALS----------------------------------*/


typedef enum {FALSE = 0, TRUE = 1} bool;


/*-------------------------FUNCTION PROTOTYPES--------------------------------*/
int depthFirstApply(char *path,int (pathFun)(char *path1));
int sizepathfun (char *path);
void printError(int type);
bool isDirectory(const char * directoryName);
void printfile(char * mainPath,int size,int pid);
int specialCheck(char * dName);
void creatingFile();
int readFile();
void option();
void endFile(int childs);
int acquireLock (char *fileSpec);
void releaseLock (int lockFd);
void removeFile();
/*----------------------END OF FUNCTION PROTOTYPES----------------------------*/



/*-------------------------------MAIN-----------------------------------------*/
int main(int argc,char ** argv){

    pid_t mainChild;
    

    /* This if/else block check command line arguments */
        if(argc == 2){
               creatingFile();
        }

        else if(argc == 3){
            if(strcmp(argv[1],"-z")== 0){ /* check option is correct or not */
                argFlag = 1;
                creatingFile();
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
    
    option();  /* editing txt file */
    readFile(); /* read from file */

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


            childPidIn = fork();  /* fork for enter new directory */
           
            if (childPidIn == 0) { /* just child process can enter in tihs if */

                sprintf(newpath,"%s/%s",path, entry->d_name); /* creating newpath */

                free(innerpath); /*free allocated location */
                free(mainPath); /*free allocated location */
                closedir(dir); /* close opened file */

                depthFirstApply(newpath, pathFun); /* recursie call */
                
                exit(1); /* exit for child process */


            }
            
            
        } else {
               
                sprintf(innerpath, "%s/%s",newpath, entry->d_name); /* we take file path */
        
                if(specialCheck(innerpath) != -1 ){ /* check file special or not */
                    total += pathFun(innerpath); /* we adding file size to total. */
                }
               
                else{
                    printfile(entry->d_name,-1,getpid());
                }
 
        }

      
    }

    

    while (wait(0)>0); /* wait for child process */
     
    printfile(newpath,total,getpid()); /* print data to txt file */

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


/* This function reading data from file and print to screen */
int readFile(){
    FILE *fp;
    char * str = (char*)malloc(sizeof(char)*PATH_MAX); /* Make allocation for given path */
    
    char* filename = "151044031sizes.txt";
 
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    }
    while (fgets(str, 1000, fp) != NULL)
        printf("%s", str);
    free(str);
    fclose(fp);
    return 0;
}

/*This function write data to file. And use file locking system */
void printfile(char * mainPath,int size,int pid){

    FILE * fp;
    fp = fopen ("151044031sizes.txt","a");

    int fd;
    if ((fd = acquireLock ("151044031sizes.txt")) < 0) {
        fprintf (stderr, "Cannot get lock file.\n");
    }   /* we locked file */

 
   /* write 10 lines of text into the file stream*/
    if(size != -1){
        fprintf (fp, "%d\t\t%d\t\t%s\n",pid,size,mainPath);
    }
    else{
        fprintf (fp, "%d\t\t-\t\t%s\n",pid,mainPath);

    }
   /* close the file*/ 

   releaseLock (fd);  /* We unlocked file */
   fclose (fp);

}

/* https://stackoverflow.com/questions/6182877/file-locks-for-linux taken from here */
/*This function for locking file */
int acquireLock (char *fileSpec) {
    int lockFd;

    if ((lockFd = open (fileSpec, O_CREAT | O_RDWR, 0666))  < 0) 
        return -1;

    if (flock (lockFd, LOCK_EX | LOCK_NB) < 0) {
        close (lockFd);
        return -1;
    }

    return lockFd;
}

/* https://stackoverflow.com/questions/6182877/file-locks-for-linux taken from here */
/*This function for unlocking file */
void releaseLock (int lockFd) {
    flock (lockFd, LOCK_UN);
    close (lockFd);
}


/*This function for editing txt file after all child writing all data in txt
If -z option active this function take txt and parse them. After calculate data for
-z option. And writing new data in txt file. */
void option(){
    FILE *fp;
    int pid[1024];
    int size[1024];
    int i = 0;
    

    char * str = (char*)malloc(sizeof(char)*PATH_MAX); /* Make allocation for given path */

    char **path = malloc(PATH_MAX * sizeof(char *)); /* Allocate row pointers*/
    for(i = 0; i < PATH_MAX; i++){
      path[i] = malloc(PATH_MAX * sizeof(char));  /* Allocate each row separately*/
    }

    i = 0;
    int p = 0;
    int s = 0;
    int st = 0;

    fp = fopen("151044031sizes.txt", "r");

    fgets(str, 1000, fp); /* jump to Heads */

    /* This file take all data in three diffrenet column*/
    while (fscanf(fp, "%s", str) == 1) {
        if (i%3==0)
        {
            pid[p] = atoi(str);
            ++p;
        }
        else if (i%3==1)
        {
            if(strcmp(str,"-") == 0){
                size[s] = 0;
            }
            else{
            size[s] = atoi(str);

            }

            ++s;
        }
        else if (i%3==2)
        {
            strcpy(path[st],str);
            ++st;
        }
        ++i;
    }

    int k,a;

    /* if -z option is active first calculate size 
    second create new file with new data */
    if(argFlag == 1){
        /* calculate new size */
        for(k=1; k<p; ++k){
            for(a=0; a <p; ++a){
                if(a != (p-k)){
                    if(strstr(path[a], path[p-k]) != NULL) {
                        size[p-k] += size[a];
                    }
                }
            }
        }
        /* create new file with new data */
        creatingFile();
        for(a=0; a <p; ++a){
            printfile(path[a],size[a],pid[a]);
            
        }

    }

    /* calculating how many child created */
    for(k=0; k<p; ++k){
        for(a=0; a <p; ++a){
            if(pid[a]>pid[k]){
                int temp = pid[a];
                pid[a] = pid[k];
                pid[k] = temp;
            }
        }
    }
    
    /* append child count and main process pid end of file */
    endFile((pid[p-1]-pid[0]+1));


    for(i = 0; i < PATH_MAX; i++){
      free(path[i]);  
    }
    /* free allocated memory */
    free(path); 
    free(str);

    fclose(fp);
}

/*This fuction append child count and main process pid end of file */
void endFile(int childs){

 FILE * fp;
    fp = fopen ("151044031sizes.txt","a");
    fprintf(fp,"%d child processes created. Main process is %d.\n",childs,getpid());
    fclose(fp);
}

/* this function create new file with headers */
void creatingFile(){
    removeFile();
    FILE * fp;
    fp = fopen ("151044031sizes.txt","w");
    fprintf(fp,"PID \t\tSIZE \t\tPATH\n");
    fclose(fp);
}

/* This function remove existing file */
void removeFile(){
    struct stat buffer;
    int exist = stat("151044031sizes.txt",&buffer);
    if(exist == 0){
        remove("151044031sizes.txt");
    }
}

/*-------------------END OF FUNCTION IMPLEMENTATION---------------------------*/

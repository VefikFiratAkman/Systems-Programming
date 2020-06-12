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
/*---------------------------END OF INCLUDES----------------------------------*/



/*-------------------------------GLOBALS--------------------------------------*/
int argFlag = 0;
/*----------------------------END OF GLOBALS----------------------------------*/




/*-------------------------FUNCTION PROTOTYPES--------------------------------*/
int depthFirstApply(char *path,int (pathFun)(char *path1));
int sizepathfun (char *path);
void printError(int type);
int specialCheck(char * dName);
/*----------------------END OF FUNCTION PROTOTYPES----------------------------*/



/*-------------------------------MAIN-----------------------------------------*/
int main(int argc,char ** argv){

	/* This if/else block check command line arguments */
    
 if(argc == 2){
        depthFirstApply(argv[1],sizepathfun);
    }else if(argc == 3){
        if(strcmp(argv[1],"-z")== 0){ /* check option is correct or not */
            argFlag = 1;
            depthFirstApply(argv[2],sizepathfun);
        }
        else if(strcmp(argv[1],"<")== 0){ /* check option is correct or not */


        char const* const fileName = argv[2]; /* should check that argc > 1 */
        FILE* file = fopen(fileName, "r"); /* should check the result */
        char line[1024];

        while (fgets(line, sizeof(line), file)) {
            /* note that fgets don't strip the terminating \n, checking its
               presence would allow to handle lines longer that sizeof(line) */
        }
        /* may check feof here to make a difference between eof and io failure -- network
           timeout for instance */

        fclose(file);
        
         size_t indexOfNullTerminator = strlen(line);
        line[indexOfNullTerminator - 1] = '\0'; 

        depthFirstApply(line,sizepathfun);
        }
    }else if(argc == 4){
        if(strcmp(argv[1],"-z")== 0 &&strcmp(argv[2],"<")== 0 ){ /* check option is correct or not */
            argFlag = 1;
             char const* const fileName = argv[3]; /* should check that argc > 1 */
            FILE* file = fopen(fileName, "r"); /* should check the result */
            char line[1024];

            while (fgets(line, sizeof(line), file)) {
                /* note that fgets don't strip the terminating \n, checking its
                   presence would allow to handle lines longer that sizeof(line) */
            }
            /* may check feof here to make a difference between eof and io failure -- network
               timeout for instance */

            fclose(file);

            size_t indexOfNullTerminator = strlen(line);
            line[indexOfNullTerminator - 1] = '\0'; // replace ':' with '\0'
            
            depthFirstApply(line,sizepathfun);
        }
        else if(strcmp(argv[2],">")== 0){
            depthFirstApply(argv[1],sizepathfun);
        }
    }else if(argc == 5){
        if(strcmp(argv[1],"-z")== 0 &&strcmp(argv[3],">")== 0 ){
            argFlag = 1;
            depthFirstApply(argv[2],sizepathfun);
        }
    }else{
        printf("ERROR: Too many arguments \n");
    }

    return 0;
}
/*----------------------------END OF MAIN-------------------------------------*/



/*-----------------------FUNCTION IMPLEMENTATION------------------------------*/

/* This function search files and directories in given path. When fuction 
found file check is reguler file or not. If is regular file function take size
of file and calculate directiroies size two different style.*/

int depthFirstApply(char *path,int (pathFun)(char *path1)){
    DIR *dir; /* Directory pointer */
    struct dirent *entry;
    
    
    char *newpath = (char*)malloc(sizeof(char)*PATH_MAX);  /* Make allocation for given path */
    char *innerpath = (char*)malloc(sizeof(char)*PATH_MAX); /* Make allocation for given path */
    char *mainPath = (char*)malloc(sizeof(char)*PATH_MAX); /* Make allocation for given path */


    strcpy(newpath,path); /* Take copy of given path. Use for add new path on path */
    strcpy(mainPath,path);/* Take copy of given path. Use for when need givenpath again. */


    int total = 0; /* size of directories */

    if (!(dir = opendir(path))){ /* try open given path */
        printError(2); /* if path is uncorrect call error */
        return -1; /* Return -1 because of error */ 
    }
    while ((entry = readdir(dir)) != NULL) { /* check opened directory is null or not */
        if (entry->d_type == DT_DIR) { /* Check file type: directory or not. */

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){ 
            	/*we dont want calling himself. It's make infinity loops so we checked this */
                continue;
            }

            sprintf(newpath,"%s/%s",path, entry->d_name); /* creating newpath */
            if(argFlag == 0){ /* check argument option -z is active or not */
                depthFirstApply(newpath, pathFun); /* calling fucntion with  newpath*/
            }
            else{
                int temp = 0;

                temp += depthFirstApply(newpath, pathFun); /* calling fucntion with  newpath*/
                if(temp != 1){ /* if temp type is not correct ( not equal 1) we dont add size of
                 this file to total size */
                   total += temp;
                }
                else{
                	/* do nothing */
                }
            }
            
            strcpy(newpath,mainPath); /* newpath cleaning from file name */

        } else {
            sprintf(innerpath, "%s/%s",newpath, entry->d_name); /* we take file path */
            if(specialCheck(innerpath) != -1 ){ /* check file special or not */
            total += pathFun(innerpath); /* we adding file size to total. */
            }
            else{
        printf("Special file\t\t%s\n",entry->d_name); /* if any special we warning user with filename */
                
            }
        }
    }
    closedir(dir); /* closing opened directory */
    printf("%d\t\t%s\n", total,newpath ); /*print calculated size of directory */

    free(newpath); /* in the beginning, we made allocation for this variable
    now we need free this allocated locaiton for avoid memory leak */

    free(innerpath);/* in the beginning, we made allocation for this variable
    now we need free this allocated locaiton for avoid memory leak */

    free(mainPath);/* in the beginning, we made allocation for this variable
    now we need free this allocated locaiton for avoid memory leak */

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

/*-------------------END OF FUNCTION IMPLEMENTATION---------------------------*/

//
// Created by vefik on 4/4/19.
//

//
// Created by vefik on 4/4/19.
//

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>


typedef enum {FALSE = 0, TRUE = 1} bool;


int lsf();
int sizepathfun (char *path);
int specialCheck(char * dName);
void permission(char * path);
bool isDirectory(const char * directoryName);


int main(int argc,char** argv) {
    lsf();
    return 0;
}


int lsf(){
    DIR *dir; /* Directory pointer */
    char *current = (char*)malloc(sizeof(char)*PATH_MAX); /* Make allocation for given path */

    struct dirent *entry;
    struct stat stats;


    getcwd(current,PATH_MAX);

    if (!(dir = opendir(current))){ /* try open given path */
        return -1; /* Return -1 because of error */
    }



    while ((entry = readdir(dir)) != NULL) {

         if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){ 
                /*we dont want calling himself. It's make infinity loops so we checked this */
                continue;
            }

        char *path = (char*)malloc(sizeof(char)*PATH_MAX); /* Make allocation for given path */
        sprintf(path, "%s/%s",current, entry->d_name); /* we take file path */

        if(!isDirectory(path)){

      if(specialCheck(path) != -1 ){ 
                    printf("D\t");
                }
       
        else{
            printf("S\t");        
        }


        permission(path);
            printf("\t%d\t",sizepathfun(path));
            printf("%s\t\n",entry->d_name);
        }
        free(path);
        }



    free(current);


    return 1;
}

/* This function take stats of given file. And return this file size in KB*/
int sizepathfun (char *path){
    struct stat stats;
    if(lstat(path,&stats) == -1){ /*  try open file stats */
        return 0;
    }
    else {
        return (stats.st_size); /* return file stats in KB */
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

void permission(char * path){
    struct dirent *entry;
    struct stat thestat;
        if(!isDirectory(path)){
            /* taken from stackoverflow */
        printf( (thestat.st_mode & S_IRUSR) ? " r" : " -");
        printf( (thestat.st_mode & S_IWUSR) ? "w" : "-");
        printf( (thestat.st_mode & S_IXUSR) ? "x" : "-");
        printf( (thestat.st_mode & S_IRGRP) ? "r" : "-");
        printf( (thestat.st_mode & S_IWGRP) ? "w" : "-");
        printf( (thestat.st_mode & S_IXGRP) ? "x" : "-");
        printf( (thestat.st_mode & S_IROTH) ? "r" : "-");
        printf( (thestat.st_mode & S_IWOTH) ? "w" : "-");
        printf( (thestat.st_mode & S_IXOTH) ? "x" : "-");

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
//
// Created by vefik on 4/3/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

int pwd();

int main(){
    pwd();
    return 1;

}

int pwd(){
    char *current = (char*)malloc(sizeof(char)*PATH_MAX); /* Make allocation for given path */
    getcwd(current,PATH_MAX);
    printf("%s\n",current);
    free(current);
    return 1;
}

//
// Created by vefik on 4/3/19.
//

#include <stdio.h>
#include <string.h>

int cat(char * filename);



int main(int argc,char** argv)
{
    if (strcmp(argv[1],"<") == 0 || strcmp(argv[1],"|") == 0){
        cat(argv[2]);
    }
    else{
        cat(argv[1]);
    }
    return 1;
}


int cat(char * filename){

    char c;

    FILE *fp;
    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Could not open file %s\n", filename);
        return -1;
    }

    for (c = getc(fp); c != EOF; c = getc(fp))
        printf("%c",c);


    fclose(fp);
    return 1;

}
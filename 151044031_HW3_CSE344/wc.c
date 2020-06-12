//
// Created by vefik on 4/3/19.
//
#include <stdio.h>
#include <string.h>

int wc(char * filename);




int main(int argc,char** argv)
{
        printf("\n");
    if (strcmp(argv[1],"<") == 0 || strcmp(argv[1],"|") == 0){
        wc(argv[2]);
    }
    else{
        wc(argv[1]);
    }
    return 1;
}



int wc(char * filename){

    int count = 0;
    char c;

    FILE *fp;
    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Could not open file %s\n", filename);
        return -1;
    }

    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n')
            ++count;


    fclose(fp);
    printf("%d lines\n",count);
    return 1;

}
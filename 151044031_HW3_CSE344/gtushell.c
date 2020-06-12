/*###########################################################################*/
/*CSE344 SYSTEM PROGRAMMING HOMEWORK 1                                       */
/*________________________________                                           */
/*Written by Vefik Fırat Akman on APRIL,5 2019                               */
/*                                                                           */
/*USAGE: ./shell                                                             */
/*                                                                           */
/*This program is rewritten shell                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/


/*------------------------------INCLUDES-------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
/*---------------------------END OF INCLUDES----------------------------------*/
/*-------------------------FUNCTION PROTOTYPES--------------------------------*/


char * getLine();
char ** split(char * source,int * size);
int utilities(char **args,int size);
int checkCommand(char ** args,int size);
int outputFile(char * arg);
void help();
void signalHandler(int s);
char ** backHistory(int time);

/*----------------------END OF FUNCTION PROTOTYPES----------------------------*/

/*-------------------------------GLOBALS--------------------------------------*/

extern char **environ;
int pipeLocation = -1;
int historyLocation =-1;
char history[1024][1024];
int historySize = 0;
char cwdStr[4096];
/*----------------------------END OF GLOBALS----------------------------------*/


/*-------------------------------MAIN-----------------------------------------*/

int main() {
    printf("Welcome to GtuShell. You can type 'help' for usage.\n\n");
    getcwd(cwdStr, sizeof(cwdStr));

    signal(SIGTERM,&signalHandler);

    char args[1024];
    char ** args2;
    while(1) {

    	int progress = -1;

        strcpy(args,getLine());
        strcpy(history[historySize],args);
        ++historySize;
        int size=0;
        args2 = split(args,&size);

        /*for(int i=0; i <size; ++i) {
            printf("-->%s\n", args2[i]);
        }*/

        if(checkCommand(args2,size) == -1){
            /*do nothing */
        }
        else{
            /* for history */
            if (historyLocation != -1){
                progress = 1;

                args2[0][historyLocation] = '\0'; 
                int bSize = 0;
                bSize = atoi(args2[0]);
                free(args2);
                args2 = backHistory(bSize);
            
            }
            /* calling utilities funciton for call utilities */
            progress = utilities(args2,size);
        	 if (strcmp(args2[0], "help") == 0) {
                progress = 1;
                help();
            }
            if (strcmp(	args2[0], "cd") == 0) {
                progress = 1;
                if (chdir(args2[1]) != 0) {
		            printf("Error: Path is incorrect. Please check 'help' for usage\n");
		        }
            }
            
           
            if (strcmp(args, "exit") == 0) {
                break;
            }
            if (progress == -1) {
                printf("ERROR: Command not found.\n");
            }

        }


    }


    free(args2);
    return 0;
}

/*----------------------------END OF MAIN-------------------------------------*/


/*-----------------------FUNCTION IMPLEMENTATION------------------------------*/


/* Taken from stackoverflow. This function get line from command line in shell*/
char * getLine() {
    char * line = malloc(100), * linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if(line == NULL)
        return NULL;

    for(;;) {
        c = fgetc(stdin);
        if(c == EOF)
            break;
        if(c == '\n')
            break;


        if(--len == 0) {
            len = lenmax;
            char * linen = realloc(linep, lenmax *= 2);

            if(linen == NULL) {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        *line++ = c;


    }

    *line = '\0';
    return linep;
}
/*tihs function split sentences to words */ 
char ** split(char * source,int * size){

    int i = 0;
    char *p = strtok (source, " ");
    char **array = (char**)malloc(sizeof(char)*PATH_MAX); /* Make allocation for given path */

    while (p != NULL)
    {
        array[i++]  = p;
        p = strtok (NULL, " ");
    }

    *size = i;


    return array;
}


/* most important function for program. */
int utilities(char **args,int size){
    int progress = -1;
    if (args[0] == NULL) {
        
        return 1;
    }



    /* checking which utilties come from arguments after do exec */
    if (strcmp(args[0],"lsf") == 0){
    	progress = 1;
        pid_t fork_pid;
        fork_pid = fork();
        if (fork_pid == 0) {

            if(args[1] == NULL){
                char str[4096];
	        	sprintf(str,"%s/lsf",cwdStr);
	            execvp(str,args);
            }

            else{
                if (strcmp(args[1],">") == 0){
               
                outputFile(args[2]);
                char str[4096];
	        	sprintf(str,"%s/lsf",cwdStr);
	            execvp(str,args);

                } 
                else if (strcmp(args[1],"<") == 0){
	               printf("ERROR: 'lsf' doesn't take any argument\n");

                } 
                else{
	               if( pipeLocation == -1){
						printf("ERROR: Check 'help' for usage (lsf)\n");
					}else{
			           	/* do nothing */
		           }

                } 
            }

           
            exit(1);
        }

    }

    if (strcmp(args[0],"cat") == 0){
    	progress = 1;
        pid_t fork_pid;
        fork_pid = fork();
        if (fork_pid == 0) {
            if(args[2] == NULL){
            	char str[4096];
            	sprintf(str,"%s/cat",cwdStr);
                execvp(str,args);

            }

            else{

                if (strcmp(args[2],">") == 0){
	                outputFile(args[3]);
	                char str[4096];
	            	sprintf(str,"%s/cat",cwdStr);
	                execvp(str,args);

                }

               	else if (strcmp(args[1],"<") == 0 || strcmp(args[1],"|") == 0){

	                char str[4096];
	            	sprintf(str,"%s/cat",cwdStr);
	                execvp(str,args);

                } 
                else{
	               if( pipeLocation == -1){
						printf("ERROR: Check 'help' for usage (cat)\n");
					}else{
			           	/* do nothing */
		           }

                } 

            }
            exit(1);
        }

    }

    if (strcmp(args[0],"pwd") == 0){
    	progress = 1;

        pid_t fork_pid;
        fork_pid = fork();
        if (fork_pid == 0) {
            if(args[1] == NULL){
                char str[4096];
            	sprintf(str,"%s/pwd",cwdStr);
                execvp(str,args);
            }

            else{
                if (strcmp(args[1],">") == 0 ){
               
	                outputFile(args[2]);
	                char str[4096];
	            	sprintf(str,"%s/pwd",cwdStr);
	                execvp(str,args);

                } 
                else if (strcmp(args[1],"<") == 0){
	               printf("ERROR: 'pwd' doesn't take any argument\n");

                } 
                else{
					if( pipeLocation == -1){
						printf("ERROR: Check 'help' for usage (pwd)\n");
					}else{
			           	/* do nothing */
		           }

                }  
            }
            exit(1);
        }

    }
     if (strcmp(args[0],"wc") == 0){
    	progress = 1;

        pid_t fork_pid;
        fork_pid = fork();
        if (fork_pid == 0) {
            if(args[2] == NULL){
                char str[4096];
	        	sprintf(str,"%s/wc",cwdStr);
	            execvp(str,args);
            }

            else{
                if (strcmp(args[2],">") == 0){
               
                outputFile(args[3]);
                char str[4096];
	        	sprintf(str,"%s/wc",cwdStr);
	            execvp(str,args);

            	}
            	else if (strcmp(args[1],"<") == 0){

                	char str[4096];
		        	sprintf(str,"%s/wc",cwdStr);
		            execvp(str,args);

                } 
                else{
	               if( pipeLocation == -1){
						printf("ERROR: Check 'help' for usage (wc)\n");
					}else{
			           	/* do nothing */
		           }

                }   
            }
            exit(1);
        }

    }

    if (strcmp(args[0],"bunedu") == 0){
    	progress = 1;
        pid_t fork_pid;
        fork_pid = fork();
        if (fork_pid == 0) {
            if(args[3] != NULL){
            	if(strcmp(args[2],">")== 0){
            		outputFile(args[3]);
            		char str[4096];
		        	sprintf(str,"%s/bunedu",cwdStr);
		            execvp(str,args);
            	}
            	else if(strcmp(args[3],">")== 0){
            		outputFile(args[4]);
            		char str[4096];
		        	sprintf(str,"%s/bunedu",cwdStr);
		            execvp(str,args);
            	}else if(strcmp(args[2],"<")== 0){
            		char str[4096];
		        	sprintf(str,"%s/bunedu",cwdStr);
		            execvp(str,args);
            	}
            	else{
            		if( pipeLocation == -1){
						printf("ERROR: Check 'help' for usage (bunedu)\n");
					}else{
			           	/* do nothing */
		           }
            	}
            }else{
            	char str[4096];
	        	sprintf(str,"%s/bunedu",cwdStr);
	            execvp(str,args);
            }
        	

            exit(1);
        }

    }
    /* if we have pipe we do nearly same thing for right side again */
    if(pipeLocation != -1){
    	int lok = pipeLocation+1;
    	

	    if (strcmp(args[pipeLocation],"|") == 0){
    		pipeLocation = -1;

			progress = 1;
			pid_t fork_pid;
	    	fork_pid = fork();
	    	if (fork_pid == 0) {
	            int fdIn[2], fdOut[2];

	            if (fdIn != NULL)
	            {
	                close(fdIn[1]);
	                dup2(fdIn[0], 0);
	            }
	            if (fdOut != NULL)
	            {
	                close(fdOut[0]);
	                dup2(fdOut[1],1);
	            }
	        	exit(1);
	        }

	        
	    	if (strcmp(args[lok],"wc") == 0){
		    	progress = 1;

		        pid_t fork_pid;
		        fork_pid = fork();
		        if (fork_pid == 0) {
	                char str[4096];
		        	sprintf(str,"%s/wc",cwdStr);
		            execvp(str,args);
		            
		            exit(1);
		        }

		    }
		    if (strcmp(args[lok],"pwd") == 0){
		    	progress = 1;

		        pid_t fork_pid;
		        fork_pid = fork();
		        if (fork_pid == 0) {
	                char str[4096];
		        	sprintf(str,"%s/pwd",cwdStr);
		            execvp(str,args);
		            
		            exit(1);
		        }

		    }
		    if (strcmp(args[lok],"cat") == 0){
		    	progress = 1;

		        pid_t fork_pid;
		        fork_pid = fork();
		        if (fork_pid == 0) {
	                char str[4096];
		        	sprintf(str,"%s/cat",cwdStr);
		            execvp(str,args);
		            
		            exit(1);
		        }

		    }
		    if (strcmp(args[lok],"lsf") == 0){
		    	progress = 1;

		        pid_t fork_pid;
		        fork_pid = fork();
		        if (fork_pid == 0) {
	                char str[4096];
		        	sprintf(str,"%s/lsf",cwdStr);
		            execvp(str,args);
		            
		            exit(1);
		        }

		    }
		    if (strcmp(args[lok],"bunedu") == 0){
		    	progress = 1;

		        pid_t fork_pid;
		        fork_pid = fork();
		        if (fork_pid == 0) {
	                char str[4096];
		        	sprintf(str,"%s/bunedu",cwdStr);
		            execvp(str,args);
		            
		            exit(1);
		        }

		    }


	        

	        while(wait(0)>0); /* waiting child and pipe */
		}
	}





    return progress;
}

/* check argumenrts are validate coömmand*/
int checkCommand(char ** args,int size){
    int c1=0,c2=0,c3=0,c4=0,c5=0,c6=0,c7=0,c8=0,c9=0,c10=0,c11=0,c12=0;
    int h = -1;
    int err =0;
    for(int g = 0; args[0][g] != '\0'; ++g){
            if(args[0][g] == '!'){
                h = 1;
                historyLocation = g;
            }
        }

    for(int i = 0; i < size; ++i){

        

        if (strcmp(args[i],"lsf") == 0){
            c1 = 1;

        }

        else if (strcmp(args[i],"cat") == 0){
            c2 = 1;

        }

        else if (strcmp(args[i],"pwd") == 0){
            c3 = 1;

            
        }
        else if (strcmp(args[i],"wc") == 0){
            c4 = 1;

        
        }
        else if (strcmp(args[i],"bunedu") == 0){
            c5 = 1;

        
        }
        else if (strcmp(args[i],"help") == 0){
            c6 = 1;

        
        }
        else if (strcmp(args[i],"cd") == 0){
            c7 = 1;

        
        }
        else if (strcmp(args[i],"exit") == 0){
            c8 = 1;

        }
        else if (strcmp(args[i],"|") == 0){
            c9 = 1;
            pipeLocation = i;

        }
        else if (strcmp(args[i],"<") == 0){
            c10 = 1;

        }
        else if (strcmp(args[i],">") == 0){
            c11 = 1;

        }
        else if (h  == 1){
            c12 = 1;
        }

        else{
            ++err;
        }

    }
    if(c2 == 1 || c4 == 1  || c5 == 1 || c7 == 1|| c9 == 1 || c10 == 1 || c11 == 1 ){
        err = 0;
    }
    if(err==0){
        return 1;
    }
    else{
        printf("Command not found.\n");

        if(c1 == 1){
            printf("You made a mistake. If you try use 'lsf' please type 'help' for usage\n");
        }
        if(c2 == 1){
            printf("You made a mistake. If you try use 'cat' please type 'help' for usage\n");
        }
        if(c3 == 1){
            printf("You made a mistake. If you try use 'pwd' please type 'help' for usage\n");
        }
        if(c4 == 1){
            printf("You made a mistake. If you try use 'wc' please type 'help' for usage\n");
        }
        if(c5 == 1){
            printf("You made a mistake. If you try use 'bunedu' please type 'help' for usage\n");
        }
        if(c6 == 1){
            printf("You made a mistake. If you try use 'help' please type 'help' for usage\n");
        }
         if(c7 == 1){
            printf("You made a mistake. If you try use 'cd' please type 'help' for usage\n");
        }
        if(c8 == 1){
            printf("You made a mistake. If you try use 'exit' please type 'help' for usage\n");
        }

        return -1;
    }

    c1 = c1+c2+c3+c4+c5+c6+c7+c8+c9+c10+c11+c12;

}


/* for rediriection output */
int outputFile(char * arg){
    int fide1 ;
    if ((fide1 = creat(arg , 0644)) < 0) {
        perror("Launcher: > ERROR ON OUTPUT");
        exit(4);
    }

    dup2(fide1, STDOUT_FILENO);
    close(fide1);
    return 1;

}


/* for usage. you can see usage in this function */
void help(){

	printf(" --lsf; which will list file type (R for regular file, S for non-regular(special) file),access rights\n"); 
	printf("(int the form of rwxr-xr-x, just like actual ls), file size(bytes) and file name of all files (not directories)\n");	 
	printf("in the present working directory. It doesn’t take any argument. Does not list any directory. \n\n");
		
	printf(" --pwd; which will print the path of present working directory.\n\n");

	printf(" --cd; which will change the present working directory to the location provided as argument. \n\n");

	printf(" --help; which will print the list of supported commands\n\n");


	printf(" --cat; which will print on standard output the contents of the file provided to it as argument or from standard input.\n");
	printf("The file can be on any directory on the system. (example, cat file.txt )(example2, pwd | cat ) \n\n");

	printf(" --wc; which will print on standard output the number of lines in the file provided to it as argument or the\n");
	printf("string coming from standard input until EOF character (example, Input: wc file.txt Output:55) (example2, Input:\n");
	printf("lsf | wc Output: 5 (there are 5 files in current directory so output of lsf has 5 lines) \n\n"); 

	printf(" --bunedu;  your  HW#1. This time, the file path argument of bunedu can also come from standard input. \n\n");

	printf(" --exit; which will exit the shell \n\n");

}

/*this is a signal handler for termination interrup  */
void signalHandler(int s){   

    if (s == SIGTERM) {
        printf("\nRuuuun!!! Signal is comiiing!!\n");
        exit(0); 
    }
    
}
/* take command from history */
char ** backHistory(int time){
    historyLocation = -1;
    char **array = (char**)malloc(sizeof(char)*PATH_MAX); /* Make allocation for given path */
    if(time > historySize){
        printf("ERROR: You overflow history size. History size: %d\n",historySize );
    }else if(time <= 0){
        printf("ERROR: History back time cannot <= 0\n");
    }
    strcpy(array[0],history[historySize-time-1]);
    strcpy(history[historySize],history[historySize-1]);
    --historySize;
    return array;
}

/*-------------------END OF FUNCTION IMPLEMENTATION---------------------------*/

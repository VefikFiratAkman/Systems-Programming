/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <dirent.h> /* DIR* */
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include "queue.h"


int sockfd, newsockfd;
sem_t job;
int busyThreads = 0;
char clientpath[4096];
char workingPaths[3][4096];
int numConsumers = 3;
int fileNum = 0;

void * consumer (void *path);
void error(const char *msg);





int main(int argc, char *argv[]){

	if( argc < 3){
		printf("USAGE ERROR DIR POOLSIZE PORTNUM\n");
		return -1;
	}

    sem_init(&job,0,0);
    strcpy(workingPaths[0],"");
    strcpy(workingPaths[1],"");
    strcpy(workingPaths[2],"");
    numConsumers = atoi(argv[2]);
    
    DIR* serv = opendir(argv[1]);

    if(serv){
        closedir(serv);
    }else{
        mkdir(argv[1],0777);
    }

    chdir(argv[1]);


    int portno;
    struct sockaddr_in serv_addr;



    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }



    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0){
        error("ERROR opening socket");
    }


    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[3]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        error("ERROR on binding");
    }


    listen(sockfd,5);

     /* ----------------------- SOCKED OPENED ----------------------------*/


    pthread_t * consumers = (pthread_t *) malloc(sizeof(pthread_t) * numConsumers); 
    
    for (int i = 0; i < numConsumers; ++i){
            pthread_create(&consumers[i],NULL,consumer,(void *) NULL);
        }
    /*Threadler baslatilmistir. */

    while(1){
        socklen_t clilen;
        struct sockaddr_in cli_addr;

        clilen = sizeof(cli_addr);
        printf("bekleniyor\n");
        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0){
                  error("ERROR on accept");
        }else{

            printf("alındı\n");
            int dolu = -1;

            if(busyThreads != numConsumers){
                dolu = 0;
            }

            write(newsockfd,&dolu,sizeof(dolu));


            int pathLong = -1;
            read(newsockfd,&pathLong,sizeof(pathLong));
            char comingPath[4096];
            for( int c = 0; c < pathLong; ++c){
                read(newsockfd ,&comingPath[c] , 1); 
            }

            /*path var mi kontrol et ------------------- */
            int samePath = 2;
            for(int p = 0; p < numConsumers; ++p){
                if(strcmp(workingPaths[p],comingPath) == 0){
                    samePath = 1;
                }
            }
            write(newsockfd,&samePath,sizeof(samePath));


            if(samePath != -1){
                for(int p = 0; p < numConsumers; ++p){
                    if(strcmp(workingPaths[p],"") == 0){
                        strcpy(workingPaths[p],comingPath);
                        p = numConsumers + 2;
                        }
                    }
                strcpy(clientpath,comingPath);
                sem_post(&job);

            }


            /* YUKARI DA KI WRITE READLER CLIENT ILE AYNI SAYIDA SENKRONIZE */



        }
    }   


    /*threadlerin maine geri gelmesi beklenmektedir.*/

    for (int i = 0; i < numConsumers; ++i){
        pthread_join(consumers[i],NULL);
    }
    /*threadler maine geri gelmistir.*/


    close(sockfd);

    return 0; 
}


/* ------------------------------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------------------- */



void *consumer (void *vpath){
    while(1){
        int i = 0;
        int newclient = 0;
        int clientSockFd = -1;
        char mypath[4096];
        int clientbagli = 1;
        sem_wait(&job);
        ++busyThreads;
        Queue * gelen;
        Queue * firstCollect;
        firstCollect = createQueue(4096);
        gelen = createQueue(4096);
        /* -------------------- CRITICAL SECTION ---------------------*/
        clientSockFd = newsockfd;
        strcpy(mypath,clientpath);
        
        /* PATH VAR MI KONTROLUUUUU ------------------------------------------------------------ */

        DIR* cPath = opendir(mypath);
        if(cPath){
            closedir(cPath);
        }else{
            mkdir(mypath,0777);
            newclient = 1;
        }

        /* PATH VAR MI KONTROLUUUUU ------------------------------------------------------------ */


        while(clientbagli){


            ++i;

            char c;
            bzero(&c,1);
            ssize_t x = recv(clientSockFd, &c, 1, MSG_PEEK);
           
            if(x == 0){
                printf("BAGLANTI KOPTU\n");
                 for(int p = 0; p < numConsumers; ++p){
                    if(strcmp(workingPaths[p],mypath) == 0){
                        strcpy(workingPaths[p],"");
                    }
                }
                clientbagli = 0;
                break;
            }

            sleep(1);




            int fileNum11 = 0;
         
            read(clientSockFd,&fileNum11,sizeof(fileNum11));  

            printf("fileNum::::::::::: %d \n",fileNum11 );
            for(int j = 0; j < fileNum11; ++j){

                 Node * file = (Node *) malloc(sizeof(Node)); 

                int pathLen = 0;

                read(clientSockFd,&pathLen,sizeof(int));
                char * gPath = (char *) malloc(pathLen);
                read(clientSockFd,gPath,pathLen);
                strcpy(file->path,gPath);
                int x = 0;
                read(clientSockFd,&x,sizeof(int));
                file->day = x;
                read(clientSockFd,&x,sizeof(int));
                file->hour = x;
                read(clientSockFd,&x,sizeof(int));
                file->minute = x;
                read(clientSockFd,&x,sizeof(int));
                file->size = x;
                read(clientSockFd,&x,sizeof(int));
                file->status = x;
                enqueue(gelen,file);

            }

            if(newclient == 0){ /* ESKI MUSTERIDIR */
                char p[4096];
                sprintf(p,"%s/mydatafile.log",mypath);
                int mydatafile = open(p,O_RDONLY , S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
            
                int pathLen;
                int size = 0;
                Node * file = (Node *) malloc(sizeof(Node)); 

                read(mydatafile,&size,sizeof(int));

                for(int y=0; y < size; ++y){


                    read(mydatafile,&pathLen,sizeof(int));
                    char * gPath = (char *) malloc(pathLen);


                    read(mydatafile,gPath,pathLen);

                    strcpy(file->path,gPath);
                   // printf("%s\n",file->path );

                    int x = 0;
                    read(mydatafile,&x,sizeof(int));
                    file->day = x;
                    //printf("Day %d -",x );
                    read(mydatafile,&x,sizeof(int));
                    file->hour = x;
                    //printf("hour %d -",x );
                    read(mydatafile,&x,sizeof(int));
                    file->minute = x;
                    //printf("minute %d -",x );
                    read(mydatafile,&x,sizeof(int));
                    file->size = x;
                   // printf("size %d -",x );
                    read(mydatafile,&x,sizeof(int));
                    file->status = x;
                    //printf("status %d\n",x );
                    enqueue(firstCollect,file);

                }

                close(mydatafile);

                Node * ilk = firstCollect->front;
                Node * anlik = gelen->front;

                for(int j = 0; j < firstCollect->size-1; ++j){
                    ilk = ilk->next;
                    anlik = anlik->next;
                        if(strcmp(ilk->path,anlik->path) == 0){

                            if(ilk->minute ==anlik->minute && ilk->hour ==anlik->hour  && ilk->day ==anlik->day){
                                anlik->status = 0;
                            }else{

                                /* BURAYI DUZELT ---------------------------------------------------------------------------------------------------------*/

                                printf("%s değişti\n",anlik->path);
                                anlik->status = -1;

                            

                                /* BURAYI DUZELT ---------------------------------------------------------------------------------------------------------*/

                                //strcpy(firstCollect->front->path,now->front->path);
                            }
                        }


                }






                sprintf(p,"%s/mydatafile.log",mypath);
                mydatafile = open(p,O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
               

                write(clientSockFd,&gelen->size,sizeof(int));
                

                write(mydatafile,&gelen->size,sizeof(int));

                for(int j = 0; j < gelen->size; ++j){

                    Node * file = myindex(gelen,j);

                    int pathLen = sizeof(file->path);
                    
                    write(clientSockFd,&pathLen,sizeof(int));
                    write(clientSockFd,file->path,sizeof(file->path));


                    write(clientSockFd,&file->day,sizeof(int));
                    write(clientSockFd,&file->hour,sizeof(int));
                    write(clientSockFd,&file->minute,sizeof(int));
                    write(clientSockFd,&file->size,sizeof(int));
                    write(clientSockFd,&file->status,sizeof(int));
                    
                    

                    write(mydatafile,&pathLen,sizeof(int));
                    
                    write(mydatafile,file->path,sizeof(file->path));
                    
                    write(mydatafile,&file->day,sizeof(int));
                    
                    write(mydatafile,&file->hour,sizeof(int));
                    
                    write(mydatafile,&file->minute,sizeof(int));
                    
                    write(mydatafile,&file->size,sizeof(int));
                    int check = 0;
                    write(mydatafile,&check,sizeof(int));
                    
                   



                }
                close(mydatafile);
                newclient = 2;


                    newclient = 2;

            }else if(newclient == 1){
                
                char p[4096];
                sprintf(p,"%s/mydatafile.log",mypath);
                int mydatafile = open(p,O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
               

                write(clientSockFd,&gelen->size,sizeof(int));
                

                write(mydatafile,&gelen->size,sizeof(int));

                for(int j = 0; j < gelen->size; ++j){

                    Node * file = myindex(gelen,j);

                    int pathLen = sizeof(file->path);
                    
                    write(clientSockFd,&pathLen,sizeof(int));
                    write(clientSockFd,file->path,sizeof(file->path));


                    write(clientSockFd,&file->day,sizeof(int));
                    write(clientSockFd,&file->hour,sizeof(int));
                    write(clientSockFd,&file->minute,sizeof(int));
                    write(clientSockFd,&file->size,sizeof(int));
                    write(clientSockFd,&file->status,sizeof(int));
                    
                    

                    write(mydatafile,&pathLen,sizeof(int));
                    
                    write(mydatafile,file->path,sizeof(file->path));
                    
                    write(mydatafile,&file->day,sizeof(int));
                    
                    write(mydatafile,&file->hour,sizeof(int));
                    
                    write(mydatafile,&file->minute,sizeof(int));
                    
                    write(mydatafile,&file->size,sizeof(int));
                    int check = 0;
                    write(mydatafile,&check,sizeof(int));
                    
                   



                }
                close(mydatafile);
                newclient = 0;

            }

            /* DOSYALARI ALIP YAZMAYA BASLAMA ZAMANI --------------------------------- */



            printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><>>>><<<<<><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");


        int fileNum2 = 0;
         
            read(clientSockFd,&fileNum2,sizeof(int));  

            printf("fileNum:: %d \n",fileNum2 );
            for(int j = 0; j < fileNum2; ++j){

                Node * file = (Node *) malloc(sizeof(Node)); 

                int pathLen = 0;

                read(clientSockFd,&pathLen,sizeof(int));
                char * gPath = (char *) malloc(pathLen);
                read(clientSockFd,gPath,pathLen);
                printf("%s dosyasi aktarildi    ",gPath );
                strcpy(file->path,gPath);
                
                printf("\n");
                int x = 0;
                read(clientSockFd,&x,sizeof(int));
                file->day = x;
                //printf("Day %d -",x );
                read(clientSockFd,&x,sizeof(int));
                file->hour = x;
                //printf("hour %d -",x );
                read(clientSockFd,&x,sizeof(int));
                file->minute = x;
                //printf("minute %d -",x );
                read(clientSockFd,&x,sizeof(int));
                file->size = x;
                //printf("size %d -",x );
                read(clientSockFd,&x,sizeof(int));
                file->status = x;
                //printf("status %d\n",x );
                char pathhmax[4096];
                strcpy(pathhmax,file->path);

                    char delim[] = "/";

                    char *ptr = strtok(pathhmax, delim);
                    char oncesi[4096];
                    strcpy(oncesi,"");
                    while (ptr != NULL)
                    {
                        char * x = ptr;
                        ptr = strtok(NULL, delim);
                        if(ptr == NULL){
                            break;
                        }
                        strcat(oncesi,x);
                        strcat(oncesi,"/");
                        DIR * d = opendir(oncesi);
                            if(d){
                            closedir(d);
                        }
                        else{
                            mkdir(oncesi,0777);
                        }

                    }


                FILE * fd = fopen(file->path,"w");
                for(int h = 0; h < file->size; ++h){
                    char c;
                    bzero(&c,1);
                    read(clientSockFd,&c,1);
                    fwrite(&c, 1, 1, fd);
                }
                fclose(fd);
            }









            /*eksik dosya var m ıbak
            count yolla
            count >0 ise
            dosya adı ve dosya writela */
            //eksik dosya yoksa 
            /* eslesen dosyalarin checkerini 0 yap 
            kac check -1 kaldı count tut*/
            /* countı atman gerekmeyebilir ----------*/
            /*arrayi oldugu gibi geri at */
            /* count kadar dosya okumayı bekle */




        }

        /* -------------------- CRITICAL SECTION ---------------------*/
        --busyThreads;
        close(clientSockFd);
        printf("yeay\n");

    }
    return NULL;

}





int mySync(char * path,Queue * firstCollect){

    int inputFd;
    DIR *dir; /* Directory pointer */
    struct dirent *entry;


    if (!(dir = opendir(path))){ /* try open given path */
        printf("Source path acilamadi\n");
        return -1; /* Return -1 because of error */ 
    }

   //int degisenler = 0;

    while ((entry = readdir(dir)) != NULL) { /* check opened directory is null or not */

        if (entry->d_type == DT_DIR) { /* Check file type: directory or not. */

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){ 
                /*we dont want calling himself. It's make infinity loops so we checked this */
                continue;
            }
         
            char newpath[4096];
            sprintf(newpath,"%s/%s",path, entry->d_name); /* creating newpath */
            mySync(newpath,firstCollect); /* calling fucntion with  newpath*/
           
        } else {

            char dosya[4096];
            sprintf(dosya,"%s/%s",path,entry->d_name);
            inputFd = open(dosya, O_RDONLY);
            if(inputFd == -1 ){
                printf("HATA: %s dosyasi acilamadi.\n",entry->d_name );          
            }else{
                close(inputFd);

                struct tm *foo;
                struct stat attrib;
                stat(dosya, &attrib);
                size_t size = attrib.st_size;
                foo = gmtime(&(attrib.st_mtime));
                Node * data  = (Node *) malloc(sizeof(Node)); 
                data->day = foo->tm_mday;
                data->hour = foo->tm_hour;
                data->minute = foo->tm_min;
                data->size = size;
                data->status = -1;

                strcpy(data->path,dosya);
                enqueue(firstCollect,data);
                ++fileNum;

            }

        }


    }
    closedir(dir); /* closing opened directory */
    return 0;


}













void error(const char *msg)
{
    perror(msg);
    exit(1);
}
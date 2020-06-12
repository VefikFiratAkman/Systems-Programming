#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <dirent.h> /* DIR* */
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include "queue.h"



int sockfd;
int fileNum = 0;
int firstTime = 0;
Queue* firstCollect;
Queue* now;
char * rPathh;


int byteCalculator(char * fileName);
void error(const char *msg);
int mySync(char * path);




int main(int argc, char *argv[]){

    if(argc < 3){
        printf("USAGE ERROR DIR IP PORTNO\n");
        return -1;
    }

    firstCollect = createQueue(4096); 
    rPathh = (char *) malloc(sizeof(argv[1]));
    strcpy(rPathh,argv[1]);
    int  portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }



    portno = atoi(argv[3]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");




    server = gethostbyname(argv[2]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }




    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);



    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");



/*-------------------------------END OF SOCKET-------------------------------*/
    int dolu = -3;
    read(sockfd,&dolu,sizeof(dolu));
    if(dolu == -1){
        printf("Maksimum sayida client bagli\n");
        return 0;
    }else{
        int pathLen = sizeof(rPathh);
        write(sockfd,&pathLen,sizeof(pathLen));
        write(sockfd,rPathh,sizeof(rPathh));
    }

    int samePath = -3;
    read(sockfd,&samePath,sizeof(dolu));
    if(samePath == -1){
        printf("Ayni pathde calisan var\n");
        return 0;
    }


    /* SERVER DOSYA ISLEMLERI BURADA BASLAR YUKARISINA DAHA BAKMA */
    /* YUKARIDA KI WRITE READLER SERVER LA ESIT VE SENKRONIZE */


    while(1){
        

        now = createQueue(4096); 
        fileNum = 0;

        mySync(rPathh);

       

        if(firstTime == 0){
            
            write(sockfd,&firstCollect->size,sizeof(firstCollect->size));
            for(int j = 0; j < firstCollect->size; ++j){

                Node * file = myindex(firstCollect,j);

                int pathLen = sizeof(file->path);
                
                write(sockfd,&pathLen,sizeof(int));
                write(sockfd,file->path,sizeof(file->path));
                write(sockfd,&file->day,sizeof(int));
                write(sockfd,&file->hour,sizeof(int));
                write(sockfd,&file->minute,sizeof(int));
                write(sockfd,&file->size,sizeof(int));
                write(sockfd,&file->status,sizeof(int));

            }

        }
        

        Queue * gelen;
        gelen = createQueue(4096);

        int gelenCount = 0;
        read(sockfd,&gelenCount,sizeof(int));  

        for(int j = 0; j < gelenCount; ++j){

            Node * file = (Node *) malloc(sizeof(Node));

            int pathLen = 0;

            read(sockfd,&pathLen,sizeof(int));
            char * gPath = (char *) malloc(pathLen);
            read(sockfd,gPath,pathLen);
            strcpy(file->path,gPath);
            
            int x = 0;
            read(sockfd,&x,sizeof(int));
            file->day = x;
            //printf("Day %d -",x );
            read(sockfd,&x,sizeof(int));
            file->hour = x;
            //printf("hour %d -",x );
            read(sockfd,&x,sizeof(int));
            file->minute = x;
            //printf("minute %d -",x );
            read(sockfd,&x,sizeof(int));
            file->size = x;
            read(sockfd,&x,sizeof(int));
            file->status = x;
            //printf("status %d\n",x );
            enqueue(gelen,file);

        }

        int gonderilecek = 0;
        for(int j = 0; j < gelen->size; ++j){
            Node * file = myindex(gelen,j);
            if(file->status == -1){
                ++gonderilecek;
            }
        }

        write(sockfd,&gonderilecek,sizeof(int));

         for(int j = 0; j < gelen->size; ++j){
            Node * file = myindex(gelen,j);

            if(file->status == -1){
                FILE * fd;


                int pathLen = sizeof(file->path);
                
                write(sockfd,&pathLen,sizeof(int));
                write(sockfd,file->path,sizeof(file->path));
                write(sockfd,&file->day,sizeof(int));
                write(sockfd,&file->hour,sizeof(int));
                write(sockfd,&file->minute,sizeof(int));
                write(sockfd,&file->size,sizeof(int));
                write(sockfd,&file->status,sizeof(int));


                fd = fopen(file->path,"r");


                for(int t = 0; t < file->size; ++t){

                    char c;
                    bzero(&c,1);
                    fread(&c,1, 1, fd);
                    write(sockfd,&c,1);
                }


            }
        }

        /* dosya göndermeye baslama zamanı */




        /* geri readle 
        ilk olarak client eksi dosya var mı countına bak
        checkeri 2 olsun
        if count>0 
        serverdan dosya adı ve dosya readle
        firstcollecte ekle (last modifiedlar sıkıntı)
        islem bitince ya da count <0 
        geri readlederken gelenarray isimli arraye kaydet
        checki 2 olanları check 0 ra geri çek */




        /* firstCollect ve gelenarray i kıyasla 
        check  sadece == -1 olanların countunu tut
        countı karsıya at
        check -1 olanları karsıya at */
            /* countı atman gerekmeyebilir ----------*/


        /* firscollecht ve nowı karsilastir */
        /* degisiklikleri firstcollecte kaydet */
        /* döngünün basina dön */


















   

         ++firstTime;




        Node * dat = dequeue(now);
        while(dat != NULL){
            free(dat);
            dat = dequeue(now);
        }

        free(now);
        sleep(3);
    

    }

	close(sockfd);
			
    return 0;
}











int mySync(char * path){

    int inputFd;
    DIR *dir; /* Directory pointer */
    struct dirent *entry;


    if (!(dir = opendir(path))){ /* try open given path */
        printf("Source path acilamadi\n");
        return -1; /* Return -1 because of error */ 
    }



     //int degisenler = 0;
    if(firstTime != 0){
            dir = opendir(path);    
        }

    while ((entry = readdir(dir)) != NULL) { /* check opened directory is null or not */
        

        if (entry->d_type == DT_DIR) { /* Check file type: directory or not. */

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){ 
                /*we dont want calling himself. It's make infinity loops so we checked this */
                continue;
            }
         
            char newpath[4096];
            sprintf(newpath,"%s/%s",path, entry->d_name); /* creating newpath */
            mySync(newpath); /* calling fucntion with  newpath*/
           
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
                if(firstTime == 0){
                    Node * data  = (Node *) malloc(sizeof(Node)); 
                    data->day = foo->tm_mday;
                    data->hour = foo->tm_hour;
                    data->minute = foo->tm_min;
                    data->size = size;
                    data->status = -1;

                    strcpy(data->path,dosya);
                    enqueue(firstCollect,data);
                    ++fileNum;

                }else{
                    Node * data  = (Node *) malloc(sizeof(Node)); 
                    data->day = foo->tm_mday;
                    data->hour = foo->tm_hour;
                    data->minute = foo->tm_min;
                    data->size = size;
                    data->status = -1;

                    strcpy(data->path,dosya);
                    enqueue(now,data);
                    ++fileNum;

                }

                /*if(firstTime != 0){
                    int x = 0;
                    if(f < fileNum){
                        x = f;
                    }else{
                        x = fileNum;
                    }
                    for(int i = 0; i< x; ++i){
                        for(int k = 0; k< x; ++k){
                            
                        }
                    }
                }*/

                /* ---------- ADD ANLIK ARRAY ----- */
                /* boyutuyla birlikte */

            }

        }


    }
    closedir(dir); /* closing opened directory */




    return 0;

}


int byteCalculator(char * fileName){
    FILE * fd;
    int bytes = 0;
    char c;
    
    fd = fopen(fileName,"r");
    
    while((c=getc(fd))!=EOF){
        ++bytes;    
    }

    fclose(fd);
    return bytes;
        
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

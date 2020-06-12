/*------------------------------INCLUDES-------------------------------------*/
#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h> /* DIR* */
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include "queue.h"

/*---------------------------END OF INCLUDES----------------------------------*/



/*-------------------------------GLOBALS--------------------------------------*/
int argFlag = 0;
Queue* queue;
pthread_mutex_t mutex;
sem_t empty;
sem_t full;
sem_t muxet;
int sFile = 0;
int rFile = 0;
int totalByte = 0;
int doneFlag = 0;
int jobNum = 0;
pthread_t * allThreads;
int numThreads = 0;
int descriptorLimit = 1024;
/*----------------------------END OF GLOBALS----------------------------------*/


/*-------------------------------STRUCT--------------------------------------*/

struct twoPaths{
    char source[2048];
    char destination[2048];
};
typedef struct twoPaths twoPaths;

/*---------------------------END OF STRUCT------------------------------------*/


/*-------------------------FUNCTION PROTOTYPES--------------------------------*/
void * producer (void *path);
void * consumer (void *path);
void freedom();
int specialCheck(char * dName);
void signalHandler(int s);
void signalHandlerProducer(int s);
void signalHandlerConsumer(int s);

/*----------------------END OF FUNCTION PROTOTYPES----------------------------*/

/* ./pcp consumer buffer a acopy */

/*-------------------------------MAIN-----------------------------------------*/
int main(int argc,char ** argv){
	if(argc != 5 ){
		printf("HATA: Programın kullanımı ./pcp <#consumer> <#buffer> source destination seklindedir. \n");
		return -1;
	}


	int sBuff = atoi(argv[2]);
	int numConsumers = atoi(argv[1]);
	numThreads = 1 + numConsumers;
	queue = createQueue(sBuff); 
	sem_init(&empty,0,sBuff);
	sem_init(&full,0,0);
	sem_init(&muxet,0,0);
    DIR *dir; /* Directory pointer */
	
	if (!(dir = opendir(argv[3]))){ /* try open given path */
        printf("Source path yok ya da bozuk.\n");
        return -1; /* Return -1 because of error */ 
    }	else{
		closedir(dir); /* closing opened directory */
    }
    if (!(dir = opendir(argv[4]))){ /* try open given path */
        printf("destination path yok ya da bozuk.\n");
        return -1; /* Return -1 because of error */ 
    }else{
		closedir(dir); /* closing opened directory */
    }




	pthread_t prod;

	pthread_t * consumers = (pthread_t *) malloc(sizeof(pthread_t) * numConsumers); 
	
	allThreads = (pthread_t *) malloc(sizeof(pthread_t) * (numConsumers+1)); 
	/*Threadler olusturulup bir de thread collection olarak allThreadde toplanmistir. */


	twoPaths * paths =  (twoPaths *) malloc(sizeof(twoPaths)); 

	strcpy(paths->source,argv[3]);
	strcpy(paths->destination,argv[4]);


	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);

	pthread_create(&prod,NULL,producer,(void *) paths);
	allThreads[0] = prod;

	for (int i = 0; i < numConsumers; ++i){
		pthread_create(&consumers[i],NULL,consumer,(void *) paths);
		allThreads[i+1] = consumers[i];
	}
	
	/*Threadler baslatilmistir. */



	/*threadlerin maine geri gelmesi beklenmektedir.*/
	pthread_join(prod,NULL);

	for (int i = 0; i < numConsumers; ++i){
		allThreads[i+1] = -1;
		pthread_join(consumers[i],NULL);
	}
	/*threadler maine geri gelmistir.*/


	/*program free islemier de yapilarak duzgunce kapatilmaya baslanir.*/
	free(paths);
	free(consumers);
	free(allThreads);

	freedom();
 	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    u_int64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    int sure = delta_us /1000;
    /*program calisma suresi hesaplanmistir . */

    printf("\nProgram %d ms surede tamamlandi\n",sure );
    printf("%d regular file %d special file aktarildi.\n",rFile,sFile );
    printf("Toplam da %d byte kopyalandi.\n",totalByte );


return 0;
}
/*----------------------------END OF MAIN-------------------------------------*/



/*-----------------------FUNCTION IMPLEMENTATION------------------------------*/


/* Start by creating a producer thread function that takes as a parameter an array of at least 2 entries 
(for the pathnames of the two directories). For each file in the first directory, the producer opens the 
file for reading and opens a file of the same name in the second directory for writing. If a file already 
exists in the destination directory with the same name, the file should be opened and truncated. If an error
occurs in opening either file, both files are closed, and an informative message is sent to standard output.
The two open file descriptors and the names of the files are then passed into a buffer. */
void *producer (void *vpath){

	 signal(SIGINT,&signalHandlerProducer);

 	int inputFd, outputFd, openFlags;
	mode_t filePerms;
 	twoPaths * newPathPack = (twoPaths *) malloc(sizeof(twoPaths)); 
    DIR *dir; /* Directory pointer */
    struct dirent *entry;

    twoPaths * pathPack = (twoPaths *) vpath;


    if (!(dir = opendir(pathPack->source))){ /* try open given path */
        printf("Source path acilamadi\n");
        return NULL; /* Return -1 because of error */ 
    }

    while ((entry = readdir(dir)) != NULL) { /* check opened directory is null or not */
	    if(descriptorLimit <= 1){
	    	printf("HATA: File descriptor Limiti (1024) doldu.\n");	
	    }
        if (entry->d_type == DT_DIR) { /* Check file type: directory or not. */

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){ 
            	/*we dont want calling himself. It's make infinity loops so we checked this */
                continue;
            }
         
 			char folder[4096];
 			char folder2[4096];

            sprintf(folder,"%s/%s",pathPack->source, entry->d_name); /* creating newpath */
            sprintf(folder2,"%s/%s",pathPack->destination, entry->d_name); /* creating newpath */

            mkdir(folder2,0777);

            strcpy(newPathPack->source,folder);
            strcpy(newPathPack->destination,folder2);

            producer((void *)newPathPack); /* calling fucntion with  newpath*/
          
        } else {


			sem_wait(&empty);
			//pthread_mutex_lock(&mutex);
			/*---------------------------------------CRITICAL SECTION ------------------------------------------*/
			char g[4096];
			char o[4096];
			sprintf(g,"%s/%s",pathPack->source,entry->d_name);
			inputFd = open(g, O_RDONLY);
			--descriptorLimit;
			
			sprintf(o,"%s/%s",pathPack->destination,entry->d_name);
     		/*------------INTERNETTEN ALDIM -----------------*/
			openFlags = O_CREAT | O_WRONLY | O_TRUNC;
			filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
			S_IROTH | S_IWOTH;     
     		/*------------INTERNETTEN ALDIM -----------------*/

			if(inputFd != -1 && outputFd != -1){
	     		outputFd = open(o, openFlags, filePerms);
				--descriptorLimit;
				Node * product  = (Node *) malloc(sizeof(Node)); 
				strcpy(product->path,entry->d_name);
				printf("%s dosyasi buffera eklendi.\n",entry->d_name);
				product->original = inputFd;
				product->copy = outputFd;
				enqueue(queue,product);
				free(product);
				if(specialCheck(entry->d_name) != -1){
					++rFile;
				}else{
					++sFile;
				}


				++jobNum;
			}else{
				printf("HATA: %s dosyasi acilamadi.\n",entry->d_name );
			}


			/*---------------------------------------CRITICAL SECTION ------------------------------------------*/
			sem_post(&muxet);
			//pthread_mutex_unlock(&mutex);
			sem_post(&full);

        }
    }


	closedir(dir); /* closing opened directory */
	doneFlag = 1;
	free(newPathPack);
	sem_post(&full);
	
	
	

	return NULL;

}
/*Each consumer thread reads an item from the buffer, copies the file from the source file descriptor to the
destination file descriptor, closes the files and writes a message to standard output giving the file name
and the completion status of the operation */
void * consumer (void *path){

	while(doneFlag == 0 || isEmpty(queue) != 1){
		int lp = -1;
		int lp2 = -1;
		sem_getvalue(&full,&lp);
		sem_getvalue(&empty,&lp2);
		if( lp > 0 && lp2 != 10  && jobNum != 0){
			--jobNum;
			sem_wait(&full);
			sem_wait(&muxet);
			//pthread_mutex_lock(&mutex);
			/*---------------------------------------CRITICAL SECTION ------------------------------------------*/


			ssize_t numRead;
		    char buf[4096];
		 	int inputFd = 0, outputFd = 0; 

		
			Node * product2 = dequeue(queue);
			if(product2 == NULL){
				printf("QUEUE NULL\n");
			}else{
				inputFd = product2->original;
				outputFd = product2->copy;
	    		while ((numRead = read(inputFd, buf, 4096)) > 0){
	         		write(outputFd, buf, numRead);
	         		totalByte += numRead;
	    		}
				printf("%s dosyasi aktarildi.\n",product2->path );

			   	close(inputFd);
			    close(outputFd);
			}


		/*---------------------------------------CRITICAL SECTION ------------------------------------------*/
		//	pthread_mutex_unlock(&mutex);
		sem_post(&empty);
		free(product2);

	}

}
	return NULL;

}

/* QUEUE yu freeler */
void freedom(){

	Node * product2 = dequeue(queue);
	while(product2 != NULL){
		free(product2);
		product2 = dequeue(queue);
	}

	free(queue);

}

/*File special mi regular mi kontrol eder */
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


/*SIGINT sinyalini handle eder. */
void signalHandlerProducer(int s){   

    if (s == SIGINT) {

    	printf("\nPRODUCER Signal Handler.\n");
      	for(int i = 1; i< numThreads;++i){
	   		if(allThreads[i] != -1){
    		  	pthread_cancel(allThreads[i]);

    		}
    	}
    	printf("Remaining consumers dead...\n");
	 	free(allThreads);
	    exit(0);
    }
 	


    
}




/*-------------------END OF FUNCTION IMPLEMENTATION---------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <errno.h> 
#include <stdint.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <time.h>
#include "shared_memory.h"
#include "read_functions.h"


#define KEY 27
#define ERROR_VAL -1
#define SEM_CHILD_LOCK "Child Critical Section"
#define SEM_REQUEST "Request"
#define SEM_RESPONSE "Response"

pid_t pid;

int main(void){

    /*----------------------------------Initilizations----------------------------------*/ 
    int child_number, i, j, id, K, N, numlines;
    FILE* fp;
    int* line_number = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    int* child_counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    char* shared_memory;
    char* line;
    char* filename;
    struct timespec start, end;
    long seconds, nanoseconds;
    double time_elapsed;

    (*child_counter) = 0; //initializing child counter
    srand(time(0)); //initializing srand()

    create_shared_memory_block(KEY);  //creating memory block
    shared_memory = attach_shared_memory_block(KEY); //attaching to a pointer

    sem_unlink(SEM_CHILD_LOCK);   //unlinking semaphores just in case
    sem_unlink(SEM_REQUEST);
    sem_unlink(SEM_RESPONSE);
    sem_t* sem_child_lock;    
    sem_t* sem_request;
    sem_t* sem_response;
    sem_child_lock = sem_open(SEM_CHILD_LOCK , O_CREAT, 0644, 1);    //initializing with value 1
    sem_request = sem_open(SEM_REQUEST, O_CREAT, 0644, 0);    //initializing with value 0
    sem_response = sem_open(SEM_RESPONSE, O_CREAT, 0644, 0);    
    /*----------------------------------Initilizations----------------------------------*/  




    /*----------------------------------User Input----------------------------------*/  
    printf("Input the name of your file: ");
    filename = read_filename_from_in();   //getting X
    if((fp = fopen(filename, "r")) == NULL){    //opening file 
        perror("File not found\n");
        return ERROR_VAL;
    }

    printf("Input number of child processes: ");
    if((K = read_number_from_in()) == -1) return ERROR_VAL;     //getting K

    printf("Input number of queries: ");
    if((N = read_number_from_in()) == -1) return ERROR_VAL;     //getting N
    /*----------------------------------User Input----------------------------------*/  



    /*----------------------------------Creating Children----------------------------------*/ 
    numlines = number_lines(fp);  //get the number of lines of the file

    for(i = 1; i <= K; i++){  //loop to create forks
        id = fork();
        if(id == 0){ //looping only if the process is the parent process
            (*child_counter)++;     //child creation counter
            child_number = (*child_counter); //child's order number
            pid = getpid(); //actual pid
            break; //breaking the loop if the process is a child so as to not create unneccessary processes
        }
        if(id <0){
            return ERROR_VAL;
        }
    }
    /*----------------------------------Creating Children----------------------------------*/ 




    /*------------------------------------------------------------Child Processes------------------------------------------------------------*/   
    if(id == 0){        //if the process is a child child
        time_elapsed = 0; //initiating counter for every child
        for(i = 1; i <= N; i++){
            /*------------------------------Starting Critical Section------------------------------*/
            sem_wait(sem_child_lock); //locking the section

            (*line_number) = ((rand() % numlines) +1); //getting random line from the maximum number of available lines
            
            printf("\nChild no.%d with id %d: Requesting line %d\n",child_number, pid, (*line_number));
            sem_post(sem_request); //requesting line

            clock_gettime(CLOCK_REALTIME, &start); //start counting time for the response
            sem_wait(sem_response); //waiting for response
            clock_gettime(CLOCK_REALTIME, &end); //stop couting time for the response

            seconds = end.tv_sec - start.tv_sec; 
            nanoseconds = end.tv_nsec - start.tv_nsec;
            time_elapsed+= seconds + (nanoseconds*1e-9);  //calculate time

            attach_shared_memory_block(KEY); //attaching shared memory

            printf("Child no.%d with id %d: Reading <%s> from shared memory\n", child_number, pid, shared_memory);

            detach_shared_memory_block(KEY); //detaching shared memory

            sem_post(sem_child_lock); //unlocking the section

            /*------------------------------Ending Critical Section------------------------------*/    
        }
        printf("\n-----Child no.%d with id %d: Exiting... Average time for Parent's response was %.3f seconds-----\n", child_number, pid, (time_elapsed/N));
        exit(EXIT_SUCCESS); //child exiting
    }
    /*------------------------------------------------------------Child Process------------------------------------------------------------*/    




/*------------------------------------------------------------Parent Process------------------------------------------------------------*/    
    else{         //if the process is the parent
        for(i = 1; i <= (K*N); i++){ //loop for as many children and requests you are about to get
            sem_wait(sem_request); //waiting for request
            
            line = read_line_from_file(fp, (*line_number)); //read the specified line from the file
            strcpy(shared_memory, line);  // copy the line into the shared memory

            detach_shared_memory_block(KEY); //detach memory block 
            
            sem_post(sem_response); //posting response
        }
    }

/*------------Shared Memories------------*/    
    destroy_shared_memory_block(KEY);
    munmap(line_number, sizeof(int));
    munmap(child_counter, sizeof(int));

/*------------Semaphores------------*/    
    sem_destroy(sem_child_lock);
    sem_destroy(sem_request);
    sem_destroy(sem_response);

/*------------Mallocs------------*/    
    free(filename);
    free(line);
    
    return 0;
}
#include <stdio.h>
#include <sys/shm.h>
#include "shared_memory.h"

void create_shared_memory_block(int key){
    shmget(key, BLOCK_SIZE, 0666 | IPC_CREAT);
}

int get_shared_memory_block_id(int key){
    return shmget(key, BLOCK_SIZE, 0);
}

char* attach_shared_memory_block(int key){
    int shmid = get_shared_memory_block_id(key);
    return shmat(shmid, NULL, 0);
}

void detach_shared_memory_block(int key){
    char* shm = attach_shared_memory_block(key);
    shmdt(shm);
}

void destroy_shared_memory_block(int key){
    shmctl(get_shared_memory_block_id(key), IPC_RMID, NULL);
}

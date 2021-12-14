#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_functions.h"

int number_lines(FILE* file){
    int counter = 1;
    if (file == NULL) return -1; 

    char c = getc(file);
    
    while(c != EOF){
        c = getc(file);
        if(c == '\n'){
            counter++;
        }
    }
    fseek(file, 0, SEEK_SET);   //reset the file pointer
    return counter;
}

char* read_line_from_file(FILE* file, int line_number){
    int i;
    char* buffer;
    size_t bufsize = 300;
    if (file == NULL) return NULL;

    buffer = (char*)malloc(bufsize * sizeof(char)); 
    if (buffer == NULL) return NULL;

    for(i = 1; i <= line_number; i++){
        getline(&buffer, &bufsize, file);
    }
    
    buffer[strcspn(buffer, "\n")] = 0;  //remove the new line from the input 
    fseek(file, 0, SEEK_SET);   //reset the file pointer

    return buffer;
}

char* read_filename_from_in(void){
    char* buffer;   //for the input of the file
    size_t bufsize = 32; 

    buffer = (char*)malloc(bufsize * sizeof(char)); 
    if(buffer == NULL) return NULL;

    getline(&buffer, &bufsize, stdin);

    buffer[strcspn(buffer, "\n")] = '\0';   //remove the new line from the input 

    return buffer;
}

int read_number_from_in(void){
    int number;
    if((scanf("%d", &number) != 1)){
        return -1;
    }
    return number;
}

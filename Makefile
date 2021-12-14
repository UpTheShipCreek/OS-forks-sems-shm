
output: main.o read_functions.o shared_memory.o 
	gcc main.o read_functions.o shared_memory.o -lpthread -lrt -o output

main.o: main.c
	gcc -c main.c 

read_functions.o: read_functions.c read_functions.h
	gcc -c read_functions.c

shared_memory.o: shared_memory.c shared_memory.h
	gcc -c shared_memory.c

clean:
	rm *.o output
    
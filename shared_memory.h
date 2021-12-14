#define BLOCK_SIZE 4096

void create_shared_memory_block(int key);

int get_shared_memory_block_id(int key);

char* attach_shared_memory_block(int key);

void detach_shared_memory_block(int key);

void destroy_shared_memory_block(int key);
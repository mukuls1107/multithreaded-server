#include "../lib/proxy_parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_REQ 10

struct cache_element {
    char* data;
    int length;
    char* url;
    time_t unit_time_track;
    struct cache_element* next_ele;
};

struct cache_element* find(char* url);
int push_element(char* data, int size, char* url);
void remove_element();

// Default localhost port
int port = 8080;
int SOCKET_IDs;

pthread_t thread_id[MAX_REQ];
sem_t semaphores;
pthread_mutex_t lock;

struct cache_element* head;  // Use struct here
int size_cache;

int main(int argc, char* argv[]) {
    int socketId, client_length;
    struct sockaddr server_address, client_addr;
    
    // Initialize the semaphore
    sem_init(&semaphores, 0, MAX_REQ);

    // Initialize the mutex
    pthread_mutex_init(&lock, NULL);

    // Check if the number of arguments is 2
    if (argc == 2) {
        port = atoi(argv[1]);
    } else {
        printf("Port Number isn't defined.\n");
        exit(1);
    }

    printf("Server starting at %d\n", port);

    // Create a socket
    SOCKET_IDs = socket(AF_INET, SOCK_STREAM, 0);

    return 0;
}

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
#define MAX_BYTES 4096
struct cache_element
{
    char *data;
    int length;
    char *url;
    time_t unit_time_track;
    struct cache_element *next_ele;
};

struct cache_element *find(char *url);
int push_element(char *data, int size, char *url);
void remove_element();

// Default localhost port
int port = 8080;
int SOCKET_IDs;

pthread_t thread_id[MAX_REQ];
sem_t semaphores;
pthread_mutex_t lock;

struct cache_element *head; 
int size_cache;

void thread_fn(void *socketNew)
{
    sem_wait(&semaphores);
    int p;
    sem_getvalue(&semaphores, p);
    printf("Semaphore value %d\n", p);
    int *t = (int *)socketNew;
    int socket = *t;
    int bytes_send_client, len;

    char *buffer = (char *)calloc(MAX_BYTES, sizeof(char));
    bzero(buffer, MAX_BYTES);
    bytes_send_client = recv(socket, buffer, MAX_BYTES, 0);

    while (bytes_send_client > 0)
    {
        len = strlen(buffer);
        if (strstr(buffer, "\r\n\n") == NULL)
        {
            bytes_send_client = recv(socket, buffer + len, MAX_BYTES - len, 0);
        }
        else
        {
            break;
        }
    }

    char*tempreq = (char * )malloc(strlen(buffer)*sizeof(char)+1);

}

int main(int argc, char *argv[])
{
    int socketId, client_length;
    struct sockaddr_in server_address, client_addr;

    // Initialize the semaphore
    sem_init(&semaphores, 0, MAX_REQ);

    // Initialize the mutex
    pthread_mutex_init(&lock, NULL);

    // Check if the number of arguments is 2
    if (argc == 2)
    {
        port = atoi(argv[1]);
    }
    else
    {
        printf("Port Number isn't defined.\n");
        exit(1);
    }

    printf("Server starting at %d\n", port);

    // Create a socket
    SOCKET_IDs = socket(AF_INET, SOCK_STREAM, 0);
    if (SOCKET_IDs < 0)
    {
        printf("Failed to create a socket\n");
        exit(1);
    }

    int reuse = 1;
    if (setsockopt(SOCKET_IDs, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)) < 0)
    {
        perror("set sock option failed\n");
    }

    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(SOCKET_IDs, (struct sockaddr *)&server_address, sizeof(server_address) < 0))
    {
        perror("PORT is not available.\n");
        exit(1);
    }

    printf("Binding on port: %d\n", port);

    int listen_status = listen(SOCKET_IDs, MAX_REQ);
    if (listen_status < 0)
    {
        perror("Error in listening.\n");
        exit(1);
    }

    int i = 0;
    int connected_socketID[MAX_REQ];
    while (1)
    {
        bzero((char *)&client_addr, size(client_addr));
        client_length = sizeof(client_addr);
        socketId = accept(SOCKET_IDs, (struct sockaddr *)&client_addr, (socklen_t *)client_length);

        if (socketId < 0)
        {
            perror("Not able to connect.\n");
            exit(1);
        }
        else
        {

            connected_socketID[i] = socketId;
        }

        struct sockaddr_in *client_pt = (struct sockaddr_in *)&client_addr;
        struct in_addr ip_addr = client_pt->sin_addr;
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ip_addr, str, INET_ADDRSTRLEN);
        printf("CLient is connected to PORT %d & IP ADDRESS %s\n.", ntohs(client_addr.sin_port), str);

        pthread_create(&thread_id[i], NULL, thread_fn, (void *)&connected_socketID[i]);
        i++;
    }
    close(SOCKET_IDs);
    return 0;
}

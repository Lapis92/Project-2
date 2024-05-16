#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>

#define PortNumber 6789
#define MaxConnects 8
#define BuffSize 256

void signalHandle();
void *handleClient(void *client_fd_ptr);

pthread_mutex_t file_mutex;

int main() {
    signal(SIGINT, signalHandle);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("socket\n");
        return -1;
    }

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(PortNumber);

    if (bind(fd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
        printf("bind error\n");
        return -1;
    }

    if (listen(fd, MaxConnects) < 0) {
        printf("listen error\n");
        return -1;
    }

    fprintf(stderr, "Listening on port %i for clients...\n", PortNumber);
    printf("If exit is needed, press CTRL+C.\n");

    pthread_mutex_init(&file_mutex, NULL);

    while (1) {
        struct sockaddr_in caddr;
        socklen_t len = sizeof(caddr);

        int client_fd = accept(fd, (struct sockaddr*)&caddr, &len);
        if (client_fd < 0) {
            printf("accept error\n");
            continue;
        }

        pthread_t thread_id;
        int *client_fd_ptr = malloc(sizeof(int));
        *client_fd_ptr = client_fd;
        pthread_create(&thread_id, NULL, handleClient, client_fd_ptr);
        pthread_detach(thread_id);
    }

    pthread_mutex_destroy(&file_mutex);
    return 0;
}

void *handleClient(void *client_fd_ptr) {
    int client_fd = *(int*)client_fd_ptr;
    free(client_fd_ptr);

    char message[BuffSize];
    memset(message, '\0', sizeof(message));

    while (read(client_fd, message, sizeof(message)) > 0) {
        pthread_mutex_lock(&file_mutex);

        FILE *chat_history = fopen("chat_history.txt", "a");
        if (chat_history != NULL) {
            fprintf(chat_history, "%s", message);
            fclose(chat_history);
        }

        pthread_mutex_unlock(&file_mutex);

        write(client_fd, message, sizeof(message));
        printf("%s", message);
        memset(message, '\0', sizeof(message));
    }

    close(client_fd);
    return NULL;
}

void signalHandle() {
    printf("\nBye bye!\n");

    pthread_mutex_lock(&file_mutex);

    FILE *chat_history = fopen("chat_history.txt", "a");
    if (chat_history != NULL) {
        fprintf(chat_history, "–––––––––––––––––––––\n");
        fprintf(chat_history, "Server terminated at %s\n", asctime(localtime(&(time_t){time(NULL)})));
        fclose(chat_history);
    }

    pthread_mutex_unlock(&file_mutex);

    exit(0);
}

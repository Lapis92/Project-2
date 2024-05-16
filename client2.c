#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PortNumber 6789
#define BuffSize 256
#define Host "localhost"

void *receiveMessages(void *sockfd_ptr);

int main() {
    char username[BuffSize];
    printf("Enter Username: ");
    scanf("%s", username);
    getchar();  // Consume the newline character left by scanf
    strcat(username, ": ");

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("bad socket\n");
        return -1;
    }

    struct hostent *hptr = gethostbyname(Host);
    if (!hptr) {
        printf("bad gethostbyname\n");
        return -1;
    }
    if (hptr->h_addrtype != AF_INET) {
        printf("bad address family\n");
        return -1;
    }

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = ((struct in_addr*)hptr->h_addr_list[0])->s_addr;
    saddr.sin_port = htons(PortNumber);

    if (connect(sockfd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0) {
        printf("bad connection\n");
        return -1;
    }

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, receiveMessages, &sockfd);

    char outMsg[BuffSize];
    char complete_line[BuffSize];

    while (1) {
        memset(outMsg, '\0', sizeof(outMsg));
        fgets(outMsg, sizeof(outMsg), stdin);

        strcpy(complete_line, username);
        strcat(complete_line, outMsg);
        write(sockfd, complete_line, strlen(complete_line));
    }

    close(sockfd);
    return 0;
}

void *receiveMessages(void *sockfd_ptr) {
    int sockfd = *(int*)sockfd_ptr;
    char inMsg[BuffSize];

    while (1) {
        memset(inMsg, '\0', sizeof(inMsg));
        if (read(sockfd, inMsg, sizeof(inMsg)) > 0) {
            printf("%s", inMsg);
        }
    }

    return NULL;
}

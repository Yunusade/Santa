#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

void connectToServer() {
    int sockfd;
    struct sockaddr_in server_address;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    int command = 0;
    send(sockfd, &command, sizeof(int), 0);

    recv(sockfd, &command, sizeof(int), 0);
    printf("Connected to server.\n");
    close(sockfd);
}

void registerParticipant(int sockfd, char *name) {
    struct sockaddr_in server_address;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    int command = 1;
    int length = strlen(name);
    send(sockfd, &command, sizeof(int), 0);
    send(sockfd, &length, sizeof(int), 0);
    send(sockfd, name, length, 0);

    recv(sockfd, &command, sizeof(int), 0);
    printf("Participant %s registered.\n", name);

    close(sockfd);
}

void triggerDrawing(int sockfd) {
    int command = 2;
    send(sockfd, &command, sizeof(int), 0);
    printf("Drawing initiated.\n");
}

void fetchAssignedGiftee(int sockfd) {
    int length;
    recv(sockfd, &length, sizeof(int), 0);

    if (length > 0) {
        char gifteeName[length + 1];
        recv(sockfd, gifteeName, length, 0);
        gifteeName[length] = '\0';

        printf("Your assigned giftee is: %s\n", gifteeName);
    } else {
        printf("Error fetching assigned giftee.\n");
    }
}


int main() {
    int sockfd;
    connectToServer();

    int continueAdding = 1;

    while (continueAdding) {
        char name[256];
        printf("Enter participant name: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = '\0';  

        registerParticipant(sockfd, name);

        printf("Do you want to continue adding names? (1 for yes, 0 for no): ");
        scanf("%d", &continueAdding);
        getchar(); 
    }

    triggerDrawing(sockfd);


    fetchAssignedGiftee(sockfd);

    close(sockfd);

    return 0;
}

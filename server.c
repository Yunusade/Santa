#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 10

typedef struct {
  int sockfd;
  char name[256];
} Participant;

Participant participants[MAX_CLIENTS];
int numParticipants = 0;

void handleRegister(int sockfd, int length, char *name) {
  if (numParticipants < MAX_CLIENTS) {
      participants[numParticipants].sockfd = sockfd;
      strncpy(participants[numParticipants].name, name, length);
      participants[numParticipants].name[length] = '\0';
      numParticipants++;
      printf("Participant %s registered.\n", name);
  } else {
      printf("Server full. Cannot register more participants.\n");
  }
}

void handleDrawing() {
  // Implement drawing logic to assign secret santas to giftees
  // Update the participants array accordingly
  // Notify clients about their assigned giftees
}

int main() {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
      perror("Socket creation failed");
      exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
      perror("Bind failed");
      exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 3) < 0) {
      perror("Listen failed");
      exit(EXIT_FAILURE);
  }

  printf("Server listening on port %d...\n", PORT);

  while (1) {
      if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
          perror("Accept failed");
          exit(EXIT_FAILURE);
      }

      int command;
      recv(new_socket, &command, sizeof(int), 0);

      switch (command) {
          case 0: // Connect
              send(new_socket, &command, sizeof(int), 0);
              printf("Client connected.\n");
              break;

          case 1: // Register
              {
                  int length;
                  recv(new_socket, &length, sizeof(int), 0);
                  char name[length + 1];
                  recv(new_socket, name, length, 0);
                  name[length] = '\0';
                  handleRegister(new_socket, length, name);
                  send(new_socket, &command, sizeof(int), 0);
              }
              break;

          case 2: // Drawing
              handleDrawing();
              printf("Drawing completed.\n");
              break;
      }
  }

  return 0;
}

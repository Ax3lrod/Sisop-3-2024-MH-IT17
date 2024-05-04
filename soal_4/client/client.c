#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Create the socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while (1) {
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline character

        if (strcmp(buffer, "exit") == 0) {
            if (send(sock, buffer, strlen(buffer), 0) < 0) {
                perror("Send failed");
                return -1;
            }
            break;
        }

        int hasKeyword = 0;
        const char *keywords[] = {"add", "edit", "genre", "status", "delete", "tampilkan", "hari"};
        for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
            if (strstr(buffer, keywords[i]) != NULL) {
                hasKeyword = 1;
                break;
            }
        }

        if (!hasKeyword) {
            printf("Server:\nInvalid Command\n\n");
            continue;
        }

        if (send(sock, buffer, strlen(buffer), 0) < 0) {
            perror("Send failed");
            return -1;
        }

        int total_read = 0;
        int valread;
        while (total_read < BUFFER_SIZE - 1) {
            valread = read(sock, buffer + total_read, BUFFER_SIZE - total_read - 1);
            if (valread < 0) {
                perror("Read failed");
                return -1;
            }
            total_read += valread;
            if (buffer[total_read - 1] == '\n') {
                buffer[total_read] = '\0';
                break;
            }
        }
        printf("Server:\n%s\n", buffer);
    }

    printf("Server:\nExiting the client\n");
    close(sock);
    return 0;
}
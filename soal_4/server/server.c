#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>  // Include for inet_ntoa
#include <unistd.h>
#include <time.h>
#include <sys/stat.h> // Include for stat
#include <errno.h>    // Include for errno

#define PORT 8080
#define BUFFER_SIZE 1024
#define FILE_PATH "/home/ax3lrod/sisop/modul3/soal_4/myanimelist.csv"
#define URL "https://drive.google.com/uc?export=download&id=10p_kzuOgaFY3WT6FVPJIXFbkej2s9f50"
#define LOG_FILE "/home/ax3lrod/sisop/modul3/soal_4/change.log"

void downloadFile(const char* filePath, const char* url) {
    // Buat perintah untuk mengunduh file dengan wget
    char command[1024];
    sprintf(command, "wget -O %s \"%s\"", filePath, url);

    // Jalankan perintah menggunakan system call
    int status = system(command);
    
    // Periksa status system call
    if (status == -1) {
        perror("Error: Unable to execute wget command");
        exit(EXIT_FAILURE);
    } else if (status != 0) {
        fprintf(stderr, "Error: wget command failed with status %d\n", status);
        exit(EXIT_FAILURE);
    }

    printf("File downloaded successfully to %s\n", filePath);
}

void logChange(const char* type, const char* message) {
    // Cek apakah file change.log sudah ada atau belum
    struct stat st;
    if (stat(LOG_FILE, &st) == -1) {
        // Jika file belum ada, buat file change.log terlebih dahulu
        FILE* newLogFile = fopen(LOG_FILE, "w");
        if (newLogFile == NULL) {
            perror("Failed to create log file");
            exit(EXIT_FAILURE);
        }
        fclose(newLogFile);
    }

    // Lanjutkan untuk menulis log ke dalam file
    FILE* logFile = fopen(LOG_FILE, "a");
    if (logFile == NULL) {
        perror("Failed to open log file");
        return;
    }
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    fprintf(logFile, "[%02d/%02d/%02d] [%s] %s\n", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, type, message);
    fclose(logFile);
}

void displayAnime(int clientSocket, char* filterType, char* filterValue) {
    FILE* file = fopen("/home/ax3lrod/sisop/modul3/soal_4/myanimelist.csv", "r");
    if (file == NULL) {
        perror("Failed to open file");
        send(clientSocket, "Failed to open file\n", 20, 0);
        return;
    }

    char line[1024];
    char response[4096] = {0};
    int matchFound = 0;
    int count = 1;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';

        char* day = strtok(line, ",");
        char* genre = strtok(NULL, ",");
        char* title = strtok(NULL, ",");
        char* status = strtok(NULL, "");

        if (day == NULL || genre == NULL || title == NULL || status == NULL) {
            continue;
        }

        if ((strcmp(filterType, "all") == 0) ||
            (strcmp(filterType, "day") == 0 && strcmp(day, filterValue) == 0) ||
            (strcmp(filterType, "genre") == 0 && strcmp(genre, filterValue) == 0) ||
            (strcmp(filterType, "title") == 0 && strcmp(title, filterValue) == 0)) {
            char entry[1024];
            snprintf(entry, sizeof(entry), "%d. %s\n", count++, title);
            if (strlen(response) + strlen(entry) < sizeof(response)) {
                strcat(response, entry);
                matchFound = 1;
            } else {
                strcat(response, "Response too long, some entries omitted.\n");
                break;
            }
        }
    }

    fclose(file);

    if (!matchFound) {
        strcpy(response, "No matching anime found.\n");
    }

    send(clientSocket, response, strlen(response), 0);
}

void addAnime(int clientSocket, char* day, char* genre, char* title, char* status) {
    FILE* file = fopen("/home/ax3lrod/sisop/modul3/soal_4/myanimelist.csv", "a");
    fprintf(file, "\n%s,%s,%s,%s", day, genre, title, status);
    fclose(file);
    char logMessage[1024];
    sprintf(logMessage, "%s added", title);
    logChange("ADD", logMessage);
    send(clientSocket, "Anime added successfully\n", 25, 0);
}

void editAnime(int clientSocket, char* oldTitle, char* newDay, char* newGenre, char* newTitle, char* newStatus) {
    FILE* file = fopen("/home/ax3lrod/sisop/modul3/soal_4/myanimelist.csv", "r");
    FILE* tempFile = fopen("/home/ax3lrod/sisop/modul3/soal_4/temp.csv", "w");
    char line[1024];
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
        char* day = strtok(line, ",");
        char* genre = strtok(NULL, ",");
        char* title = strtok(NULL, ",");
        char* status = strtok(NULL, "\n");
        if (strcmp(title, oldTitle) == 0) {
            fprintf(tempFile, "%s,%s,%s,%s\n", newDay, newGenre, newTitle, newStatus);
            found = 1;
        } else {
            fprintf(tempFile, "%s,%s,%s,%s\n", day, genre, title, status);
        }
    }
    fclose(file);
    fclose(tempFile);
    remove("/home/ax3lrod/sisop/modul3/soal_4/myanimelist.csv");
    rename("/home/ax3lrod/sisop/modul3/soal_4/temp.csv", "/home/ax3lrod/sisop/modul3/soal_4/myanimelist.csv");
    if (found) {
        char logMessage[1024];
        sprintf(logMessage, "%s edited to %s,%s,%s,%s", oldTitle, newDay, newGenre, newTitle, newStatus);
        logChange("EDIT", logMessage);
        send(clientSocket, "Anime edited successfully\n", 26, 0);
    } else {
        send(clientSocket, "Anime not found\n", 16, 0);
    }
}

void deleteAnime(int clientSocket, char* title) {
    FILE* file = fopen("/home/ax3lrod/sisop/modul3/soal_4/myanimelist.csv", "r");
    FILE* tempFile = fopen("/home/ax3lrod/sisop/modul3/soal_4/temp.csv", "w");
    char line[1024];
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
        char* day = strtok(line, ",");
        char* genre = strtok(NULL, ",");
        char* titleLine = strtok(NULL, ",");
        char* status = strtok(NULL, "\n");
        if (strcmp(titleLine, title) != 0) {
            fprintf(tempFile, "%s,%s,%s,%s\n", day, genre, titleLine, status);
        } else {
            found = 1;
        }
    }
    fclose(file);
    fclose(tempFile);
    remove("/home/ax3lrod/sisop/modul3/soal_4/myanimelist.csv");
    rename("/home/ax3lrod/sisop/modul3/soal_4/temp.csv", "/home/ax3lrod/sisop/modul3/soal_4/myanimelist.csv");
    if (found) {
        char logMessage[1024];
        sprintf(logMessage, "%s deleted", title);
        logChange("DEL", logMessage);
        send(clientSocket, "Anime deleted successfully\n", 27, 0);
    } else {
        send(clientSocket, "Anime not found\n", 16, 0);
    }
}

void getStatus(int clientSocket, char* title) {
    FILE* file = fopen("/home/ax3lrod/sisop/modul3/soal_4/myanimelist.csv", "r");
    if (file == NULL) {
        perror("Failed to open file");
        send(clientSocket, "Failed to open file\n", 20, 0);
        return;
    }

    char line[1024];
    char response[4096] = {0};
    int matchFound = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';

        char* day = strtok(line, ",");
        char* genre = strtok(NULL, ",");
        char* animeTitle = strtok(NULL, ",");
        char* status = strtok(NULL, "");

        if (day == NULL || genre == NULL || animeTitle == NULL || status == NULL) {
            continue;
        }

        if (strcmp(animeTitle, title) == 0) {
            snprintf(response, sizeof(response), "Status of %s: %s\n", title, status);
            matchFound = 1;
            break;
        }
    }

    fclose(file);

    if (!matchFound) {
        snprintf(response, sizeof(response), "Anime '%s' not found\n", title);
    }

    send(clientSocket, response, strlen(response), 0);
}

void handleCommand(int clientSocket, char* command) {
    char* token = strtok(command, " ");  // Memisahkan token menggunakan spasi sebagai delimiter
    int validCommand = 0; // Flag untuk menandai apakah perintah valid atau tidak
    if (token != NULL) {
        if (strcmp(token, "tampilkan") == 0) {
            displayAnime(clientSocket, "all", "");
            validCommand = 1; // Menandai perintah sebagai valid
        } else if (strcmp(token, "hari") == 0) {
            token = strtok(NULL, "");  // Mengambil sisa string setelah "hari "
            if (token)
                displayAnime(clientSocket, "day", token);
            else
                send(clientSocket, "Missing day parameter\n", 22, 0);
            validCommand = 1; // Menandai perintah sebagai valid
        } else if (strcmp(token, "genre") == 0) {
            token = strtok(NULL, "");  // Mengambil sisa string setelah "genre "
            if (token)
                displayAnime(clientSocket, "genre", token);
            else
                send(clientSocket, "Missing genre parameter\n", 24, 0);
            validCommand = 1; // Menandai perintah sebagai valid
        } else if (strcmp(token, "status") == 0) {
            token = strtok(NULL, "");  // Mengambil sisa string setelah "status "
            if (token)
                getStatus(clientSocket, token);
            else
                send(clientSocket, "Missing title for status check\n", 30, 0);
            validCommand = 1; // Menandai perintah sebagai valid
        } else if (strcmp(token, "add") == 0) {
            // Tokenize the rest of the command using commas as delimiters
            char* day = strtok(NULL, ",");
            char* genre = strtok(NULL, ",");
            char* title = strtok(NULL, ",");
            char* status = strtok(NULL, "");

            if (day && genre && title && status) {
                // Send the parameters to the addAnime function
                addAnime(clientSocket, day, genre, title, status);
                validCommand = 1; // Menandai perintah sebagai valid
            } else {
                send(clientSocket, "Missing parameters for add\n", 27, 0);
            }
        } else if (strcmp(token, "edit") == 0) {
            char* oldTitle = strtok(NULL, ",");
            char* newDay = strtok(NULL, ",");
            char* newGenre = strtok(NULL, ",");
            char* newTitle = strtok(NULL, ",");
            char* newStatus = strtok(NULL, "");
            if (oldTitle && newDay && newGenre && newTitle && newStatus) {
                editAnime(clientSocket, oldTitle, newDay, newGenre, newTitle, newStatus);
                validCommand = 1; // Menandai perintah sebagai valid
            } else {
                send(clientSocket, "Missing parameters for edit\n", 28, 0);
            }
        } else if (strcmp(token, "delete") == 0) {
            token = strtok(NULL, "");  // Mengambil sisa string setelah "delete "
            if (token)
                deleteAnime(clientSocket, token);
            else
                send(clientSocket, "Missing title for delete\n", 25, 0);
            validCommand = 1; // Menandai perintah sebagai valid
        }
    }

    // Jika perintah tidak valid, kirim pesan "Invalid Command" ke klien
    if (!validCommand) {
        char* message = "Invalid Command";
        int send_status = send(clientSocket, message, strlen(message), 0);
        if (send_status == -1) {
            perror("Failed to send message");
        }
    }
}

int main() {
    downloadFile(FILE_PATH, URL);

    int server_fd, clientSocket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is running and waiting for connections...\n");

    while (1) {
        if ((clientSocket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;  // Continue to accept next connection
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

        // Handle client requests in a loop
        while (1) {
            int read_size = read(clientSocket, buffer, BUFFER_SIZE);
            if (read_size > 0) {
                buffer[read_size] = '\0';  // Null-terminate the buffer
                printf("Command received: %s\n", buffer);
                handleCommand(clientSocket, buffer);
            } else if (read_size == 0) {
                printf("Client disconnected.\n");
                break;  // Exit the inner loop if client disconnected
            } else {
                perror("recv failed");
                break;  // Exit the inner loop on receive error
            }
        }

        close(clientSocket);
    }

    close(server_fd);
    return 0;
}

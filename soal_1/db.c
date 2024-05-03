#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define SHM_SIZE 1024  // Define the size of the shared memory
#define SHM_KEY_START 0x1234  // Starting shared memory key
#define SHM_KEY_END 0x1244    // Ending shared memory key

const char* getFileType(const char* filename) {
    if (strstr(filename, "trashcan")) {
        return "Trash Can";
    } else if (strstr(filename, "parkinglot")) {
        return "Parking Lot";
    } else {
        return "Unknown";
    }
}

int main() {
    int shmid;
    char *shm;
    char new_path[1024];
    char old_path[1024];
    time_t now;
    struct tm *now_tm;
    char timestamp[20];

    for (int key = SHM_KEY_START; key <= SHM_KEY_END; key++) {
        // Access the shared memory segment
        shmid = shmget(key, SHM_SIZE, 0666);
        if (shmid < 0) {
            perror("shmget failed");
            break;  // Exit the loop if no shared memory segment is found
        }

        // Attach the segment to our data space
        shm = shmat(shmid, NULL, 0);
        if (shm == (char *) -1) {
            perror("shmat failed");
            continue;  // Skip to the next key
        }

        // Construct new path for the file, considering db.c is run inside microservices
        sprintf(new_path, "database/%s", strrchr(shm, '/') + 1);

        // Adjust old_path to point to the absolute location of new-data
        sprintf(old_path, "/home/ax3lrod/sisop/modul3/soal_1/new-data/%s", strrchr(shm, '/') + 1);

        if (rename(old_path, new_path) == -1) {
            perror("Failed to move file");
            printf("Failed to move file from %s to %s\n", old_path, new_path);
        } else {
            printf("File moved successfully from %s to %s\n", old_path, new_path);
        }

        // Log the file movement
        FILE *log = fopen("database/db.log", "a");
        if (log != NULL) {
            // Get current time and format it
            now = time(NULL);
            now_tm = localtime(&now);
            strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", now_tm);
            const char* fileType = getFileType(strrchr(new_path, '/') + 1);
            fprintf(log, "[%s] [%s] [%s]\n", timestamp, fileType, strrchr(new_path, '/') + 1);
            fclose(log);
        } else {
            perror("Failed to open log file");
        }

        // Detach from the shared memory
        shmdt(shm);
    }

    return 0;
}
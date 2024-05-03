#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#define SHM_SIZE 1024 // Define the size of the shared memory

int send_to_shared_memory(char *file_path, int unique_key) {
    int shmid;
    char *shm;

    // Create the shared memory segment
    shmid = shmget(unique_key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        return 1;
    }

    // Attach the segment to our data space
    shm = shmat(shmid, NULL, 0);
    if (shm == (char *) -1) {
        perror("shmat");
        return 1;
    }

    // Copy the file path to the shared memory
    strncpy(shm, file_path, SHM_SIZE);

    // Detach from the shared memory
    shmdt(shm);

    return 0;
}

int main() {
    DIR *d;
    struct dirent *dir;
    int unique_key = 0x1234; // Initial shared memory key

    d = opendir("new-data");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) { // Check if it's a regular file
                char *file_name = dir->d_name;
                char full_path[1024];
                sprintf(full_path, "new-data/%s", file_name);

                if (strstr(file_name, "trashcan.csv") || strstr(file_name, "parkinglot.csv")) {
                    printf("Processing file: %s\n", file_name);
                    printf("File %s sent to shared memory\n", file_name);
                    send_to_shared_memory(full_path, unique_key++); // Send to shared memory with unique key
                } else {
                    printf("Processing file: %s\n", file_name);
                    printf("File %s deleted because its name does not contain 'trashcan' or 'parkinglot'\n", file_name);
                    remove(full_path); // Delete the file
                }
            }
        }
        closedir(d);
    }

    return 0;
}
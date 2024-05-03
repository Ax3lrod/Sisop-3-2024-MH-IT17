#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define SHM_SIZE 1024
#define SHM_KEY_START 0x1234
#define SHM_KEY_END 0x1240
#define MAX_ENTRIES 100

typedef struct {
    char name[100];
    float rating;
} Entry;

int main() {
    int shmid;
    char *shm, *ptr;
    FILE *file;
    char line[1024];
    float max_rating = 0.0;
    Entry entries[MAX_ENTRIES];
    int entry_count = 0;
    char type[100] = {0};
    char filepath[1024];

    for (int key = SHM_KEY_START; key <= SHM_KEY_END; key++) {
        shmid = shmget(key, SHM_SIZE, 0666);
        if (shmid < 0) {
            if (errno != ENOENT) {
                perror("shmget failed");
            }
            continue;
        }

        shm = shmat(shmid, NULL, 0);
        if (shm == (char *) -1) {
            perror("shmat failed");
            continue;
        }

        if (*shm == '\0') {
            shmdt(shm);
            continue;
        }

        ptr = shm;
        while (*ptr != '\0') {
            char *filename = strrchr(ptr, '/');
            filename = filename ? filename + 1 : ptr;

            snprintf(filepath, sizeof(filepath), "database/%s", filename);

            if (strstr(filename, ".csv")) {
                if (strstr(filename, "trashcan")) {
                    strcpy(type, "Trash Can");
                } else if (strstr(filename, "parkinglot")) {
                    strcpy(type, "Parking Lot");
                } else {
                    fprintf(stderr, "File type not recognized: %s\n", filename);
                    ptr += strlen(ptr) + 1;
                    continue;
                }

                file = fopen(filepath, "r");
                if (!file) {
                    perror("Failed to open file");
                    ptr += strlen(ptr) + 1;
                    continue;
                }

                fgets(line, sizeof(line), file); // Skip header
                while (fgets(line, sizeof(line), file)) {
                    char *name = strtok(line, ",");
                    char *rating = strtok(NULL, ",");

                    if (rating) {
                        float current_rating = atof(rating);
                        if (current_rating > max_rating) {
                            max_rating = current_rating;
                            entry_count = 0; // Reset the count
                        }
                        if (current_rating == max_rating) {
                            if (entry_count < MAX_ENTRIES) {
                                strncpy(entries[entry_count].name, name, sizeof(entries[entry_count].name) - 1);
                                entries[entry_count].rating = current_rating;
                                entry_count++;
                            }
                        }
                    }
                }

                fclose(file);

                printf("Type: %s\n", type);
                printf("File: %s\n", filename);
                printf("---------------------------------\n");
                for (int i = 0; i < entry_count; i++) {
                    printf("Name: %s\n", entries[i].name);
                    printf("Rating: %.1f\n", entries[i].rating);
                }
                printf("\n");

                max_rating = 0.0;
                entry_count = 0; // Reset for the next file
            }

            ptr += strlen(ptr) + 1;
        }

        shmdt(shm);
    }

    return 0;
}
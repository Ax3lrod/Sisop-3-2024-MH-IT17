# LAPRES SOAL SHIFT MODUL 3

## Anggota Kelompok

1. Aryasatya Alaauddin 5027231082 (Mengerjakan soal 1 dan 4)
2. Diandra Naufal Abror 5027231004 (Mengerjakan soal 2)
3. Muhamad Rizq Taufan 5027231021 (Mengerjakan soal 3)

## NOMOR 1

Pada zaman dahulu pada galaksi yang jauh-jauh sekali, hiduplah seorang Stelle. Stelle adalah seseorang yang sangat tertarik dengan Tempat Sampah dan Parkiran Luar Angkasa. Stelle memulai untuk mencari Tempat Sampah dan Parkiran yang terbaik di angkasa. Dia memerlukan program untuk bisa secara otomatis mengetahui Tempat Sampah dan Parkiran dengan rating terbaik di angkasa. Programnya berbentuk microservice sebagai berikut:
a. Dalam auth.c pastikan file yang masuk ke folder new-entry adalah file csv dan berakhiran  trashcan dan parkinglot. Jika bukan, program akan secara langsung akan delete file tersebut. 
Contoh dari nama file yang akan diautentikasi:
* belobog_trashcan.csv
* osaka_parkinglot.csv
b. Format data (Kolom)  yang berada dalam file csv adalah seperti berikut:
![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/38a3cb01-142c-43de-b29a-84f971ae7b1e)
atau
![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/4e631450-9b71-430e-aa11-18232df9e2ca)
c. File csv yang lolos tahap autentikasi akan dikirim ke shared memory. 
d. Dalam rate.c, proses akan mengambil data csv dari shared memory dan akan memberikan output Tempat Sampah dan Parkiran dengan Rating Terbaik dari data tersebut.
![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/da6044c1-c087-4718-b061-b372f7a345ac)
e. Pada db.c, proses bisa memindahkan file dari new-data ke folder microservices/database, WAJIB MENGGUNAKAN SHARED MEMORY.
f. Log semua file yang masuk ke folder microservices/database ke dalam file db.log dengan contoh format sebagai berikut:
[DD/MM/YY hh:mm:ss] [type] [filename]
ex : `[07/04/2024 08:34:50] [Trash Can] [belobog_trashcan.csv]

Contoh direktori awal:
.
├── auth.c
├── microservices
│   ├── database
│   │   └── db.log
│   ├── db.c
│   └── rate.c
└── new-data
    ├── belobog_trashcan.csv
    ├── ikn.csv
    └── osaka_parkinglot.csv

Contoh direktori akhir setelah dijalankan auth.c dan db.c:
.
├── auth.c
├── microservices
│   ├── database
│   │   ├── belobog_trashcan.csv
│   │   ├── db.log
│   │   └── osaka_parkinglot.csv
│   ├── db.c
│   └── rate.c
└── new-data

**Solusi**
**Oleh Aryasatya Alaauddin (5027231082)**.

Untuk menyelesaikan soal ini, hal pertama yang saya lakukan adalah membuat struktur folder seperti yang diinginkan soal
Saya membuat folder khusus untuk soal_1, di dalamnya saya membuat folder new-data (./soal_1/new-data) dan microservices (./soal_1/microservices.
Di dalam folder microservices saya membuat folder database (./soal_1/microservices/database) dan membuat file kosong bernama db.log untuk menyimpan log dari db.c.
Setelah struktur folder sudah saya buat seperti di soal, saya kembali ke ./soal_1 dan membuat kode auth.c disana.

A. auth.c

auth.c bertujuan untuk memilah semua file yang ada di folder ./soal_1/new-data dan memindahkan file yang terautentikasi ke shared memory.
Untuk mempermudah pemrosesan file dari shared memory kedepannya, saya menetapkan fixed key untuk shared memory pertama yang terbuat dan kemudian program
akan melakukan increment terhadap key tersebut hingga semua file yang terotentikasi sudah dipindahkan semua.

```
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
```
Metode yang saya gunakan adalah sebagai berikut:
1. program membuka folder new-data dan melakukan looping untuk melihat semua file di dalamnya.
2. kemudian program akan memeriksa apakah file merupakan file reguler (bukan folder etc.)
3. kemudian program akan memeriksa apakah file merupakan file .csv yang berakhiran dengan string trashcan atau parkinglot (berakhiran trashcan.csv atau parkinglot.csv)
4. jika benar maka program akan mencatat full path dari file tersebut dan memasukkannya ke fungsi untuk memindahkan ke shared memory jika tidak program akan menghapus filenya.
5. untuk setiap pemanggilan fungsi, program akan melakukan increment terhadap unique_key untuk mengakses shared memory.

```
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
```

Di atas adalah fungsi yang bertugas mengirim full path dari file terpilih ke shared memory.

B. rate.c

rate.c bertujuan untuk membaca file .csv yang sudah terpilih dan menampilkan output berupa nama trashcan dan parkinglot dengan rating terbaik beserta ratingnya.

```
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
            file = fopen(filepath, "r");
            
            if (!file) { // If file not found in database/
                snprintf(filepath, sizeof(filepath), "/home/ax3lrod/sisop/modul3/soal_1/new-data/%s", filename);
                file = fopen(filepath, "r");
            }

            if (!file) {
                fprintf(stderr, "Failed to open file: %s\n", filepath);
                ptr += strlen(ptr) + 1;
                continue;
            }

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
```
Berikut adalah ringkasan dari metode yang saya gunakan.
1. program akan mengakses shared memory menggunakan fixed unique_key awal yang sama dengan yang digunakan oleh auth.c
2. kemudian program akan mendapatkan informasi full path dari semua file .csv yang ingin ditampilkan.
3. kemudian program akan mengambil nama dari file, mencarinya di folder ./soal_1/new-data atau folder ./soal_1/microservices/database untuk mencari keberadaan file
4. jika file ditemukan maka dia akan menampilkan informasi dengan format yang telah ditentukan.
5. untuk setiap file yang ditampilkan, program akan melakukan inkremen ke unique_key hingga dia membaca NULL (semua file shared memory sudah terbaca).

C. db.c

db.c bertujuan untuk membaca shared memory dan memindahkan file terpilih ke folder ./soal_1/microservices/database dan menuliskan log untuk setiap perpindahan.

```
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
```

Berikut adalah ringkasan dari metode yang saya gunakan untuk db.c.
1. Pertama program akan mengakses shared memory menggunakan fixed unique_key awal yang sama dengan auth.c
2. Program mendapatkan informasi full path dari file-file yang terpilih dan memindahkannya ke folder ./soal_1/microserices/database
3. Setiap perpindahan dia akan mencatatnya di log, dan untuk memastikan tipe filenya (apakah dia trashcan atau parkinglot) program menggunakan fungsi berikut:
```
const char* getFileType(const char* filename) {
    if (strstr(filename, "trashcan")) {
        return "Trash Can";
    } else if (strstr(filename, "parkinglot")) {
        return "Parking Lot";
    } else {
        return "Unknown";
    }
}
```
4. Untuk setiap perpindahan file, program akan melakukan increment pada unique_key hingga semua file shared memory dipindahkan.

**Sample Output**

Untuk test-case saya menggunakan file-file yang sama dengan soal, yaitu belobog_trashcan.csv, osaka_parkinglot.csv, dan ikn.csv

isi dari belobog_trashcan.csv versi saya:

name, rating
Qlipoth Fort, 9.7
Everwinter Hill, 9.7
Rivet Town, 6.0

isi dari osaka_parkinglot.csv versi saya:

name, rating
Dotonbori, 8.6
Kiseki, 9.7
Osaka Castle, 8.5

untuk ikn.csv saya kosongkan.

A. auth.c
![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/1ea1a39e-5c0b-4021-a6df-f86dcf051bec)
Karena ikn.csv tidak memiliki string trashcan ataupun parkinglot di namanya maka file akan dihapus oleh auth.c.
Command "ipcs -m" menunjukkan ada 2 shared memory untuk 2 file yang berhasil dipindahkan (belobog_trashcan.csv dan osaka_parkinglot.csv).

B. rate.c
![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/8b240c50-a94b-4692-bcc4-6695fa7dc1a0)
Karena belobog_trashcan.csv memiliki 2 nama trashcan dengan rating max maka program akan menampilkan keduanya.

C. db.c
![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/82a122a3-127f-4294-b1f6-9b53a86b6cc0)
Program memindahkan file terpilih dari ./soal_1/new-data ke ./soal_1/microservices/database dan mencatat perpindahan di db.log.







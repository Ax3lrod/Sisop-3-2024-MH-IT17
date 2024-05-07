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
**Oleh Aryasatya Alaauddin (5027231082)**

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

## NOMOR 4

Lewis Hamilton 🏎 seorang wibu akut dan sering melewatkan beberapa episode yang karena sibuk menjadi asisten. Maka dari itu dia membuat list anime yang sedang ongoing (biar tidak lupa) dan yang completed (anime lama tapi pengen ditonton aja). Tapi setelah Lewis pikir-pikir malah kepikiran untuk membuat list anime. Jadi dia membuat file (harap diunduh) dan ingin menggunakan socket yang baru saja dipelajarinya untuk melakukan CRUD pada list animenya. 
a. Client dan server terhubung melalui socket. 
b. client.c di dalam folder client dan server.c di dalam folder server
c. Client berfungsi sebagai pengirim pesan dan dapat menerima pesan dari server.
d. Server berfungsi sebagai penerima pesan dari client dan hanya menampilkan pesan perintah client saja.  
e. fServer digunakan untuk membaca myanimelist.csv. Dimana terjadi pengiriman data antara client ke server dan server ke client.
* Menampilkan seluruh judul
* Menampilkan judul berdasarkan genre
* Menampilkan judul berdasarkan hari
* Menampilkan status berdasarkan berdasarkan judul
* Menambahkan anime ke dalam file myanimelist.csv
* Melakukan edit anime berdasarkan judul
* Melakukan delete berdasarkan judul
* Selain command yang diberikan akan menampilkan tulisan “Invalid Command”
f. Karena Lewis juga ingin track anime yang ditambah, diubah, dan dihapus. Maka dia membuat server dapat mencatat anime yang dihapus dalam sebuah log yang diberi nama change.log.
* Format: [date] [type] [massage]
* Type: ADD, EDIT, DEL
* Ex:
[29/03/24] [ADD] Kanokari ditambahkan.
[29/03/24] [EDIT] Kamis,Comedy,Kanokari,completed diubah menjadi Jumat,Action,Naruto,completed.
[29/03/24] [DEL] Naruto berhasil dihapus.

g. Koneksi antara client dan server tidak akan terputus jika ada kesalahan input dari client, cuma terputus jika user mengirim pesan “exit”. Program exit dilakukan pada sisi client.
h. Hasil akhir:
soal_4/
    ├── change.log
    ├── client/
    │   └── client.c
    ├── myanimelist.csv
    └── server/
        └── server.c

Cara kerja

![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/7198851a-7ef7-48fb-b932-404f8f49aaf9)

**Solusi**
**Oleh Aryasatya Alaauddin (5027231082)**

Untuk menyelesaikan soal_4, pertama saya membuat folder khusus untuk soal ini bernama soal_4 dan di dalamnya saya membuat folder client (./soal_4/client) untuk menyimpan kode client.c dan membuat folder server (./soal_4/server) untuk menyimpan kode server.c.
Kemudian saya melanjutkan dengan membuat kode client.c dan server.c

A. client.c

client.c bertujuan untuk menerima input dari user kemudian mengirimkan inputnya ke server.c yang sedang berjalan.

```
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
```
Berikut adalah ringkasan dari metode/logika untuk client.c

1. Program dimulai dengan membuat soket menggunakan pemanggilan fungsi socket. Ini membuat soket TCP IPv4.
2. Kemudian, program menyiapkan struktur alamat server (serv_addr) dengan alamat IP server (127.0.0.1 untuk localhost) dan nomor port (PORT).
3. Klien terhubung ke server menggunakan pemanggilan fungsi connect dengan soket yang dibuat dan alamat server.
4. Setelah terhubung, program memasuki loop di mana meminta input dari pengguna dan mengirim input ke server menggunakan pemanggilan fungsi send.
5. Sebelum mengirim input, program memeriksa apakah input cocok dengan string "exit". Jika ya, program mengirim input untuk mematikan server.c dan keluar dari loop dan mematikan client.c.
6. Jika tidak, program memeriksa apakah input merupakan input valid (berisi command tampilan, hari, genre, add, edit, delete) menggunakan fungsi strstr. Jika tidak, program mencetak pesan "Perintah Tidak Valid" dan melanjutkan ke iterasi berikutnya dari loop. Jika ya, program mengirim input ke server menggunakan pemanggilan fungsi send.
7. Setelah mengirim input, program menunggu respons dari server dengan berulang kali memanggil fungsi read dalam loop hingga seluruh respons diterima (hingga BUFFER_SIZE - 1 byte).
8. Respons yang diterima kemudian dicetak ke konsol.
9. Setelah keluar dari loop utama, program menutup koneksi soket menggunakan pemanggilan fungsi close.

B. server.c

server.c bertujuan untuk mengunduh file myanimelist.csv, menerima dan memproses input dari client.c, melakukan operasi CRUD pada myanimelist.csv sesuai input yang diberikan, dan mengirimkan output ke client.c. Selain itu server.c juga perlu membuat dan mencatat di change.log setelah melakukan operasi edit, add, dan delete pada myanimelist.csv.

```
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
                if (strcmp(buffer, "exit") == 0) {
                    printf("Received exit signal from client. Shutting down server...\n");
                    break; // Keluar dari loop untuk menutup server
                }
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
        break;  // Exit the outer loop to shut down the server
    }

    close(server_fd);
    return 0;
}
```
Berikut adalah ringkasan dari metode/logika yang saya gunakan untuk server.c
1. Program mengunduh myanimelist.csv dan meletakkannya di folder pengerjaan soal_4 (./soal_4)
2. Program membuat socket server dan menunggu koneksi dari klien menggunakan fungsi accept.
3. Ketika ada koneksi klien yang masuk, program memasuki loop bersarang di mana membaca input dari klien menggunakan fungsi read.
```
while (1) {
    int read_size = read(clientSocket, buffer, BUFFER_SIZE);
    if (read_size > 0) {
        buffer[read_size] = '\0';  // Null-terminate the buffer
        printf("Command received: %s\n", buffer);
        // Proses input selanjutnya
    }
    // ...
}
```
4. Input yang diterima dari klien disimpan dalam buffer buffer. Jika input yang diterima adalah string "exit", program akan keluar dari loop bersarang dan menutup koneksi dengan klien.
5. Jika input yang diterima bukan "exit", program akan memanggil fungsi handleCommand dengan argumen socket klien dan input yang diterima (dalam buffer buffer).
```
handleCommand(clientSocket, buffer);
```
6. Dalam fungsi handleCommand, program memisahkan input menjadi token-token menggunakan fungsi strtok dengan pemisah spasi.
```
char* token = strtok(command, " ");
```
7. Token pertama diidentifikasi sebagai perintah, dan program melakukan pengecekan terhadap token tersebut untuk menentukan perintah apa yang diterima.
8. Jika perintah yang diterima adalah "tampilkan", program memanggil fungsi displayAnime dengan filter "all" untuk menampilkan seluruh daftar anime.
9. Jika perintah yang diterima adalah "hari", program mengambil argumen setelah "hari" (yaitu nama hari) dan memanggil fungsi displayAnime dengan filter "day" dan argumen nama hari.
10. Jika perintah yang diterima adalah "genre", program mengambil argumen setelah "genre" (yaitu nama genre) dan memanggil fungsi displayAnime dengan filter "genre" dan argumen nama genre.
11. Jika perintah yang diterima adalah "status", program mengambil argumen setelah "status" (yaitu judul anime) dan memanggil fungsi getStatus dengan argumen judul anime.
12. Jika perintah yang diterima adalah "add", program mengambil argumen-argumen setelah "add" (hari, genre, judul, status) dan memanggil fungsi addAnime dengan argumen-argumen tersebut.
13. Jika perintah yang diterima adalah "edit", program mengambil argumen-argumen setelah "edit" (judul lama, hari baru, genre baru, judul baru, status baru) dan memanggil fungsi editAnime dengan argumen-argumen tersebut.
14. Jika perintah yang diterima adalah "delete", program mengambil argumen setelah "delete" (yaitu judul anime) dan memanggil fungsi deleteAnime dengan argumen judul anime.
15. Jika perintah yang diterima tidak valid, program akan mengirimkan pesan "Invalid Command" ke klien menggunakan fungsi send.
16. Jika perintah yang diterima adalah "exit", program akan keluar dari loop dan akan berhenti.
17. Setelah memproses perintah, program kembali ke loop bersarang dan menunggu input selanjutnya dari klien.

**Sample Output**

1. Pertama jalankan server.c terlebih dahulu kemudian jalankan client.c
![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/ca3f305b-4ba1-402d-9684-b3016b387070)

2. Tes command "tampilkan"
![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/8a574063-55e9-4d51-aee6-c269c458ac65)

3. Tes command "hari x"
![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/de040e57-6af8-417d-907a-94f03e544801)

4. Tes command "genre x"
![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/2a5c2a01-1c3a-4068-877d-d0d88ab8b786)

5. Tes command "status x"
![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/9842b17c-fd1c-4ea3-aeb5-a99ffac28f08)

6. Tes command "add x"
![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/c1edcca8-27a6-48df-aa27-aaad0be61623)

7. Tes command "edit x"
![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/24b41780-0f17-494b-be7d-1185c4406616)

8. Tes command "delete x"
![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/241cd141-c2fb-465a-a032-55167e8de925)

9. Tes invalid command
![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/4a742971-f40b-4f45-be2b-dd1ff17b2c75)

10. Tes command "exit"
![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/8a9467f9-5a66-4b88-ad90-16e4d3aedb6f)

## REVISI

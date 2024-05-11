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

## NOMOR 2
`Diandra Naufal Abror 5027231004`
Max Verstappen seorang pembalap F1 dan programer memiliki seorang adik bernama Min Verstappen (masih SD) sedang menghadapi tahap paling kelam dalam kehidupan yaitu perkalian matematika, Min meminta bantuan Max untuk membuat kalkulator perkalian sederhana (satu sampai sembilan). Sembari Max nguli dia menyuruh Min untuk belajar perkalian dari web (referensi) agar tidak bergantung pada kalkulator.
(Wajib menerapkan konsep pipes dan fork seperti yang dijelaskan di modul Sisop. Gunakan 2 pipes dengan diagram seperti di modul 3).
- a. Sesuai request dari adiknya Max ingin nama programnya dudududu.c. Sebelum program parent process dan child process, ada input dari user berupa 2 string. Contoh input: tiga tujuh. 
- b. Pada parent process, program akan mengubah input menjadi angka dan melakukan perkalian dari angka yang telah diubah. Contoh: tiga tujuh menjadi 21. 
- c. Pada child process, program akan mengubah hasil angka yang telah diperoleh dari parent process menjadi kalimat. Contoh: `21` menjadi “dua puluh satu”.
- d. Max ingin membuat program kalkulator dapat melakukan penjumlahan, pengurangan, dan pembagian, maka pada program buatlah argumen untuk menjalankan program : 

perkalian	: ./kalkulator -kali

penjumlahan	: ./kalkulator -tambah

pengurangan	: ./kalkulator -kurang

pembagian	: ./kalkulator -bagi

Beberapa hari kemudian karena Max terpaksa keluar dari Australian Grand Prix 2024 membuat Max tidak bersemangat untuk melanjutkan programnya sehingga kalkulator yang dibuatnya cuma menampilkan hasil positif jika bernilai negatif maka program akan print “ERROR” serta cuma menampilkan bilangan bulat jika ada bilangan desimal maka dibulatkan ke bawah.
- e. Setelah diberi semangat, Max pun melanjutkan programnya dia ingin (pada child process) kalimat akan di print dengan contoh format : 

perkalian	: “hasil perkalian tiga dan tujuh adalah dua puluh satu.”

penjumlahan	: “hasil penjumlahan tiga dan tujuh adalah sepuluh.”

pengurangan	: “hasil pengurangan tujuh dan tiga adalah empat.”

pembagian	: “hasil pembagian tujuh dan tiga adalah dua.”
- f. Max ingin hasil dari setiap perhitungan dicatat dalam sebuah log yang diberi nama histori.log. Pada parent process, lakukan pembuatan file log berdasarkan data yang dikirim dari child process. 

Format: [date] [type] [message]

Type: KALI, TAMBAH, KURANG, BAGI

Ex:

[10/03/24 00:29:47] [KALI] tujuh kali enam sama dengan empat puluh dua.

[10/03/24 00:30:00] [TAMBAH] sembilan tambah sepuluh sama dengan sembilan belas.

[10/03/24 00:30:12] [KURANG] ERROR pada pengurangan.

## Solusi
### Deklarasi _Array_
```
char *satuanAngka[] = {"nol", "satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan"};
char *belasanAngka[] = {"sepuluh", "sebelas", "dua belas", "tiga belas", "empat belas", "lima belas", "enam belas", "tujuh belas", "delapan belas", "sembilan belas"};
```
Array `satuanAngka` dan `belasanAngka` dideklarasi untuk menyimpan representasi teks dari angka yang diperlukan dalam proses konversi angka menjadi teks.

### Fungsi Konversi
```
int konversiKeAngka(char *operan) {
    if (strcmp(operan, "sepuluh") == 0) {
        return 10;
    } else if (strcmp(operan, "sebelas") == 0) {
        return 11;
    } else if (strncmp(operan, "dua", 3) == 0) {
        if (strlen(operan) == 3) {
            return 2;
        } else if (strcmp(operan, "dua belas") == 0) {
            return 12;
        }
    } else if (strncmp(operan, "tiga", 4) == 0) {
        if (strlen(operan) == 4) {
            return 3;
        } else if (strcmp(operan, "tiga belas") == 0) {
            return 13;
        }
    } else if (strncmp(operan, "empat", 5) == 0) {
        if (strlen(operan) == 5) {
            return 4;
        } else if (strcmp(operan, "empat belas") == 0) {
            return 14;
        }
    } else if (strncmp(operan, "lima", 4) == 0) {
        if (strlen(operan) == 4) {
            return 5;
        } else if (strcmp(operan, "lima belas") == 0) {
            return 15;
        }
    } else if (strncmp(operan, "enam", 4) == 0) {
        if (strlen(operan) == 4) {
            return 6;
        } else if (strcmp(operan, "enam belas") == 0) {
            return 16;
        }
    } else if (strncmp(operan, "tujuh", 5) == 0) {
        if (strlen(operan) == 5) {
            return 7;
        } else if (strcmp(operan, "tujuh belas") == 0) {
            return 17;
        }
    } else if (strncmp(operan, "delapan", 7) == 0) {
        if (strlen(operan) == 7) {
            return 8;
        } else if (strcmp(operan, "delapan belas") == 0) {
            return 18;
        }
    } else if (strncmp(operan, "sembilan", 8) == 0) {
        if (strlen(operan) == 8) {
            return 9;
        } else if (strcmp(operan, "sembilan belas") == 0) {
            return 19;
        }
    }

    return 0;
}
```
- Fungsi yang bertugas untuk mengonversi _string_ operan menjadi angka, logika yang digunakan adalah dengan menggunakan perbandingan _string_ dan manipulasi _string_ untuk mengonversi _string_ operan menjadi angka yang sesuai.

### Fungsi Hitung
```
void hitungOperasi(char *operasi, char *operan1, char *operan2, char *hasilTeks, int *hasilAngka) {
    int op1 = konversiKeAngka(operan1);
    int op2 = konversiKeAngka(operan2);

    if (strcmp(operasi, "-kali") == 0) {
        *hasilAngka = op1 * op2;
    } else if (strcmp(operasi, "-tambah") == 0) {
        *hasilAngka = op1 + op2;
    } else if (strcmp(operasi, "-kurang") == 0) {
        *hasilAngka = op1 - op2;
    } else if (strcmp(operasi, "-bagi") == 0) {
        *hasilAngka = op1 / op2;
    }

    if (*hasilAngka < 0) {
        strcpy(hasilTeks, "ERROR");
    } else if (*hasilAngka < 10) {
        strcpy(hasilTeks, satuanAngka[*hasilAngka]);
    } else if (*hasilAngka < 20) {
        strcpy(hasilTeks, belasanAngka[*hasilAngka - 10]);
    } else if (*hasilAngka < 100) {
        char puluhan[10], satuan[10];
        int angkaPuluhan = *hasilAngka / 10;
        int angkaSatuan = *hasilAngka % 10;
        strcpy(puluhan, satuanAngka[angkaPuluhan]);
        strcpy(satuan, satuanAngka[angkaSatuan]);
        sprintf(hasilTeks, "%s puluh %s", puluhan, satuan);
    } else {
        strcpy(hasilTeks, "ERROR");
    }
}
```
- Memiliki fungsi vital untuk melakukan operasi matematika seperti penjumlahan, pengurangan, perkalian, atau pembagian. Logika fungsi `konversiKeAngka` akan mengonversi operan menjadi angka, lalu mengeksekusi operasi yang dikehendaki. Hasil angka tersebut disimpan dalam variabel `hasilAngka`, sedangkan untuk hasil teksnya disimpan dalam `hasilTeks` menggunakan array `satuanAngka` dan `belasanAngka` sebagai referensi untuk mengonversi angka menjadi teks.

### Fungsi Utama
```
int main(int argc, char *argv[]) {
    char *jenisOperasi = argv[1];
    char operan1[100];
    char operan2[100];
    int pipeInduk[2];
    int pipeAnak[2];
    pid_t prosesAnak;

    time_t waktuSekarang;
    struct tm *infoWaktu;
    time(&waktuSekarang);
    infoWaktu = localtime(&waktuSekarang);
// Segmen Penerapan Pipes dan Fork
    pipe(pipeInduk);
    pipe(pipeAnak);

    scanf("%s", operan1);
    scanf("%s", operan2);

    prosesAnak = fork();
// Segmen Proses Induk
    if (prosesAnak > 0) {
        close(pipeInduk[0]);
        int hasilAngka;
        char hasilTeks[100];

        hitungOperasi(jenisOperasi, operan1, operan2, hasilTeks, &hasilAngka);
        write(pipeInduk[1], &hasilAngka, sizeof(int));
        close(pipeInduk[1]);

        wait(NULL);

        close(pipeAnak[1]);
        read(pipeAnak[0], hasilTeks, sizeof(hasilTeks));
// Segmen File Log
        FILE *filelog;
        filelog = fopen("histori.log", "a");

        fprintf(filelog, "[%02d/%02d/%02d %02d:%02d:%02d] [%s] %s %s %s sama dengan %s.\n",
                infoWaktu->tm_mday, infoWaktu->tm_mon + 1, infoWaktu->tm_year - 100,
                infoWaktu->tm_hour, infoWaktu->tm_min, infoWaktu->tm_sec,
                strcmp(jenisOperasi, "-kali") == 0 ? "KALI" : strcmp(jenisOperasi, "-tambah") == 0 ? "TAMBAH" : strcmp(jenisOperasi, "-kurang") == 0 ? "KURANG" : "BAGI",
                operan1, strcmp(jenisOperasi, "-kali") == 0 ? "kali" : strcmp(jenisOperasi, "-tambah") == 0 ? "tambah" : strcmp(jenisOperasi, "-kurang") == 0 ? "kurang" : "bagi",
                operan2, hasilTeks);

        fclose(filelog);
        close(pipeAnak[0]);
    }
// Segmen Proses Anak
        else {
        close(pipeInduk[1]);
        int hasilAngka;
        read(pipeInduk[0], &hasilAngka, sizeof(int));
        char hasilTeks[100];
        hitungOperasi(jenisOperasi, operan1, operan2, hasilTeks, &hasilAngka);
        close(pipeInduk[0]);
        close(pipeAnak[0]);

        printf("hasil %s %s dan %s adalah %s.\n",
               strcmp(jenisOperasi, "-kali") == 0 ? "perkalian" : strcmp(jenisOperasi, "-tambah") == 0 ? "penjumlahan" : strcmp(jenisOperasi, "-kurang") == 0 ? "pengurangan" : "pembagian",
               operan1, operan2, hasilTeks);

        write(pipeAnak[1], hasilTeks, sizeof(hasilTeks));
        close(pipeAnak[1]);
    }

    return 0;
}
```
- Fungsi Utama, secara garis beras berisi akumulasi logika yang mengeksekusi keseluruhan pproses. Berisi logika yang memroses _input user_, membuat proses anak dengan `fork`, melakukan komunikasi antar proses induk dan anak melalui `pipes`, eksekusi operasi matematika pada operan, konversi hasil menjadi teks, dan mencatat riwayat operasi ke dalam file log.
- Segmen Penerapan Pipes dan Fork. Pipes `pipeInduk` dan `pipeAnak` dibuat untuk komunikasi dua arah, sedangkan fungsi `fork()` digunakan untuk membuat proses anak yang berjalan independen dari proses induk. Kode di dalam blok `if` dijalankan proses induk, sedangkan kode blok `else` dijalankan proses anak.
- Segmen Proses Induk, bertugas untuk melakukan operasi matematika operan dengan memanggil fungsi `hitungOperasi`. Hasilnya akan dikirim ke proses anak melalui `pipeInduk` menggunakan `write`. Lalu proses induk akan menunggu proses anak selesai dengan `wait(NULL)`. Setelah proses anak selesai, proses induk menerima output text melalui `pipeAnak`. Terakhir adalah proses pencatatan riwayat hasil ke dalam file histori.log menggunakan format yang ditetapkan.
- Segmen Proses Anak, memproses output angka dari proses induk melalui `pipeInduk` dan mengonversikannya menjadi teks dengan fungsi `hitungOperasi`. Proses anak akan mencetak output text dengan format yang ditentukan, lalu mengirimkannya lagi ke proses induk melalui `pipeAnak`.
- Segmen File Log, berada di dalam proses induk. Berguna untuk menuliskan hasil operasi ke dalam file histori.log. Caranya adalah dengan membuka file log dengan `fopen`. Kemudian `fprintf` untuk menuliskan output operasi dengan format yang ditetapkan, dengan contoh yaitu "[10/03/24 00:29:47] [KALI] tujuh kali enam sama dengan empat puluh dua.". Terakhir adalah menutup file log dengan `fclose`.

### Output
![Ubuntu 64-bit - VMware Workstation 09_05_2024 14_27_50](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/151877527/15042892-2281-4f87-b257-91807c64d44d)

## NOMOR 3
Shall Leglerg🥶 dan Carloss Signs 😎 adalah seorang pembalap F1 untuk tim Ferrari 🥵. Mobil F1 memiliki banyak pengaturan, seperti penghematan ERS, Fuel, Tire Wear dan lainnya. Pada minggu ini ada race di sirkuit Silverstone. Malangnya, seluruh tim Ferrari diracun oleh Super Max Max pada hari sabtu sehingga seluruh kru tim Ferrari tidak bisa membantu Shall Leglerg🥶 dan Carloss Signs 😎 dalam race. Namun, kru Ferrari telah menyiapkan program yang bisa membantu mereka dalam menyelesaikan race secara optimal. Program yang dibuat bisa mengatur pengaturan - pengaturan dalam mobil F1 yang digunakan dalam balapan. Programnya ber ketentuan sebagai berikut:
a. Pada program actions.c, program akan berisi function function yang bisa di call oleh paddock.c
b. Action berisikan sebagai berikut:
    - Gap [Jarak dengan driver di depan (float)]: Jika Jarak antara Driver dengan Musuh di depan adalah < 3.5s maka return Gogogo, jika jarak > 3.5s dan 10s return Push, dan jika jarak > 10s maka return Stay out of trouble.
    - Fuel [Sisa Bensin% (string/int/float)]: Jika bensin lebih dari 80% maka return Push Push Push, jika bensin di antara 50% dan 80% maka return You can go, dan jika bensin kurang dari 50% return Conserve Fuel.
    - Tire [Sisa Ban (int)]: Jika pemakaian ban lebih dari 80 maka return Go Push Go Push, jika pemakaian ban diantara 50 dan 80 return Good Tire Wear, jika pemakaian di antara 30 dan 50 return Conserve Your Tire, dan jika pemakaian ban kurang dari 30         maka return Box Box Box.
    - Tire Change [Tipe ban saat ini (string)]: Jika tipe ban adalah Soft return Mediums Ready, dan jika tipe ban Medium return Box for Softs.

		Contoh:
		[Driver] : [Fuel] [55%]
		[Paddock]: [You can go]

c. Pada paddock.c program berjalan secara daemon di background, bisa terhubung dengan driver.c melalui socket RPC.
d. Program paddock.c dapat call function yang berada di dalam actions.c.
e. Program paddock.c tidak keluar/terminate saat terjadi error dan akan log semua percakapan antara paddock.c dan driver.c di dalam file race.log

Format log:
    [Source] [DD/MM/YY hh:mm:ss]: [Command] [Additional-info]
    ex :
    [Driver] [07/04/2024 08:34:50]: [Fuel] [55%]
    [Paddock] [07/04/2024 08:34:51]: [Fuel] [You can go]

f. Program driver.c bisa terhubung dengan paddock.c dan bisa mengirimkan pesan dan menerima pesan serta menampilan pesan tersebut dari paddock.c sesuai dengan perintah atau function call yang diberikan.
g. Jika bisa digunakan antar device/os (non local) akan diberi nilai tambahan.
h. untuk mengaktifkan RPC call dari driver.c, bisa digunakan in-program CLI atau Argv (bebas) yang penting bisa send command seperti poin B dan menampilkan balasan dari paddock.c
    ex:
    Argv: 
        ./driver -c Fuel -i 55% 
    in-program CLI:
        Command: Fuel
    Info: 55%

Contoh direktori 😶‍🌫️:
    .		.
    ├── client
    │   └── driver.c
    └── server
    ├── actions.c
    ├── paddock.c
    └── race.log
## Solusi
### actions.c
```
#include <stdio.h>
#include <string.h>

char* Gap(float distance) {
    if (distance < 3.5)
        return "Gogogo";
    else if (distance >= 3.5 && distance <= 10)
        return "Push";
    else
        return "Stay out of trouble";
}

char* Fuel(int fuel) {
    if (fuel > 80)
        return "Push Push Push";
    else if (fuel >= 50 && fuel <= 80)
        return "You can go";
    else
        return "Conserve Fuel";
}

char* Tire(int tire) {
    if (tire > 80)
        return "Go Push Go Push";
    else if (tire >= 50 && tire < 80)
        return "Good Tire Wear";
    else if (tire >= 30 && tire < 50)
        return "Conserve Your Tire";
    else
        return "Box Box Box";
}

char* TireChange(char* currentTire) {
    if (strcmp(currentTire, "Soft") == 0)
        return "Mediums Ready";
    else if (strcmp(currentTire, "Medium") == 0)
        return "Box for Softs";
    else
        return "Invalid tire type";
}

```
Ini adalah file yang berisi implementasi dari fungsi-fungsi yang diperlukan untuk mengatur pengaturan mobil F1 sesuai dengan spesifikasi yang diberikan. Setiap fungsi dalam file ini menerima parameter yang sesuai dengan kebutuhan (misalnya, 	jarak, tingkat bensin, pemakaian ban) dan mengembalikan string yang merepresentasikan keputusan atau tindakan yang diambil berdasarkan nilai parameter tersebut.

Gap(float distance): Fungsi ini menerima jarak antara mobil dengan mobil di depannya sebagai parameter. Berdasarkan jarak yang diterima, fungsi ini akan mengembalikan tindakan yang sesuai, seperti "Gogogo" jika jarak kurang dari 3.5 detik, "Push" jika jarak antara 3.5 dan 10 detik, dan "Stay out of trouble" jika jarak lebih dari 10 detik.

Fuel(int fuel): Fungsi ini menerima tingkat bensin mobil sebagai parameter. Berdasarkan tingkat bensin yang diterima, fungsi ini akan mengembalikan tindakan yang sesuai, seperti "Push Push Push" jika bensin lebih dari 80%, "You can go" jika bensin di antara 50% dan 80%, dan "Conserve Fuel" jika bensin kurang dari 50%.

Tire(int tire): Fungsi ini menerima tingkat pemakaian ban mobil sebagai parameter. Berdasarkan pemakaian ban yang diterima, fungsi ini akan mengembalikan tindakan yang sesuai, seperti "Go Push Go Push" jika pemakaian ban lebih dari 80%, "Good Tire Wear" jika pemakaian di antara 50 dan 80%, "Conserve Your Tire" jika pemakaian di antara 30 dan 50%, dan "Box Box Box" jika pemakaian kurang dari 30%.

*TireChange(char currentTire)**: Fungsi ini menerima jenis ban yang sedang digunakan sebagai parameter. Berdasarkan jenis ban yang diterima, fungsi ini akan mengembalikan tindakan yang sesuai, seperti "Mediums Ready" jika ban saat ini adalah Soft, dan "Box for Softs" jika ban saat ini adalah Medium.

1. Pengguna menjalankan driver.c dengan memberikan argumen hostname atau alamat IP dari server tempat paddock.c berjalan.
###  driver.c
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/rpc.h>
#include "actions.h"

#define PROGNUM 0x31341712
#define VERSNUM 1

CLIENT *cl;

void call_and_print(const char *function_name, char *(*rpc_function)(CLIENT *, ...), void *arg) {
    char *result;
    result = rpc_function(cl, arg);
    printf("[Paddock] [%s]: [%s]\n", function_name, result);
    free(result);
}

int main(int argc, char *argv[]) {
    char *server;
    if (argc < 2) {
        printf("Usage: %s server_host\n", argv[0]);
        exit(1);
    }
    server = argv[1];

    cl = clnt_create(server, PROGNUM, VERSNUM, "tcp");
    if (cl == NULL) {
        clnt_pcreateerror(server);
        exit(1);
    }

    // Contoh penggunaan RPC untuk Gap
    float distance = 7.2;
    call_and_print("Gap", gap_rpc_1, &distance);

    // Contoh penggunaan RPC untuk Fuel
    int fuel = 65;
    call_and_print("Fuel", fuel_rpc_1, &fuel);

    // Contoh penggunaan RPC untuk Tire
    int tire = 45;
    call_and_print("Tire", tire_rpc_1, &tire);

    // Contoh penggunaan RPC untuk Tire Change
    char currentTire[] = "Soft";
    call_and_print("Tire Change", tire_change_rpc_1, &currentTire);

    clnt_destroy(cl);

    return 0;
}

```
Ini adalah program yang bertindak sebagai antarmuka untuk pengguna. Pengguna dapat menggunakan program ini untuk berkomunikasi dengan paddock.c menggunakan RPC. Program ini akan membuat koneksi dengan paddock.c menggunakan protokol RPC, dan 	kemudian mengirimkan permintaan ke paddock.c sesuai dengan input yang diberikan oleh pengguna. Setiap respon yang diterima dari paddock.c akan ditampilkan kepada pengguna.

call_and_print(const char *function_name, char *(*rpc_function)(CLIENT *, ...), void *arg): Fungsi ini digunakan untuk memanggil fungsi RPC yang sesuai, menangani respons yang diterima, dan menampilkannya kepada pengguna. Fungsi ini menerima nama fungsi RPC, pointer ke fungsi RPC, dan argumen untuk fungsi RPC.

Main function: Pada fungsi main, program ini membuat koneksi dengan paddock.c menggunakan protokol RPC. Kemudian, program ini memanggil fungsi RPC yang sesuai untuk setiap aksi yang ingin dilakukan (seperti memeriksa jarak, tingkat bensin, pemakaian ban, dll.). Setelah menerima respons dari paddock.c, program ini menampilkan respons tersebut kepada pengguna.

2. driver.c membuat koneksi RPC dengan paddock.c.
### paddock.c
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <rpc/rpc.h>
#include "actions.h"

#define PROGNUM 0x31341712
#define VERSNUM 1
#define GAP_PROC 1
#define FUEL_PROC 2
#define TIRE_PROC 3
#define TIRE_CHANGE_PROC 4

void race_log(const char *source, const char *command, const char *info) {
    FILE *log_file = fopen("race.log", "a");
    if (log_file != NULL) {
        time_t current_time;
        time(&current_time);
        fprintf(log_file, "[%s] [%s]: [%s] [%s]\n", source, ctime(&current_time), command, info);
        fclose(log_file);
    }
}

char **gap_rpc_1_svc(float *distance, struct svc_req *rqstp) {
    static char *result;
    result = Gap(*distance);
    race_log("Paddock", "Gap", result);
    return (&result);
}

char **fuel_rpc_1_svc(int *fuel, struct svc_req *rqstp) {
    static char *result;
    result = Fuel(*fuel);
    race_log("Paddock", "Fuel", result);
    return (&result);
}

char **tire_rpc_1_svc(int *tire, struct svc_req *rqstp) {
    static char *result;
    result = Tire(*tire);
    race_log("Paddock", "Tire", result);
    return (&result);
}

char **tire_change_rpc_1_svc(char **currentTire, struct svc_req *rqstp) {
    static char *result;
    result = TireChange(*currentTire);
    race_log("Paddock", "Tire Change", result);
    return (&result);
}

int main() {
    if (freopen("race.log", "a", stdout) == NULL) {
        perror("freopen");
        exit(1);
    }

    registerrpc(PROGNUM, VERSNUM, GAP_PROC, gap_rpc_1_svc, xdr_float, xdr_wrapstring);
    registerrpc(PROGNUM, VERSNUM, FUEL_PROC, fuel_rpc_1_svc, xdr_int, xdr_wrapstring);
    registerrpc(PROGNUM, VERSNUM, TIRE_PROC, tire_rpc_1_svc, xdr_int, xdr_wrapstring);
    registerrpc(PROGNUM, VERSNUM, TIRE_CHANGE_PROC, tire_change_rpc_1_svc, xdr_wrapstring, xdr_wrapstring);

    svc_run();

    return 0;
}
```
Ini adalah program yang berjalan sebagai daemon di background dan bertindak sebagai server RPC untuk menerima permintaan dari driver.c. Program ini akan mendaftarkan fungsi-fungsi RPC yang sesuai dengan kebutuhan (misalnya, gap_rpc_1_svc, 		fuel_rpc_1_svc) yang kemudian akan dipanggil oleh driver.c. Setiap kali fungsi RPC dipanggil, paddock.c akan menjalankan fungsi yang sesuai dari actions.c dan mencatat percakapan ke dalam file race.log.

race_log(const char *source, const char *command, const char *info): Fungsi ini digunakan untuk mencatat percakapan antara paddock.c dan driver.c ke dalam file race.log. Informasi yang dicatat meliputi sumber pesan (Driver atau Paddock), waktu percakapan, perintah atau fungsi yang dipanggil, dan informasi tambahan jika ada.

Gap RPC, Fuel RPC, Tire RPC, TireChange RPC: Setiap fungsi RPC ini menerima permintaan dari driver.c dan memanggil fungsi yang sesuai dari actions.c. Setelah itu, hasilnya dikembalikan kepada driver.c dan percakapan dicatat menggunakan fungsi race_log.

3. Pengguna memasukkan perintah melalui argumen command-line atau interaksi langsung dengan program.
4. driver.c mengirimkan permintaan ke paddock.c melalui RPC.
5. paddock.c menerima permintaan, menjalankan fungsi yang sesuai dari actions.c, dan mengembalikan hasil ke driver.c.
6. driver.c menerima respons dari paddock.c melalui RPC dan menampilkannya kepada pengguna.
7. Langkah 3-6 diulang sampai pengguna memutuskan untuk keluar dari program.

 
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

11. Cek change.log
![image](https://github.com/Ax3lrod/Sisop-3-2024-MH-IT17/assets/150204139/48829deb-0042-413d-84e6-aa677a241178)

## REVISI

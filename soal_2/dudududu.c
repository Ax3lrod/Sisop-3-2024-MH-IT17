#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>

char *satuanAngka[] = {"nol", "satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan"};
char *belasanAngka[] = {"sepuluh", "sebelas", "dua belas", "tiga belas", "empat belas", "lima belas", "enam belas", "tujuh belas", "delapan belas", "sembilan belas"};

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

    pipe(pipeInduk);
    pipe(pipeAnak);

    scanf("%s", operan1);
    scanf("%s", operan2);

    prosesAnak = fork();

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
    } else {
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

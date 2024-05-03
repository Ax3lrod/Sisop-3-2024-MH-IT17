# LAPRES SOAL SHIFT MODUL 3

## Anggota Kelompok

1. Aryasatya Alaauddin 5027231082 (Mengerjakan soal 1 dan 4)
2. Diandra Naufal Abror 5027231004 (Mengerjakan soal 2)
3. Muhamad Rizq Taufan 5027231021 (Mengerjakan soal 3)

## NOMOR 1

## NOMOR 2
`Diandra Naufal Abror 5027231004`
Max Verstappen seorang pembalap F1 dan programer memiliki seorang adik bernama Min Verstappen (masih SD) sedang menghadapi tahap paling kelam dalam kehidupan yaitu perkalian matematika, Min meminta bantuan Max untuk membuat kalkulator perkalian sederhana (satu sampai sembilan). Sembari Max nguli dia menyuruh Min untuk belajar perkalian dari web (referensi) agar tidak bergantung pada kalkulator.

**(Wajib menerapkan konsep pipes dan fork seperti yang dijelaskan di modul Sisop. Gunakan 2 pipes dengan diagram seperti di modul 3)**.

a. Sesuai request dari adiknya Max ingin nama programnya dudududu.c. Sebelum program _parent process_ dan _child process_, ada input dari user berupa 2 string. Contoh input: tiga tujuh. 

b. Pada _parent process_, program akan mengubah input menjadi angka dan melakukan perkalian dari angka yang telah diubah. Contoh: tiga tujuh menjadi 21. 

c. Pada _child process_, program akan mengubah hasil angka yang telah diperoleh dari _parent process_ menjadi kalimat. Contoh: "21" menjadi “dua puluh satu”.

d. Max ingin membuat program kalkulator dapat melakukan penjumlahan, pengurangan, dan pembagian, maka pada program buatlah argumen untuk menjalankan program : 
- perkalian	: ./kalkulator -kali
- penjumlahan	: ./kalkulator -tambah
- pengurangan	: ./kalkulator -kurang
- pembagian	: ./kalkulator -bagi
Beberapa hari kemudian karena Max terpaksa keluar dari Australian Grand Prix 2024 membuat Max tidak bersemangat untuk melanjutkan programnya sehingga kalkulator yang dibuatnya cuma menampilkan hasil positif jika bernilai negatif maka program akan print “ERROR” serta cuma menampilkan bilangan bulat jika ada bilangan desimal maka dibulatkan ke bawah.

e. Setelah diberi semangat, Max pun melanjutkan programnya dia ingin (pada _child process_) kalimat akan di print dengan contoh format : 
- perkalian	: “hasil perkalian tiga dan tujuh adalah dua puluh satu.”
- penjumlahan	: “hasil penjumlahan tiga dan tujuh adalah sepuluh.”
- pengurangan	: “hasil pengurangan tujuh dan tiga adalah empat.”
- pembagian	: “hasil pembagian tujuh dan tiga adalah dua.”

f. Max ingin hasil dari setiap perhitungan dicatat dalam sebuah log yang diberi nama **histori.log**. Pada _parent process_, lakukan pembuatan file log berdasarkan data yang dikirim dari _child process_. 
- **Format**: [date] [type] [message]
- **Type**: KALI, TAMBAH, KURANG, BAGI
- **Ex**:
1. [10/03/24 00:29:47] [KALI] tujuh kali enam sama dengan empat puluh dua.
2. [10/03/24 00:30:00] [TAMBAH] sembilan tambah sepuluh sama dengan sembilan belas.
3. [10/03/24 00:30:12] [KURANG] ERROR pada pengurangan.

## SOLUSI

## NOMOR 3

## NOMOR 4

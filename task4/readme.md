# **TASK-4 Cross Platform File Transfer**

Tugas ini mengimplementasikan sistem **pengiriman file JSON dari PC ke ESP32 Receiver** melalui **dua tahap komunikasi**:

1. **PC → ESP32 Bridge** via **Serial (USB)**
2. **ESP32 Bridge → ESP32 Receiver** via **ESP-NOW**

Tujuan Tugas ini adalah membangun komunikasi **dua arah lintas perangkat** dengan sistem **pemecahan file menjadi potongan (chunk)**, **pengiriman bertahap**, dan **rekonstruksi file JSON** di sisi penerima menggunakan **SPIFFS dan ArduinoJson**.

---

##  **Diagram Alur**

![Diagram](https://raw.githubusercontent.com/virose-its/MODUL-PEMBELAJARAN/refs/heads/main/Programming/PENUGASAN/TASK-4%20Cross%20PLatform%20FIle%20Transfer%20ESP-NOW/assets/diagram_task4.jpeg)
---

## **Struktur Folder**

```
Task4/
│
├── PC/
|   ├── build/
|   ├── serial/
│   ├── CMakeLists.txt
│   ├── main.cpp
│   └── data.json
│
├── Sender/
|   ├── .pio/
|   ├── include/
|   |    ├── README
|   |    └── sender.h
|   ├── lib/
|   ├── src/
|   |    ├── main.cpp
|   |    └── sender.cpp
|   ├── test/
|   ├── .gitignore
|   └── platform.ini
│
└── Receiver/
    ├── .pio/
    ├── include/
    |    ├── README
    |    └── receiver.h
    ├── lib/
    ├── src/
    |    ├── main.cpp
    |    └── receiver.cpp
    ├── test/
    ├── .gitignore
    └── platform.ini
```

---

# **1. PC — File Sender (C++)**

### **Fungsi:**

PC bertugas:

* Membaca file JSON lokal (`data.json`)
* Memecah file menjadi blok berukuran **240 byte**
* Mengirim setiap blok ke ESP32 Bridge melalui **Serial Port**

---

### **CMakeLists.txt**

```cmake
cmake_minimum_required(VERSION 3.15)
project(Task4PC CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task4
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_library(serial_lib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/serial/src/serial.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/serial/src/impl/win.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/serial/src/impl/list_ports/list_ports_win.cc
)

target_include_directories(serial_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/serial/include
)

target_link_libraries(task4 PRIVATE serial_lib)

if (WIN32)
    target_link_libraries(task4 PRIVATE ws2_32)
endif()
```

 **Penjelasan:**

* `project(Task4PC CXX)` → mendefinisikan proyek C++.
* `add_executable(task4 main.cpp)` → membuat executable bernama `task4`.
* `add_library(serial_lib ...)` → menambahkan library statis untuk komunikasi serial.
* `target_link_libraries` → menghubungkan executable dengan library serial.
* `ws2_32` digunakan untuk dukungan socket di Windows.

---

###  **data.json**

```json
{
  "nama": "Muhammad Fahmi Ilmi",
  "jurusan": "Teknik Informatika",
  "umur": 19,
  "deskripsi": "Saya adalah orang yang sangat disiplin dan rendah hati..."
}
```

---

###  **main.cpp**

```cpp
#include <iostream>
#include <fstream>
#include <cstring>
#include <serial/serial.h>

#define UKURAN_CHUNK 240  // ukuran potongan data per paket
```

 Mengimpor library:

* `iostream` → untuk output ke terminal
* `fstream` → untuk baca file
* `serial/serial.h` → library komunikasi serial
* `UKURAN_CHUNK` didefinisikan agar tiap pengiriman tidak melebihi 240 byte.

---

```cpp
int main() {
    const char* nama_file = "../../data.json";

    FILE *file = fopen(nama_file, "rb");
    if (!file) {
        std::cerr << "Gagal buka file\n";
        return 1;
    }
```

 **Penjelasan:**

* Membuka file JSON dalam mode **binary read (rb)**.
* Jika file gagal dibuka, program berhenti.

---

```cpp
    serial::Serial serialESP("/dev/ttyUSB0", 115200, serial::Timeout::simpleTimeout(1000));
    if (!serialESP.isOpen()) {
        std::cerr << "Gagal buka port serial\n";
        return 1;
    }
```

 **Penjelasan:**

* Membuka port serial `/dev/ttyUSB0`
* Baudrate: 115200.
* Timeout 1 detik.
* Validasi: pastikan port benar-benar terbuka.

---

```cpp
    char buffer[UKURAN_CHUNK];
    int id_chunk = 0;

    while (!feof(file)) {
        size_t byte_terbaca = fread(buffer, 1, UKURAN_CHUNK, file);
        if (byte_terbaca > 0) {
            char paket[UKURAN_CHUNK + 1];
            paket[0] = id_chunk;               
            memcpy(paket + 1, buffer, byte_terbaca); 

            serialESP.write((uint8_t*)paket, byte_terbaca + 1);
            id_chunk++;

            std::cout << "Chunk " << id_chunk-1 << " dikirim, size=" << byte_terbaca << "\n";
        }
    }
```

 **Langkah yang dilakukan:**

1. Membaca 240 byte dari file.
2. Menyimpan ID chunk di byte pertama (`paket[0]`).
3. Menyalin data file ke posisi selanjutnya (`paket + 1`).
4. Mengirim seluruh paket (ID + data) ke ESP Bridge.
5. Menampilkan status ke terminal.

---

```cpp
    fclose(file);
    serialESP.close();
    std::cout << "Semua data berhasil dikirim!\n";
    return 0;
}
```

 **Menutup file dan port serial** setelah pengiriman selesai.

---

#  **2. ESP32 Bridge (Sender)**

###  **Fungsi:**

* Menerima data dari PC melalui **Serial USB**
* Meneruskan data melalui **ESP-NOW** ke **Receiver**
* Bertindak sebagai **jembatan (Bridge)** antara wired dan wireless

---

###  **sender.h**

```cpp
#ifndef SENDER_H
#define SENDER_H

#include <Arduino.h>

struct Paket {
    uint8_t id;
    uint8_t panjang;
    char data[240];
};

void kirimESPNow(Paket paket, const uint8_t *alamatPenerima);

#endif
```

 **Penjelasan:**

  * `struct Paket` mendefinisikan format data yang dikirim:
  * `id`: urutan chunk
  * `panjang`: ukuran data aktual
  * `data`: isi data chunk (maks. 240 byte)

---

###  **sender.cpp**

```cpp
#include "sender.h"
#include <esp_now.h>

void kirimESPNow(Paket paket, const uint8_t *alamatPenerima) {
    esp_err_t hasil = esp_now_send(alamatPenerima, (uint8_t *)&paket, sizeof(paket));
    if (hasil != ESP_OK) Serial.println("Gagal kirim ESP-NOW");
}
```

 **Penjelasan:**

* Fungsi ini mengirimkan struct `Paket` ke alamat MAC tujuan melalui ESP-NOW.
* Jika gagal, menampilkan pesan error ke Serial Monitor.

---

###  **main.cpp**

```cpp
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "sender.h"

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) Serial.println("ESP-NOW gagal");

    uint8_t alamatReceiver[] = {0x24,0x6F,0x28,0xAB,0xCD,0xEF};
    esp_now_peer_info_t infoPeer = {};
    memcpy(infoPeer.peer_addr, alamatReceiver, 6);
    infoPeer.channel = 0;
    infoPeer.encrypt = false;
    esp_now_add_peer(&infoPeer);
}
```

 **Penjelasan:**

* Menginisialisasi ESP-NOW.
* Mendaftarkan alamat MAC Receiver agar bisa dikirimi data.

---

```cpp
void loop() {
    if (Serial.available() >= 1) {
        uint8_t id = Serial.read();
        uint8_t buffer[240];
        int panjangData = 0;

        while (Serial.available() && panjangData < 240) {
            buffer[panjangData++] = Serial.read();
        }

        Paket paket;
        paket.id = id;
        paket.panjang = panjangData;
        memcpy(paket.data, buffer, panjangData);

        uint8_t penerima[] = {0x24,0x6F,0x28,0xAB,0xCD,0xEF};
        kirimESPNow(paket, penerima);
    }
}
```

 **Penjelasan Loop:**

1. Cek apakah ada data masuk dari PC.
2. Baca byte pertama sebagai **ID paket**.
3. Baca sisa data hingga 240 byte.
4. Buat struct `Paket` berisi id + panjang + isi.
5. Kirim paket ke Receiver melalui ESP-NOW.

---

#  **3. ESP32 Receiver**

###  **Fungsi:**

* Menerima paket data dari Bridge (ESP-NOW)
* Menyimpan setiap chunk ke buffer
* Menyatukan semua data jadi file `terima.json`
* Menampilkan isi JSON ke Serial Monitor

---

###  **receiver.h**

```cpp
#ifndef RECEIVER_H
#define RECEIVER_H

#include <Arduino.h>

struct Paket {
    uint8_t id;
    uint8_t panjang;
    char data[240];
};

#define MAKS_CHUNK 50

void saatTerimaData(const uint8_t *mac, const uint8_t *dataMasuk, int len);
void simpanDanTampilFile();

bool semuaChunkDiterima();

#endif

```

 **Penjelasan:**

* Menyimpan struktur dan berisi fungsi proses penyimpanan data yang diterima.

---

###  **receiver.cpp**

```cpp
#include "receiver.h"
#include <SPIFFS.h>
#include <esp_now.h>
#include <ArduinoJson.h>

char bufferTerima[MAKS_CHUNK][240];
bool flagTerima[MAKS_CHUNK] = {false};
int totalChunk = 0;
```

 **Penjelasan:**

* `bufferTerima` → menyimpan isi setiap chunk.
* `flagTerima` → menandai chunk mana yang sudah diterima.
* `totalChunk` → menghitung jumlah chunk total.

---

```cpp
void saatTerimaData(const uint8_t *mac, const uint8_t *dataMasuk, int len) {
    Paket paket;
    memcpy(&paket, dataMasuk, sizeof(paket));
    memcpy(bufferTerima[paket.id], paket.data, paket.panjang);
    flagTerima[paket.id] = true;
    if (paket.id + 1 > totalChunk) totalChunk = paket.id + 1;
}
```

 **Penjelasan:**

* Callback ESP-NOW saat menerima data.
* Menyalin data dari `dataMasuk` ke buffer berdasarkan ID chunk.
* Menandai chunk sudah diterima.

---
```cpp
bool semuaChunkDiterima() {
    if (totalChunk == 0) return false;
    for (int i = 0; i < totalChunk; i++)
        if (!flagTerima[i]) return false;
    return true;
}
```
* fungsi untuk megecek apakah semua chunk atau potongan sudah diterima

```cpp
void simpanDanTampilFile() {
    File fileTulis = SPIFFS.open("/terima.json","wb");
    for (int i=0;i<totalChunk;i++) {
        fileTulis.write((uint8_t*)bufferTerima[i], strlen(bufferTerima[i]));
    }
    fileTulis.close();
```

 **Langkah:**

1. Membuka file `terima.json` di SPIFFS.
2. Menulis seluruh chunk ke file.

---

```cpp
    File fileBaca = SPIFFS.open("/terima.json","r");
    if (!fileBaca) {
        Serial.println("Gagal buka file JSON");
        return;
    }

    String isiFile = fileBaca.readString();
    fileBaca.close();
```

 Membaca kembali isi file untuk ditampilkan.

---

```cpp
    StaticJsonDocument<1024> doc;
    if (deserializeJson(doc, isiFile)) {
        Serial.println("Gagal parsing JSON");
        return;
    }
```

 Mengubah string JSON menjadi objek yang bisa diakses.

---

```cpp
    Serial.println("[KONTEN FILE YANG DITERIMA]");
    Serial.print("NAMA: "); Serial.println(doc["nama"].as<const char*>());
    Serial.print("JURUSAN: "); Serial.println(doc["jurusan"].as<const char*>());
    Serial.print("UMUR: "); Serial.println(doc["umur"].as<int>());
    Serial.print("DESKRIPSI DIRI: "); Serial.println(doc["deskripsi"].as<const char*>());

    totalChunk = 0;
    memset(flagTerima, 0, sizeof(flagTerima));
}
```

 **Menampilkan isi file JSON** hasil transfer dan mereset buffer untuk pengiriman berikutnya.

---

###  **main.cpp**

```cpp
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <SPIFFS.h>
#include "receiver.h"

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    if (!SPIFFS.begin(true)) Serial.println("SPIFFS gagal");
    if (esp_now_init() != ESP_OK) Serial.println("ESP-NOW gagal");

    esp_now_register_recv_cb(saatTerimaData);
}

void loop() {
    if (semuaChunkDiterima()) {
        simpanDanTampilFile();
    }
}
```

 **Penjelasan:**

* `setup()` → inisialisasi SPIFFS dan ESP-NOW.
* `loop()` → panggil funngsi `semuaChunkDiterima()` untuk cek apakah semua chunk sudah diterima.
  Jika ya → panggil `simpanDanTampilFile()` untuk menyatukan file dan menampilkan hasilnya.

---

##  **Output Akhir di Serial Monitor Receiver**

```
[KONTEN FILE YANG DITERIMA]
NAMA: Muhammad Fahmi Ilmi
JURUSAN: Teknik Informatika
UMUR: 19
DESKRIPSI DIRI: Saya adalah orang yang sangat disiplin dan rendah hati...
```

---
## Link:
* http://wjwwood.io/serial/doc/1.1.0/index.html
* https://www.geeksforgeeks.org/c/fread-function-in-c/
* https://docs.arduino.cc/language-reference/en/functions/communication/serial/write/
* https://docs.arduino.cc/language-reference/en/functions/communication/serial/readString/
* https://arduinojson.org/v6/api/staticjsondocument/
* https://www.programmingelectronics.com/spiffs-esp32/
* https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/network/esp_now.html#

---
## Sekian MAAFF KALAUN SALAH

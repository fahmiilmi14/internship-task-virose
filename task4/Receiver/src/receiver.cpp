#include "receiver.h"
#include <SPIFFS.h>
#include <esp_now.h>
#include <ArduinoJson.h>

char bufferTerima[MAKS_CHUNK][240];
bool flagTerima[MAKS_CHUNK] = {false};
int totalChunk = 0;

void saatTerimaData(const uint8_t *mac, const uint8_t *dataMasuk, int len) {
    Paket paket;
    memcpy(&paket, dataMasuk, sizeof(paket));
    memcpy(bufferTerima[paket.id], paket.data, paket.panjang);
    flagTerima[paket.id] = true;
    if (paket.id + 1 > totalChunk) totalChunk = paket.id + 1;
}

void simpanDanTampilFile() {
    // tulis semua chunk ke SPIFFS
    File fileTulis = SPIFFS.open("/terima.json","wb");
    for (int i=0;i<totalChunk;i++) {
        fileTulis.write((uint8_t*)bufferTerima[i], strlen(bufferTerima[i]));
    }
    fileTulis.close();

    // baca file ke String 
    File fileBaca = SPIFFS.open("/terima.json","r");
    if (!fileBaca) {
        Serial.println("Gagal buka file JSON");
        return;
    }

    String isiFile = fileBaca.readString();
    fileBaca.close();

    // parsing JSON
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, isiFile);
    if (error) {
        Serial.println("Gagal parsing JSON");
        return;
    }

    // cetak sesuai format
    Serial.println("[KONTEN FILE YANG DITERIMA]");
    Serial.print("NAMA: "); Serial.println(doc["nama"].as<const char*>());
    Serial.print("JURUSAN: "); Serial.println(doc["jurusan"].as<const char*>());
    Serial.print("UMUR: "); Serial.println(doc["umur"].as<int>());
    Serial.print("DESKRIPSI DIRI: "); Serial.println(doc["deskripsi"].as<const char*>());

    // reset flag
    totalChunk = 0;
    memset(flagTerima, 0, sizeof(flagTerima));
}

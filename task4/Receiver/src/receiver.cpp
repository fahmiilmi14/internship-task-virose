#include "receiver.h"
#include <SPIFFS.h>
#include <esp_now.h>
#include <ArduinoJson.h>

char bufferTerima[MAKS_CHUNK][240];
int panjangChunk[MAKS_CHUNK];  // simpan panjang asli tiap chunk
bool flagTerima[MAKS_CHUNK] = {false};
int totalChunk = 0;

void saatTerimaData(const uint8_t *mac, const uint8_t *dataMasuk, int len) {
    Paket paket;
    memcpy(&paket, dataMasuk, sizeof(paket));
    memcpy(bufferTerima[paket.id], paket.data, paket.panjang);
    panjangChunk[paket.id] = paket.panjang;     // simpan panjang aktual
    flagTerima[paket.id] = true;
    if (paket.id + 1 > totalChunk) totalChunk = paket.id + 1;
}

bool semuaChunkDiterima() {
    if (totalChunk == 0) return false;
    for (int i = 0; i < totalChunk; i++)
        if (!flagTerima[i]) return false;
    return true;
}

void simpanDanTampilFile() {
    File fileTulis = SPIFFS.open("/terima.json","w");
    for (int i = 0; i < totalChunk; i++) {
        fileTulis.write((uint8_t*)bufferTerima[i], panjangChunk[i]);
    }
    fileTulis.close();

    File fileBaca = SPIFFS.open("/terima.json","rb");
    if (!fileBaca) {
        Serial.println("Gagal buka file JSON");
        return;
    }

    String isiFile = fileBaca.readString();
    fileBaca.close();

    DynamicJsonDocument doc(2048);  
    DeserializationError error = deserializeJson(doc, isiFile);
    if (error) {
        Serial.println("Gagal parsing JSON");
        Serial.println(isiFile);  
        return;
    }

    Serial.println("[KONTEN FILE YANG DITERIMA]");
    Serial.print("NAMA: "); Serial.println(doc["nama"].as<const char*>());
    Serial.print("JURUSAN: "); Serial.println(doc["jurusan"].as<const char*>());
    Serial.print("UMUR: "); Serial.println(doc["umur"].as<int>());
    Serial.print("DESKRIPSI DIRI: "); Serial.println(doc["deskripsi"].as<const char*>());

    // reset
    totalChunk = 0;
    memset(flagTerima, 0, sizeof(flagTerima));
}

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
    bool semuaDiterima = true;
    for (int i=0;i<totalChunk;i++) if (!flagTerima[i]) semuaDiterima = false;

    if (semuaDiterima && totalChunk > 0) {
        simpanDanTampilFile();
    }
}

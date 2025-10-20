#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <SPIFFS.h>
#include "receiver.h"

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    Serial.println(WiFi.macAddress());

    if (!SPIFFS.begin(true)) Serial.println("SPIFFS gagal");
    if (esp_now_init() != ESP_OK) Serial.println("ESP-NOW gagal");

    esp_now_register_recv_cb(saatTerimaData);
}

void loop() {
    if (semuaChunkDiterima()) {
        simpanDanTampilFile();
    }
}

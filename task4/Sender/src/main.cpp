#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "sender.h"  // include header

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

void loop() {
    if (Serial.available() >= 1) {
        uint8_t id = Serial.read(); // baca header ID
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

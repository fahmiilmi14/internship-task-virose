#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "sender.h"

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    Serial.println("Bridge siap...");

    if (esp_now_init() != ESP_OK) Serial.println("ESP-NOW gagal");

    uint8_t alamatReceiver[] = {0x30,0xC9,0x22,0x32,0xC8,0x70};
    esp_now_peer_info_t infoPeer = {};
    memcpy(infoPeer.peer_addr, alamatReceiver, 6);
    infoPeer.channel = 0;
    infoPeer.encrypt = false;
    esp_now_add_peer(&infoPeer);
}

void loop() {
    static uint8_t id = 0;
    static uint8_t buffer[240];
    static int panjangData = 0;
    static unsigned long lastDataTime = 0;

    // kalau ada data baru dari serial
    while (Serial.available() > 0 && panjangData < 240) {
        if (panjangData == 0) {
            id = Serial.read(); // byte pertama = ID
        } else {
            buffer[panjangData - 1] = Serial.read();
        }
        panjangData++;
        lastDataTime = millis();
    }

    // kalau sudah lama gak ada data baru, kirim paket
    if (panjangData > 1 && millis() - lastDataTime > 50) {
        Paket paket;
        paket.id = id;
        paket.panjang = panjangData - 1;
        memcpy(paket.data, buffer, paket.panjang);

        uint8_t penerima[] = {0x30,0xC9,0x22,0x32,0xC8,0x70};
        kirimESPNow(paket, penerima);

        Serial.printf("Kirim chunk ID %d, panjang %d byte\n", id, paket.panjang);

        panjangData = 0; // reset buat chunk berikutnya
    }
}

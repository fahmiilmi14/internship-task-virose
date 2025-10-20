#include "sender.h"
#include <esp_now.h>

void kirimESPNow(Paket paket, const uint8_t *alamatPenerima) {
    esp_err_t hasil = esp_now_send(alamatPenerima, (uint8_t *)&paket, sizeof(paket));
    if (hasil != ESP_OK) Serial.println("Gagal kirim ESP-NOW");
}

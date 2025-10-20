#ifndef SENDER_H
#define SENDER_H

#include <Arduino.h>

struct Paket {
    uint8_t id;
    uint8_t panjang;
    char data[240];
};

// deklarasi fungsi kirim
void kirimESPNow(Paket paket, const uint8_t *alamatPenerima);

#endif

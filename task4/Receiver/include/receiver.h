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

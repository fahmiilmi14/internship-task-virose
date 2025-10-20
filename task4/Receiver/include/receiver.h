#ifndef RECEIVER_H
#define RECEIVER_H

#include <Arduino.h>

struct Paket {
    uint8_t id;
    uint8_t panjang;
    char data[240];
};

#define MAKS_CHUNK 50

extern char bufferTerima[MAKS_CHUNK][240];
extern bool flagTerima[MAKS_CHUNK];
extern int totalChunk;

void saatTerimaData(const uint8_t *mac, const uint8_t *dataMasuk, int len);

#endif

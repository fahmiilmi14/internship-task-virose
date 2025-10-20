#include <iostream>
#include <fstream>
#include <cstring>
#include <serial/serial.h>

#define UKURAN_CHUNK 240  // sisakan untuk header/protokol

int main() {
    const char* nama_file = "../../data.json";

    FILE *file = fopen(nama_file, "rb");
    if (!file) {
        std::cerr << "Gagal buka file\n";
        return 1;
    }

    serial::Serial serialESP("/dev/ttyUSB0", 115200, serial::Timeout::simpleTimeout(1000));
    if (!serialESP.isOpen()) {
        std::cerr << "Gagal buka port serial\n";
        return 1;
    }

    char buffer[UKURAN_CHUNK];
    int id_chunk = 0;

    while (!feof(file)) {
        size_t byte_terbaca = fread(buffer, 1, UKURAN_CHUNK, file);
        if (byte_terbaca > 0) {
            char paket[UKURAN_CHUNK + 1];
            paket[0] = id_chunk;               // header: ID
            memcpy(paket + 1, buffer, byte_terbaca); // data raw binary

            serialESP.write((uint8_t*)paket, byte_terbaca + 1);
            id_chunk++;

            std::cout << "Chunk " << id_chunk-1 << " dikirim, size=" << byte_terbaca << "\n";
        }
    }

    fclose(file);
    serialESP.close();
    std::cout << "Semua data berhasil dikirim!\n";
    return 0;
}

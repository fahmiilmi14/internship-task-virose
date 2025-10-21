#include <iostream>
#include <fstream>
#include <cstring>
#include <serial/serial.h>
#include <thread>
#include <chrono>

#define UKURAN_CHUNK 240

int main() {
    const char* nama_file = "../../data.json";

    FILE *file = fopen(nama_file, "rb");
    if (!file) {
        std::cerr << "Gagal buka file\n";
        return 1;
    }

    serial::Serial serialESP("COM3", 115200, serial::Timeout::simpleTimeout(1000));
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
            paket[0] = (char)id_chunk;               // header ID
            memcpy(paket + 1, buffer, byte_terbaca);

            serialESP.write((uint8_t*)paket, byte_terbaca + 1);
            serialESP.flush();

            std::cout << "Chunk " << id_chunk << " dikirim, size=" << byte_terbaca << "\n";
            id_chunk++;

            std::this_thread::sleep_for(std::chrono::milliseconds(80));
        }
    }

    fclose(file);
    serialESP.close();
    std::cout << "Semua data berhasil dikirim!\n";
    return 0;
}

#include "main.h"

esp_now_peer_info_t peer_info;

uint8_t mac_addresses[MAC_ADDRESS_TOTAL][MAC_ADDRESS_LENGTH] = {
    {0x24, 0x0A, 0xC4, 0x0A, 0x21, 0x11},  // BISMA
    {0x24, 0x0A, 0xC4, 0x0A, 0x21, 0x10},  // JSON
    {0x24, 0x0A, 0xC4, 0x0A, 0x20, 0x11},  // FARUG
    {0x24, 0x0A, 0xC4, 0x0A, 0x10, 0x10},  // Fauzan Firdaus
    {0x24, 0x0A, 0xC4, 0x0A, 0x10, 0x11},  // Africha Sekar wangi
    {0x24, 0x0A, 0xC4, 0x0A, 0x11, 0x10},  // Rafaina Erin Sadia
    {0x24, 0x0A, 0xC4, 0x0A, 0x11, 0x11},  // Antonius Michael Yordanis Hartono
    {0x24, 0x0A, 0xC4, 0x0A, 0x12, 0x10},  // Dinda Sofi Azzahro
    {0x24, 0x0A, 0xC4, 0x0A, 0x12, 0x11},  // Muhammad Fahmi Ilmi
    {0x24, 0x0A, 0xC4, 0x0A, 0x13, 0x10},  // Dhanishara Zaschya Putri Syamsudin
    {0x24, 0x0A, 0xC4, 0x0A, 0x13, 0x11},  // Irsa Fairuza
    {0x24, 0x0A, 0xC4, 0x0A, 0x14, 0x10},  // Revalinda Bunga Nayla Laksono

};

const char *mac_names[MAC_ADDRESS_TOTAL] = {
    "BISMA",                               // 0
    "JSON",                                // 1
    "FARUG",                               // 2
    "Fauzan Firdaus",                      // 3
    "Africha Sekar Wangi",                 // 4
    "Rafaina Erin Sadia",                  // 5
    "Antonius Michael Yordanis Hartono",   // 6
    "Dinda Sofi Azzahro",                  // 7
    "Muhammad Fahmi Ilmi",                 // 8
    "Dhanishara Zaschya Putri Syamsudin",  // 9
    "Irsa Fairuza",                        // 10
    "Revalinda Bunga Nayla Laksono",       // 11
};

esp_err_t mulai_esp_now(int index_mac_address) {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    /* Init ESP-NOW */
    esp_err_t result = esp_now_init();
    if (result != ESP_OK)
        return result;

    /* Set callback function to handle received data */
    result = esp_now_register_recv_cb(callback_data_esp_now);
    if (result != ESP_OK)
        return result;

    result = esp_now_register_send_cb(callback_pengiriman_esp_now);
    //     if (result != ESP_OK)
    //         return result;

    /* Set MAC Address */
    uint8_t mac[MAC_ADDRESS_LENGTH];
    memcpy(mac, mac_addresses[index_mac_address], MAC_ADDRESS_LENGTH);
    result = esp_wifi_set_mac(WIFI_IF_STA, mac);
    if (result != ESP_OK)
        return result;

    /* Initialize peer_info and set fields*/
    memset(&peer_info, 0, sizeof(esp_now_peer_info_t));
    peer_info.channel = 0;
    peer_info.encrypt = false;

    /* Add All MAC to peer list  */
    for (int i = 0; i < MAC_ADDRESS_TOTAL; i++) {
        memcpy(peer_info.peer_addr, mac_addresses[i], MAC_ADDRESS_LENGTH);
        result = esp_now_add_peer(&peer_info);
        if (result != ESP_OK)
            return result;
    }

    return ESP_OK;
}

int cari_mac_index(const uint8_t *mac) {
    for (int i = 0; i < MAC_ADDRESS_TOTAL; i++) {
        // Compare the MAC address
        if (memcmp(mac, mac_addresses[i], MAC_ADDRESS_LENGTH) == 0)
            return i;
    }

    // if not found return -1
    return -1;
}

String mac_index_to_names(int mac_index) {
    if ((mac_index < 0 || mac_index >= MAC_ADDRESS_TOTAL) || (mac_index == -1)) {
        return "Unknown";
    }
    return mac_names[mac_index];
}

void callback_data_esp_now(const uint8_t *mac, const uint8_t *data, int len) {
    int index_mac_asal = cari_mac_index(mac);
    process_perintah(data, len, index_mac_asal);
}
void callback_pengiriman_esp_now(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.printf("\nStatus pengiriman ESP-NOW: %s\n", esp_err_to_name(status));
}
void callback_data_serial(const uint8_t *data, int len) {
    process_perintah(data, len);
}
void baca_serial(void (*callback)(const uint8_t *data, int len)) {
    // TODO 2: implementasi kode buat nerima perintah dari serial
    if(Serial.available() >= 4) {
        //baca Header
        if(Serial.read() == 0xFF && Serial.read() == 0xFF && Serial.read() == 0x00){
            int panjang_data = Serial.read(); //untuk ambil panjang data;

            //untuk memastikan data sudah siap dibaca
            while (Serial.available() < panjang_data){
                delay(1);
            }

            // baca dtaa sebanyak panjang_data
            uint8_t buffer[50]; //buffer untuk data
            for (int i = 0; i < panjang_data; i++){
                buffer[i] = Serial.read();
            }

            //kirim ke fungsi callbak
            callback(buffer, panjang_data);
        }
    }
}

// --------------------- Proses Perintah ----------------------
void process_perintah(const uint8_t* data,int len,int index_mac_asal){
    if(len<2) return; // minimal 2 byte: perintah + tujuan

    uint8_t perintah = data[0];
    uint8_t tujuan   = data[1];

    String pesan = "";
    for(int i=2;i<len;i++) pesan += (char)data[i];

    String nama_asal = (index_mac_asal==-1)?"Laptop":mac_index_to_names(index_mac_asal);
    String nama_tujuan = mac_index_to_names(tujuan);

    // --------- PERINTAH HALO ------------
    if(perintah==0x00){
        if(index_mac_asal==-1){ // dari Serial
            uint8_t kirim[50];
            kirim[0]=0x00;
            kirim[1]=tujuan;
            String tambahan = "Halo " + nama_tujuan + " Aku " + mac_index_to_names(mac_index_ku);
            memcpy(&kirim[2], tambahan.c_str(), tambahan.length());
            esp_now_send(mac_addresses[tujuan],kirim,2+tambahan.length());
            Serial.println("[Serial]->[ESP-NOW] "+tambahan);
        } else { // dari ESP lain
            uint8_t balas[50];
            balas[0]=0x01; // JAWAB
            balas[1]=index_mac_asal;
            String tambahan = "Halo Juga " + nama_asal + " Aku " + mac_index_to_names(mac_index_ku);
            memcpy(&balas[2], tambahan.c_str(), tambahan.length());
            esp_now_send(mac_addresses[index_mac_asal],balas,2+tambahan.length());
            Serial.println("[ESP-NOW]->Balasan "+tambahan);
        }
    }
    // --------- PERINTAH JAWAB ------------
    else if(perintah==0x01){
        if(index_mac_asal==-1){ // dari Serial
            uint8_t kirim[50];
            kirim[0]=0x02; // CEK
            kirim[1]=tujuan;
            String tambahan = nama_tujuan+" ini "+mac_index_to_names(mac_index_ku)+" apa kamu disana?";
            memcpy(&kirim[2], tambahan.c_str(), tambahan.length());
            esp_now_send(mac_addresses[tujuan],kirim,2+tambahan.length());
            Serial.println("[Serial]->[ESP-NOW] "+tambahan);
        } else { // dari ESP lain
            uint8_t balas[50];
            balas[0]=0x01; // JAWAB
            balas[1]=index_mac_asal;
            String tambahan = "Iya Aku "+nama_asal+" Disini - "+mac_index_to_names(mac_index_ku);
            memcpy(&balas[2], tambahan.c_str(), tambahan.length());
            esp_now_send(mac_addresses[index_mac_asal],balas,2+tambahan.length());
            Serial.println("[ESP-NOW]->Balasan "+tambahan);
        }
    }
    // --------- PERINTAH CEK ------------
    else if(perintah==0x02){
        if(index_mac_asal!=-1){ // hanya diterima ESP lain
            Serial.print("[JAWAB DITERIMA] ");
            Serial.println(pesan);
        }
    }
    else{
        Serial.println("[PERINTAH TIDAK VALID]");
    }
}

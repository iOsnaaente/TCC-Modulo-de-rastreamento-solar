#include "../src/controllers/includes/serialController.h"
#include "../src/modules/logger/sdcard.h"

SDCard *sdCard;

void SDCardTest() {
    
    serial_begin();

    sdCard = new SDCard( "My SD Card", "/TrackerLogger" );
    DEBUG_SERIAL( "MAIN", "SDCard begin" );

    // Escreve um arquivo no cartão SD
    const char* data = "Hello, world!\n";
    DEBUG_SERIAL( "MAIN", sdCard->default_path.c_str() );
    
    if (sdCard->write( "/example.txt", (const uint8_t*)data, strlen(data)) == ESP_OK) {
        DEBUG_SERIAL( "MAIN", "File written successfully");
    } else {
        DEBUG_SERIAL( "MAIN", "Failed to write file");
    }

    // Lê o arquivo do cartão SD
    uint8_t buffer[128] = {0};
    if (sdCard->read("/example.txt", buffer, strlen(data)) == ESP_OK) {
        Serial.print("File read successfully: ");
        DEBUG_SERIAL( "MAIN", (char*)buffer);
    } else {
        DEBUG_SERIAL( "MAIN", "Failed to read file");
    }

    // Desmonta o cartão SD
    if (sdCard->unmount() == ESP_OK) {
        DEBUG_SERIAL( "MAIN", "SD Card unmounted successfully");
    } else {
        DEBUG_SERIAL( "MAIN", "Failed to unmount SD Card");
    }
}


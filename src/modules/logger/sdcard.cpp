/**
  * @file SDCard.cpp
  * @brief Implementação da classe de interface para uso do Logger usando o SDCard.
  * @details Esta classe fornece a implementação de operações com um cartão SD, como leitura, escrita,
  *          montagem, desmontagem e formatação. Ela herda da classe Logger para funcionalidades de
  *          registro de logs.
  * @author Bruno Gabriel Flores Sampaio
  * @date 01 de agosto de 2024
  */


#include "sdcard.h"


// Monta o cartão SD
esp_err_t SDCard::mount( void ) {
    if ( this->is_mounted() ){
        DEBUG_SERIAL( "SDCard::mount", "SDCard already mounted!" );
        return ESP_ERR_INVALID_STATE;
    }

    // Inicia o barramento SPI
    spi_bus_config_t spi_config;
    memset(&spi_config, 0, sizeof(spi_bus_config_t));
    spi_config.mosi_io_num = BOARD_MOSI;
    spi_config.miso_io_num = BOARD_MISO;
    spi_config.sclk_io_num = BOARD_SCK;
    spi_config.quadwp_io_num = -1;
    spi_config.quadhd_io_num = -1;
    spi_config.max_transfer_sz = 4000000U;

    esp_err_t ret = spi_bus_initialize( VSPI_HOST, &spi_config, 1 );
    if (ret != ESP_OK) {
        DEBUG_SERIAL("SDCard::mount", "Falha ao inicializar o VSPI bus!");
        return ret;
    }    
    DEBUG_SERIAL("SDCard::mount", "Barramento VSPI inicializado com sucesso!" );

    // Inicia o cartão SD usando o pino CS especificado
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = BOARD_CS;
    slot_config.host_id = VSPI_HOST;

    // uint8_t ssPin, SPIClass &spi, uint32_t frequency, const char *mountpoint
    if ( !SD.begin( BOARD_CS, this->default_path.c_str() ) ){
        DEBUG_SERIAL( "SDCard::mount", "SDCard Mount falhou!" );
        this->is_initialized = false;
        return ESP_ERR_INVALID_STATE;
    }
    this->create_directory( this->default_path.c_str() );
    this->is_initialized = true;
    DEBUG_SERIAL( "SDCard::mount", "SDCard Mount feito com sucesso!" );
    return ESP_OK;
}


// Desmonta o sistema de arquivos do cartão SD
esp_err_t SDCard::unmount( void ) {
    if (!this->is_mounted()) {
        DEBUG_SERIAL( "SDCard::unmount", "SDCard não inicializado!" );
        return ESP_ERR_INVALID_STATE;
    }
    this->is_initialized = false;
    DEBUG_SERIAL( "SDCard::unmount", "SDCard unmounted");
    return ESP_OK;
}


// Escreve dados em um arquivo no cartão SD
esp_err_t SDCard::write(const std::string path, const uint8_t* data, size_t length) {
    if (!this->is_mounted()) {
        DEBUG_SERIAL("SDCard::write", "Card não inicializado");
        return ESP_ERR_INVALID_STATE;
    }
    DEBUG_SERIAL("SDCard::write", path.c_str() );
    File file = SD.open(path.c_str(), FILE_APPEND );
    if (!file) {
        DEBUG_SERIAL("SDCard::write", "Falha para abrir o arquivo para escrita");
        return ESP_FAIL;
    }
    size_t written = file.write(data, length);
    file.close();
    if (written == length) {
        DEBUG_SERIAL("SDCard::write", "Escrito no arquivo: " + path.c_str());
        return ESP_OK;
    } else {
        DEBUG_SERIAL("SDCard::write", "A escrita falhou!");
        return ESP_FAIL;
    }
}

// Lê dados de um arquivo no cartão SD
esp_err_t SDCard::read(const std::string path, uint8_t* buffer, size_t length) {
    if (!this->is_mounted()) {
        DEBUG_SERIAL("SDCard::read", "Card not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    File file = SD.open( path.c_str(), FILE_READ );
    if (!file) {
        DEBUG_SERIAL("SDCard::read", "Failed to open file for reading");
        return ESP_FAIL;
    }
    size_t read_length = file.read(buffer, length);
    file.close();
    if (read_length == length) {
        DEBUG_SERIAL("SDCard::read", "File read: " + path.c_str() );
        return ESP_OK;
    } else {
        DEBUG_SERIAL("SDCard::read", "Read failed");
        return ESP_FAIL;
    }
    return ESP_OK;
}

bool SDCard::is_mounted( void ) const {
    return this->is_initialized;
}


bool SDCard::directory_exist(const char* path){
    File dir = SD.open(path);
    bool exists = dir && dir.isDirectory();
    if (dir)
        dir.close();
    return exists;
}


void SDCard::create_directory(const char* path) {
    File dir = SD.open(path);
    if (dir) {
        if (dir.isDirectory()) {
            DEBUG_SERIAL( "SDCard::create_directory", "Directory already exists");
        } else {
            DEBUG_SERIAL( "SDCard::create_directory", "Path exists but is not a directory");
        }
        dir.close();
    // O diretório não existe, então cria um novo
    } else {
        if (SD.mkdir(path)) {
            DEBUG_SERIAL( "SDCard::create_directory", "Directory created successfully");
        } else {
            DEBUG_SERIAL( "SDCard::create_directory", "Failed to create directory");
        }
    }
}
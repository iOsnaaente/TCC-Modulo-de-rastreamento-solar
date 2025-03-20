/**
  * @file SDCard.h
  * @brief Classe de interface para uso do Logger usando o SDCard.
  * @details Esta classe fornece a interface para operações com um cartão SD, como leitura, escrita,
  *          montagem, desmontagem e formatação. Ela herda da classe Logger para funcionalidades de
  *          registro de logs.
  * @author Bruno Gabriel Flores Sampaio
  * @date 01 de agosto de 2024
  */


#ifndef SDCARD_H
#define SDCARD_H

#include "../../controllers/includes/serialController.h"
#include "../../interfaces/Logger.h"
#include "../../../board_config.h"

#include "driver/sdspi_host.h"

#include <esp_log.h>
#include <esp_err.h>
#include <string>

#include "SD.h"
#include "SPI.h"
#include "FS.h"


class SDCard : public Logger {

public:
    // Objeto para tratar os métodos de diretórios via SDCard 
    // fs::SDFS *sd;

    /**
     * @brief Construtor da classe SDCard.
     * @param description Descrição do cartão SD.
     * @param default_path Caminho de escrita no castão SD.
     */
    SDCard( const std::string description = "SD Card", const std::string default_path = "/Tracker_datalogger"  ) 
    : Logger(description, default_path) {
        is_initialized = false;
        this->mount();
    };

    /** 
     * @brief Destrutor da classe SDCard 
     */
    ~SDCard() {
        if (this->is_initialized) {
            DEBUG_SERIAL( "SDCard::unmount", "SDCard unmounted!" );
            unmount();
        }else{
            DEBUG_SERIAL( "SDCard::unmount", "SDCard alreadey unmounted!" );
        }
        this->is_initialized = false;
    }

    /**
     * @brief Escreve dados em um arquivo no cartão SD.
     * @param path Caminho do arquivo no cartão SD.
     * @param data Ponteiro para os dados a serem escritos.
     * @param length Tamanho dos dados a serem escritos, em bytes.
     * @return ESP_OK em caso de sucesso, ou outro código de erro.
     */
    esp_err_t write(const std::string path, const uint8_t* data, size_t length) override;

    /**
     * @brief Lê dados de um arquivo no cartão SD.
     * @param path Caminho do arquivo no cartão SD.
     * @param buffer Ponteiro para o buffer onde os dados lidos serão armazenados.
     * @param length Tamanho dos dados a serem lidos, em bytes.
     * @return ESP_OK em caso de sucesso, ou outro código de erro.
     */
    esp_err_t read(const std::string path, uint8_t* buffer, size_t length) override;

    /**
     * @brief Verifica se o cartão SD está inserido.
     * @return true se o cartão estiver inserido, false caso contrário.
     */
    bool is_mounted( void ) const override;

    /**
     * @brief Monta o cartão SD.
     * @return ESP_OK em caso de sucesso, ou outro código de erro.
     */
    esp_err_t mount( void );

    /**
     * @brief Desmonta o sistema de arquivos do cartão SD.
     */
    esp_err_t unmount( void );
    
     /**
     * @brief Verifica se um diretório existe.
     * @param path Caminho do diretório a ser verificado.
     */
    bool directory_exist(const char* path);

    /**
     * @brief Verifica se um diretório existe e, se não existir, cria-o.
     * @param path Caminho do diretório a ser verificado/criado.
     */
    void create_directory(const char* path);

};

#endif // SDCARD_H

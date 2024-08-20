/**
  * @file Logger.h
  * @brief Classe de interface para uso do Logger usando o SDCard
  * @details Este arquivo contém a definições e protótipos da classe Interface de uso para SDCard.
  * @note A classe SDCard é uma interface e deve ter seus métodos sobreescritos pela classe que definira as 
  * os métodos corretos de uso do SDCard, únicos para cada tipo de cartão SD usado.
  * @author Bruno Gabriel Flores Sampaio, 
  * @date 01 de agosto de 2024
  */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <esp_err.h>
#include <cstdint>
#include <string>

enum logger_type_t {
  SDCARD_LOGGING,
  SERIAL_LOGGING,
};

struct logger_data_t {
  uint16_t year;
  uint16_t month;
  uint16_t day;
  uint16_t hour;
  uint16_t minute;
  uint16_t second;
  double   lat;
  double   lon;
  double   alt;
  double   temperature;
  double   azimuth;
  double   zenith;
  uint16_t motor_pot;
  uint16_t motor_pos;
  uint16_t motor_temp;
};

class Logger {
public:
    const std::string description;
    const std::string default_path;
    bool is_initialized = false;

    // Inicializa o cartão SD
    Logger( const std::string description, const std::string default_path )
      : description(description), default_path(default_path) {};
    // Deinit  
    virtual ~Logger( ) = default;
    // Escreve dados em um arquivo no cartão SD
    virtual esp_err_t write(const std::string path, const uint8_t* data, size_t length) = 0;
    // Lê dados de um arquivo no cartão SD
    virtual esp_err_t read(const std::string path, uint8_t* buffer, size_t length) = 0;
    // Verifica se o cartão SD está montado
    virtual bool is_mounted( void ) const = 0;
};

#endif
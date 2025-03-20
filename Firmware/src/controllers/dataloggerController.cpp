/**
  * @file dataloggerController.h
  * @brief Controlador para salvar dados de processos
  * @author Bruno Gabriel Flores Sampaio, 
  * @date 02 de agosto de 2024
  */

#include "includes/dataloggerController.h"


// Helper - Formata uma data em formato especifico para o nome dos arquivos desejado
std::string format_date_to_filename(const logger_data_t& log) {
    char filename[sizeof("1990_01_01.csv")];
    std::sprintf(filename, "%04d_%02d_%02d.csv", log.year, log.month, log.day);
    return std::string(filename);
}

// Helper - Formata um pacote de dados em um formato especifico para ser salvo em .csv
std::string format_data_to_write( logger_data_t log, int16_t &len_data ) {
  char data[1024] ={ '\r'};
  len_data = std::sprintf( data, \
    "%04d-%02d-%02d %02d:%02d:%02d,%.6f,%.6f,%.2f,%.2f,%.2f,%.2f,%u,%u,%u\n", \
    log.year, log.month, log.day, \
    log.hour, log.minute, log.second, \
    log.lat, log.lon, log.alt, log.temperature, \
    log.azimuth, log.zenith, \
    log.motor_pot, log.motor_pos, log.motor_temp); \
  return std::string(data);
}


DataloggerController::DataloggerController( const std::string description, logger_type_t logger_type = SDCARD_LOGGING )
  : log_type(logger_type) {
    if ( logger_type == SDCARD_LOGGING ){
      logger = new SDCard( "My SD Card", "/TrackerLogger" ); 
      if ( this->logger->is_mounted()){
      } else {
        delete logger;
        logger = nullptr;
      }
    }else if ( logger_type == SERIAL_LOGGING ){
        DEBUG_SERIAL("Datalogger", "SerialLoggin não implementado");
    } else {
        DEBUG_SERIAL("Datalogger", "Datalogger não encontrado");
    }
}
    
esp_err_t DataloggerController::write_data( logger_data_t log ){
    if (!this->logger || !this->logger->is_mounted()) {
        DEBUG_SERIAL("Datalogger", "Logger not available or not mounted");
        return ESP_ERR_INVALID_STATE;
    }
    // Formata o nome do arquivo 
    std::string filename = format_date_to_filename(log);
    
    // Formata o buffer para escrita 
    int16_t data_size;
    const uint8_t *data_to_write = (const uint8_t *)format_data_to_write( log, data_size ).c_str();
    
    // Escreve no arquivo correto 
    esp_err_t ret = this->logger->write( filename, data_to_write, data_size );
    if ( ret != ESP_OK ){
        DEBUG_SERIAL("Datalogger", "Erro para escrever no arquivo de log!!");
        return ret;
    }
    DEBUG_SERIAL("Datalogger", "Log escrito com sucesso!");
    DEBUG_SERIAL("Datalogger", (const char *)data_to_write ); 
    return ESP_OK;
}


esp_err_t DataloggerController::update_data( logger_data_t &log, bool write_data = false){
  this->log_data = log;
  if (write_data) {
    return this->write_data(log_data);
  }
  return ESP_OK;
}
/**
  * @file dataloggerController.h
  * @brief Controlador para salvar dados de processos
  * @author Bruno Gabriel Flores Sampaio, 
  * @date 02 de agosto de 2024
  */

#ifndef _DATALOGGER_H_ 
#define _DATALOGGER_H_ 

/* Importar a biblioteca externa */
#include "../../../board_config.h"

#include "../../interfaces/Logger.h"
#include "../../modules/logger/sdcard.h"

#include "serialController.h"

#include <cstdio>  
#include <string>


class DataloggerController {
public:
    logger_type_t log_type;
    logger_data_t log_data;
    Logger* logger;

    DataloggerController( const std::string description, logger_type_t logger_type );

    esp_err_t write_data( logger_data_t log );
    esp_err_t update_data( logger_data_t &log, bool write_data );
};



#endif // _DATALOGGER_H_
/**
  * @file main.cpp
  * @brief Entry Point do Firmware de controle 
  * @author Bruno Gabriel Flores Sampaio
  * @date 16 de março de 2025
  */

/** Board configurations */
#include "../board_config.h"

/** Controladores */
#include "controllers/includes/dataloggerController.h"
#include "controllers/includes/modbusController.h"
#include "controllers/includes/motorController.h"
#include "controllers/includes/serialController.h"

/** Módulos */
#include "modules/system/relay.h"


/** Objetos usados no sistema */
DataloggerController    *datalogger;
MotorController         *controller;
ModbusController        *modbus; 
Relay                   *relay;


// Contador para ser usado para debug
uint32_t debugger_counter = 0;

// Periodo para ser printado alguma informação via Terminal 
uint32_t debugger_period = 1000;

// Periodo do loop de verificação de estados 
uint32_t loop_period = 10;


void setup(){

    /** 
     * @brief Inicia a Serial para ser usada como Debug
     * @note Dentro de BoardConfig pode ser alterado o nível de Debug utilizado:
     * 1. DEBUG_SERIAL_COMPLETO 
     * 2. DEBUG_SERIAL_REDUZIDO 
     * 3. DEBUG_SERIAL_DESLIGADO 
    **/ 
    serial_begin( );

    /** 
     * @brief Inicia a comunicação WiFi para controle Modbus
     * 1. Inicia a interface Wi-Fi com Login e Senha definidos em BoardConfig
     * 2. Inicia a aplicação Modbus-TCP/IP utilizando a rede Wi-Fi
     * 3. Inicializa os registradores Modbus 
     * 4. Retorna o objeto Modbus para ser usado 
    **/ 
    DEBUG_SERIAL( "WIFI SSID", SSID_MASTER );
    modbus = new ModbusController( "ModbusController", RTC_SOFT );


    /** 
     * @brief Inicia o controle dos motores
     * @param description Descrição do objeto 
     * @param sensor_type Tipo de sensor para ser usado no módulo 
     * @param motor_type Tipo de motor utilizado no módulo 
     * 
     * 1. Inicializa a classe especifica do Sensor passado como parametro em SensorType_t 
     * 2. Inicializa a classe especifica do Motor passado como parametro em MotorType_t
     * 3. Cria a Task de controle de rastreamento do sistema
     * 
     * @note O tipo de rastreamento é dado pela MACRO definida em BoardConfig: 
     * - AZIMUTE_MODE - Modo de rastreio para Azimute 
     * - ZENITE_MODE - Modo de rastreio para Zenite 
    **/
    controller = new MotorController( "MotorController", SENSOR_AS5600_I2C, MOTOR_DC );  // controller->set_pid_gains( 10.25, 0.42, 4.78 );
    

    /**
     * @brief Define os valores de PID para a malha de controle do motor
     * @details A malha de controle é composta por um controlador em cascata, sendo composto por:
     * 1. Malha externa: Uma malha externa, que recebe o valor de referência do SP da planta (Posição);
     * 2. Malha interna: Uma malha interna que recebe a saída da primeira malha e faz o controle de velocidade do motor.
     * 
     * Dessa forma, a malha externa cuida da referência de posição, pois é a variavel de interesse do sistema e por consequencia,
     * é a malha mais lenta da planta, sua saída entra na malha de controle de velociade, uma malha rápida, que altera a
     * velocidade do motor para alcançar o objetivo do SP mais rápido.
     * 
     * Essa malha em cascata atingiu um bom resultado, sendo usado a medição de velocidade do motor ao invés do valor de 
     * corrente do mesmo, pois essa grandeza não pode ser medida de forma efetiva pelo módulo.   
     * 
     *                        double Kp, Ki, Kd
     **/ 
    controller->set_pos_pid_gains(  120.0, 100.0, 10.0 );
    // controller->set_vel_pid_gains(  1.0, 0.0, 0.0 );



    /** 
     * @brief Inicializa o Relé de proteção. 
    **/
    relay = new Relay( "Relay controller" );
    relay->turn_on();
    controller->stop();

    /** 
     * @brief Inicia a task do LED para debug visual
    **/
    xTaskCreate( &blink_Task, "BlinkTask", 1024*2, NULL, tskIDLE_PRIORITY, NULL );

}



/** 
 * @brief Loop principal 
 * 
 * Optou-se por utilizar o loop principal pois ele desempenha uma função simples, uma vez que as 
 * tarefas complexas de atuação, sensoriamento e controle do sistema, já estão sendo executadas
 * seguindo um Schedule do freeRTOS. 
 *  
 * A função do Loop esta relacionada com a comunicação Modbus do sistema, pois:
 * 1. Ele verifica de forma periodica se é necessário atualizar a Data e Hora do sistema;
 * 2. Ele verifica o estado de operação do sistema. 
 * 
 * @note Como o sistema será demonstrado como um caso de uso, criou-se estados de operação
 * que serão usados exclusivamente para a demonstração. Para uma aplicação real, pode-se criar 
 * estados mais complexos e que se adequem melhor à aplicação do mesmo.
 * 
 * Para a demonstração, será utilizado apenas um conjunto de estados que irão variar o SP da planta 
 * para valores fixos e conhecidos, permitindo mostrar a eficacia do sistema de controle e mostrando
 * que ele pode ser usado em um sistema de rastreamento solar. 
**/
void loop() {

    // Etapa 1: Verificar se é necessário atualizar a Data e Hora 
    if ( modbus->mb->Coil( COIL_DT_SYNC ) == true ){
        modbus->mb->Ireg( INPUT_YEAR,   modbus->mb->Hreg( HR_YEAR ) );
        modbus->mb->Ireg( INPUT_MONTH,  modbus->mb->Hreg( HR_MONTH ) );
        modbus->mb->Ireg( INPUT_DAY,    modbus->mb->Hreg( HR_DAY ) );  
        modbus->mb->Ireg( INPUT_HOUR,   modbus->mb->Hreg( HR_HOUR ) );
        modbus->mb->Ireg( INPUT_MINUTE, modbus->mb->Hreg( HR_MINUTE ) );
        modbus->mb->Ireg( INPUT_SECOND, modbus->mb->Hreg( HR_SECOND ) );
        modbus->mb->Coil( COIL_DT_SYNC, false );
    }

    // Etapa 2: Verifica os estados de operação do sistema 
    switch (modbus->mb->Hreg( HR_STATE ) ){


    /**
     * @brief Estado de operação Automático: COD: 0x00 
     * 
     * Neste estado de operação o sistema irá:
     * 1. Verificar se a Data e Hora do sistema estão atualizados 
     * 2. Iniciar a rotina de rastreio do SP através dos registradores Modbus 
    **/ 
    case HR_STATE_AUTO: 
        
        /**
         * Verifica se a Hora do sistema é válida
         * Se não for, então mantem o sistema desligado 
         */
        if ( modbus->mb->Ireg(INPUT_YEAR) <= 0 ) {
            controller->stop();
            relay->turn_off();

            if ( debugger_counter++ > (debugger_period / loop_period) ){
                debugger_counter = 0;
                DEBUG_SERIAL( "DATETIME", "Waiting datetime update" ); 
                vTaskDelay( pdMS_TO_TICKS(loop_period));
            }
        
        /**
         * Se a hora for válida, verifica se o Zenite é maior que Zero
         * Pois esse valor garante que o sistema deva estar em funcionamento
         * Se não for, o sistema deverá ir para o estado de GO_HOME 
         */
        }else{
            if ( ((int16_t) modbus->mb->Ireg(INPUT_ZENITH)) >= 0.0  ) { 
                controller->start();
                relay->turn_on();
                controller->set_target( modbus->mb->Ireg(INPUT_SUN_TARGET) );
            }else {
                modbus->mb->Hreg( HR_STATE, HR_STATE_GO_HOME ); 
            }        
        }
        break;


    /**
     * @brief Estados de operação por Quadrante: 
     * 
     * Nestes estados de operação o sistema irá possuir um SP fixo
     * determinados pelos valores de HR_QUADRANT_X_ANGLE.
     * 
     * Os valores dos quadrantes estão definidos em ModbusController.h 
     * 
     * HR_QUADRANT_1_ANGLE   ((float)(0.000))
     * HR_QUADRANT_2_ANGLE   ((float)(90.00))
     * HR_QUADRANT_3_ANGLE   ((float)(180.0))
     * HR_QUADRANT_4_ANGLE   ((float)(270.0))
     * HR_QUADRANT_12_ANGLE  ((float)(45.00))
     * HR_QUADRANT_23_ANGLE  ((float)(135.0))
     * HR_QUADRANT_34_ANGLE  ((float)(225.0))
     * HR_QUADRANT_41_ANGLE  ((float)(315.0))
    **/ 
    case HR_STATE_QUADRANT_1: // COD: 0x01
        controller->start();
        controller->set_target( HR_QUADRANT_1_ANGLE );
        modbus->mb->Ireg(INPUT_SUN_TARGET, HR_QUADRANT_1_ANGLE );
        break;
    case HR_STATE_QUADRANT_2: // COD: 0x02
        controller->start();
        controller->set_target( HR_QUADRANT_2_ANGLE );
        modbus->mb->Ireg(INPUT_SUN_TARGET, HR_QUADRANT_2_ANGLE );
        break;
    case HR_STATE_QUADRANT_3: // COD: 0x03
        controller->start();
        controller->set_target( HR_QUADRANT_3_ANGLE );
        modbus->mb->Ireg(INPUT_SUN_TARGET, HR_QUADRANT_3_ANGLE );
        break;
    case HR_STATE_QUADRANT_4: // COD: 0x04
        controller->start();
        controller->set_target( HR_QUADRANT_4_ANGLE );
        modbus->mb->Ireg(INPUT_SUN_TARGET, HR_QUADRANT_4_ANGLE );
        break;

    case HR_STATE_QUADRANT_12: // COD: 0x12
        controller->start();
        controller->set_target( HR_QUADRANT_12_ANGLE );
        modbus->mb->Ireg(INPUT_SUN_TARGET, HR_QUADRANT_12_ANGLE );
        break;
    case HR_STATE_QUADRANT_23: // COD: 0x23
        controller->start();
        controller->set_target( HR_QUADRANT_23_ANGLE );
        modbus->mb->Ireg(INPUT_SUN_TARGET, HR_QUADRANT_23_ANGLE );
        break;
    case HR_STATE_QUADRANT_34: // COD: 0x34
        controller->start();
        controller->set_target( HR_QUADRANT_34_ANGLE );
        modbus->mb->Ireg(INPUT_SUN_TARGET, HR_QUADRANT_34_ANGLE );
        break;
    case HR_STATE_QUADRANT_41: // COD: 0x41
        controller->start();
        controller->set_target( HR_QUADRANT_41_ANGLE );
        modbus->mb->Ireg(INPUT_SUN_TARGET, HR_QUADRANT_41_ANGLE );
        break;
    
    case 127:
        controller->motor->set_direction( BDC_FORWARD );
        controller->motor->set_speed(BDC_MAX_POWER);
    
    case 126:
    case 128:
        controller->motor->set_direction( BDC_BACKWARD );
        controller->motor->set_speed(BDC_MAX_POWER);

    /** 
     * @brief Estado de operação de retorno para casa: COD: 0xFD
     * Retorna o sistema para uma posição inicial conhecida para iniciar a rotina de rastreio
     */
    case HR_STATE_GO_HOME:
        controller->start();
        relay->turn_on();
        controller->set_target( POS_HOME_MIN );
        if ( 
            modbus->mb->Ireg(INPUT_SENSOR_POS) > POS_HOME_MIN && 
            modbus->mb->Ireg(INPUT_SENSOR_POS) < POS_HOME_MAX   
        ){
            modbus->mb->Hreg( HR_STATE, HR_STATE_WAITING );
            DEBUG_SERIAL( "HOME", "POSITION REACHED" );
        }

    /** 
     * @brief Estado de operação aguarda na posição HOME: COD: 0xFE
     */
    case HR_STATE_WAITING:
        controller->stop();
        relay->turn_off();
        if ( ((int16_t) modbus->mb->Ireg(INPUT_ZENITH)) >= 0.0  ) {
            modbus->mb->Hreg( HR_STATE, HR_STATE_AUTO ); 
            DEBUG_SERIAL( "INICITIATE", "Starting Tracking. Good Morning SM!" );             
        } 


    /**
     * @brief Estado de operação DESLIGADO. COD: 0xFF
    **/
    case HR_STATE_OFF:
    default:
        controller->stop();
        break;
    }


    // Debug 
    if ( debugger_counter++ > (debugger_period / loop_period) ){
        debugger_counter = 0;
        DEBUG_SERIAL( "SYSTEM STATE"  , modbus->mb->Hreg( HR_STATE ) ); 
        DEBUG_SERIAL( "GOAL POSITION"  , modbus->mb->Ireg(INPUT_SUN_TARGET) ); 
        DEBUG_SERIAL( "MOTOR POSITION" , controller->position ); 
        DEBUG_SERIAL( "MOTOR SPEED %"  , controller->motor->speed ); 
        Serial.println(WiFi.localIP());
    }
    modbus->mb->Ireg( INPUT_SENSOR_POS, (float)controller->sensor->scaled_position );
    vTaskDelay( pdMS_TO_TICKS( loop_period ) );
}


/** Task de Led blink */
void blink_Task(void *pvParameters) {
    gpio_set_direction( LED_BOARD, (gpio_mode_t) GPIO_MODE_OUTPUT );
    bool state = false;
    while(true) {
        gpio_set_level(LED_BOARD, state = !state );
        vTaskDelay( pdMS_TO_TICKS(500));
    }
}
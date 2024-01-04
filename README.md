# TCC
Projeto de hardware para desenvolvimento de um módulo de controle de motor DC com sensor de posição angular para movimentação de paineis solares em dois eixos utilizando rede Modbus.

### Description 

The [bdc_motor](https://components.espressif.com/component/espressif/bdc_motor), 
The [PID component](https://components.espressif.com/component/espressif/pid_ctrl) is also managed by the component manager. 
These components' dependencies are listed in the [manifest file](main/idf_component.yml).


### Hardware Required

* A iOsnaaente TCC board with the ESP32-WROOM32E;
* A USB cable for Power supply and programming type micro-B;
* A separate 12 or 24V power supply for brushed DC motor;   
* H-bridge L298N datasheet used on board;
* A brushed DC motor connected in output borne on TCC board;
* A quadrature encoder of type AB to detect speed and direction of turns.

### Connection MCPWM + Encoder :
      
                              Battery power (8.4V - 24V)       
                                         |
      ESP 32                             v   L298N                   _
+-------------------+             +--------------------+            | |
|                   |             |      H-Bridge      |            | |
|               GND +<----------->| GND                |      +--------------+
|                   |             |      Chanel 1      |      |              |
|            GPIO_A +----GPIO0--->+ IN_A         OUT_A +----->|   Brushed    |
|            GPIO_B +----GPIO1--->+ IN_B               |      |     DC       |
|           BDC_PWM +----PWM0B--->+ PWM          OUT_B +----->|    Motor     |
|                   |             |                    |      |              |
|                   |             +--------------------+      |              |
|                   |                                         +------+-------+
|                   |                Encoder based                   |
|                   |                   on PCNT                      |
|                   |             +--------------------+             |
|            VCC3.3 +------------>+ VCC    Encoder     |             |
|                   |             |                    |             |
|               GND +<----------->+ GND                +<------------+
|                   |             |                    |
|         ENCODER_A |<---PCNT1----+ C1                 |
|         ENCODER_B |<---PCNT1----+ C2                 |
|                   |             |                    |
|                   |             |                    |
|                   |             |                    |
+-------------------+             +--------------------+

### Steps 

[1] Fazer a definição dos pinouts de motores e encoders
    - IN1, IN2 e PWM para motores
    - ENC_A e ENC_B para o encoder de quadratura 

[2] Inicializar os pinouts apropriadamente 
    - Pinos de motores como OUTPUT
    - Pinos de sensor como INPUT 

[3] Inicializar as configurações do motor
    - Instaciar a struct motor_t 

[4] Inicializar as configurações do encoder
    - Instanciar a struct encoder_t 

[5] Criar a rotina de calculo da velocidade
    - Task ou Timer interrupt 



### Tutorials 

To learn more about Motor Controle PWM (MCPWM) see: 
[1] https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32/api-reference/peripherals/mcpwm.html
[2] https://demo-dijiudu.readthedocs.io/en/latest/api-reference/peripherals/mcpwm.html

To learn more about Pulse CoNTer (PCNT) see: 
[1] https://demo-dijiudu.readthedocs.io/en/latest/api-reference/peripherals/mcpwm.html




/*
 * Description: Controle de velocidade de motor DC com encoder incremental usando malha de controle PID.
 * Author: Bruno Gabriel F. Sampaio 
 * Organization: Grupo de pesquisa em robótica Taura Bots 
 * Date: 09/12/2023 
 * License: MIT
 *
 * Obs: Sistema rodando com motor CB605 com encoder acoplado
 *      Tensão de operação de 12V. 
 *      Placa de desenvolvimento própria. 
 *      Para testar, deve-se mudar as definições dos Pinouts
 *
*/

#include "constants.h"
#include "arduino.h"

#define CURRENT_SENSOR 33

#define SENSOR_PIN1 36
#define SENSOR_PIN2 39
#define LED_PIN 2

#define MOTOR_IN1 17
#define MOTOR_IN2 16
#define MOTOR_ENB 4

#define BACKWARD false
#define FORWARD true

/* Varredura de frequencias */
#define INERTIAL_BREAKS 50
#define DELAY_FOR 35
#define STEP_FOR 1

/* Degrais de frequencia */
#define TIME_LOOP_MS 1000
#define PRINT_DELAY 30

/* Seletor de referencias */
#define SAW_TOOTH_REF 1

/* Controle com PID */
#define RATIO_PWM_PULSES 2
#define KP 0.500
#define KD 0.050
#define KI 0.005

/* Compensa o tempo de loop para garantir sincronia de medição */
#define LOOP_TIME_MS 150
#define PULSES_RATIO 0.20

/* Dente de serra como referencia */
int32_t saw_tooth = 0;

/* Tempo para mudar entre as referencias */
uint32_t ref_time = millis();

/* velocidade de saída */
float vel_out = 0;
float prev_error = 0;
float int_acum = 0;

uint32_t loop_time;

/* Variáveis globais */
uint32_t pulses;
uint32_t current;

uint32_t backward_count = 0;
uint32_t forward_count = 0;
bool direction = BACKWARD;

void move_motor_forward(int vel) {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, HIGH);
  analogWrite(MOTOR_ENB, vel);
}

void move_motor_backward(int vel) {
  digitalWrite(MOTOR_IN2, LOW);
  digitalWrite(MOTOR_IN1, HIGH);
  analogWrite(MOTOR_ENB, vel);
}



/* Função de interrupção para contagem dos pulsos 
 * Para a contagem de pulsos, há dois encoders com
 * defasagem para detecção do sentido de rotação.
 */
void IRAM_ATTR count_pulses(void) {
  bool vel_state = digitalRead(SENSOR_PIN1);
  bool dir_state = digitalRead(SENSOR_PIN2);
  /* Sentido horário */
  if (vel_state && !dir_state) {
    forward_count++;
    /* Sentido anti-horário */
  } else if (!vel_state && dir_state) {
    backward_count++;
    /* Próximo pulso sem importancia */
  } else {
  }
}

/* Para atualizar as variáveis ```pulses``` e ```direction``` 
 * chamar a função compute_pulses
 */
void compute_pulses(void) {
  pulses = (backward_count + forward_count);
  if (backward_count > forward_count) {
    direction = BACKWARD;
  } else {
    direction = FORWARD;
  }
  backward_count = 0;
  forward_count = 0;
}

static inline void compute_current(void) {
  current = analogRead(CURRENT_SENSOR);
}

uint32_t get_ref(uint8_t ref_type) {
  // Dente de serra como referencia
  if (ref_type == SAW_TOOTH_REF) {
    if (saw_tooth > 200) {
      saw_tooth = 0;
    } else {
      saw_tooth += 1;
    }
    return saw_tooth;

  }else{
    return (0.0);
  }
}



void setup(void) {
  /* Configuração dos estados dos pinos usados */
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_ENB, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SENSOR_PIN1, INPUT);
  pinMode(SENSOR_PIN2, INPUT);
  pinMode(CURRENT_SENSOR, INPUT);

  /* Ativar a serial com Baud de 115200 bits por segundo */
  Serial.begin(115200);

  /* Habilitar as interrupções do contador de pulsos */
  attachInterrupt(SENSOR_PIN1, count_pulses, RISING);
  attachInterrupt(SENSOR_PIN2, count_pulses, RISING);

  /* Ligar os motores */
  pinMode(POWER_RELAY, OUTPUT);
  digitalWrite(POWER_RELAY, HIGH);

  /* preparar os encoder */
  compute_pulses();
}


void loop(void) {

  loop_time = micros();
  /* conversão PWM para pulsos */
  float ref_conv = (float)get_ref(SAW_TOOTH_REF);
  /* Computa o número de pulsos */
  compute_pulses();
  /* Computa o Erro */
  float error = (float)(ref_conv - (pulses * PULSES_RATIO));
  /* calculo do integral */
  int_acum += error;
  /* calculo do derivativo */
  float derivative = error - prev_error;
  prev_error = error;
  vel_out += (float)((KP * error) + (KD * derivative) + (KI * int_acum));
  if (vel_out > 0) {
    if (vel_out > 255) {
      vel_out = 255;
    }
    move_motor_forward((uint8_t)vel_out);
  } else {
    vel_out *= -1;
    if (vel_out > 255) {
      vel_out = 255;
    }
    move_motor_backward((uint8_t)vel_out);
  }
  compute_current();

  Serial.print("VEL:");
  Serial.print(vel_out);
  Serial.print(",REF:");
  Serial.print(saw_tooth);
  Serial.print(",SENSOR:");
  Serial.print(pulses * PULSES_RATIO);
  Serial.print(",CURRENT:");
  Serial.print( current/4 );
  Serial.println(",MAX:255,MIN:0");
  delayMicroseconds(LOOP_TIME_MS * 1000 - (micros() - loop_time));
}
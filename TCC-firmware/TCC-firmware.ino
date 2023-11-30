#include "constants.h"

#define DEBUG true 

void setup() {
  /* LED da placa para debug visual  */
  pinMode(LED_BUILTIN, OUTPUT);
  /* ADC com soquete externo - Sensores */
  pinMode(ADC_EXT1, INPUT);
  pinMode(ADC_EXT2, INPUT);
  pinMode(ADC_EXT3, INPUT);
  /* ADCs internos  */
  pinMode(ADC4, INPUT);
  pinMode(ADC5, INPUT);
  pinMode(ADC6, INPUT);
  /* GPIO de controle do Relé de potência  */
  pinMode(POWER_RELAY, OUTPUT);
  /* GPIO para ponte H - L298N */
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  /* ADC de leitura de corrente da ponte H  */
  pinMode(V_SENSING, INPUT);

#if DEBUG
  Serial.begin(UART_BAUDRATE, UART_MODE, UART_RXD0, UART_TXD0 );
  Serial.print( "Serial init");
#endif

  /* Liga o Relé da ponte H */
  digitalWrite( POWER_RELAY, HIGH );

}

void move_motor_forward( int vel ){
  digitalWrite( IN1, LOW );
  digitalWrite( IN2, HIGH );
  analogWrite( ENB, vel );
}

void move_motor_backward( int vel ){
  digitalWrite( IN2, LOW );
  digitalWrite( IN1, HIGH );
  analogWrite ( ENB, vel );
}



void loop() {
  /* Faz um toggle no LED2 da placa */
  digitalWrite( LED_BUILTIN, HIGH );
  move_motor_forward( 255 );
  delay( 1000 );
  digitalWrite( LED_BUILTIN, LOW );
  move_motor_backward( 255 );
  delay( 1000 );
}

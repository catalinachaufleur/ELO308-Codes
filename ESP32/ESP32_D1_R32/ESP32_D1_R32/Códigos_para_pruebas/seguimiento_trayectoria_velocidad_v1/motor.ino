//Esta funcion es valida para la Esp32
#include <analogWrite.h>
/*  Función de motor  */
/*  Pines motor 1 (derecho)*/
#define ain1  16
#define ain2  27
#define pwm_a  17

/*  Pines motor 2 (izquierdo)*/
#define bin1  12
#define bin2  13
#define pwm_b  14
  
void motor(int Velocidad_motor1, int Velocidad_motor2)
{ 
  /*
   * Función de motor motor(M1, M2).
   * Mueve los motores con velocidad M1 y M2 con un valor entero entre 0 y 1024
   * En caso de introducir un número negativo, el motor se mueve en sentido inverso.
  */
  
  pinMode(ain1, OUTPUT); 
  pinMode(ain2, OUTPUT); 

  pinMode(bin1, OUTPUT); 
  pinMode(bin2, OUTPUT); 
  
  analogWriteResolution(pwm_a, 12);       //  Resolutión de los motores 11 = 2048 - 12 = 4096
  analogWriteResolution(pwm_b, 12);
  analogWriteFrequency(pwm_a, 500);
  analogWriteFrequency(pwm_b, 500);
  if (Velocidad_motor1 > 0)
  {
    digitalWrite(ain1, HIGH); 
    digitalWrite(ain2, LOW);
    analogWrite(pwm_a, Velocidad_motor1); 
  }

  else if (Velocidad_motor1 < 0)
  {
    digitalWrite(ain1, LOW); 
    digitalWrite(ain2, HIGH);
    analogWrite(pwm_a, -Velocidad_motor1); 
  }

  else
  {
    digitalWrite(ain1, LOW); 
    digitalWrite(ain2, LOW);
  }

  if (Velocidad_motor2 > 0)
  {
    digitalWrite(bin1, HIGH); 
    digitalWrite(bin2, LOW);
    analogWrite(pwm_b, Velocidad_motor2); 
  }

  else if (Velocidad_motor2 < 0)
  {
    digitalWrite(bin1, LOW); 
    digitalWrite(bin2, HIGH);
    analogWrite(pwm_b, -Velocidad_motor2); 
  }

  else
  {
    digitalWrite(bin1, LOW); 
    digitalWrite(bin2, LOW);
  }
}

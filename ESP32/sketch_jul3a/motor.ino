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

void motor(int Velocidad_motor_izq, int Velocidad_motor_der)
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

  analogWriteFrequency(500);
  
  if (Velocidad_motor_der > 0)
  {
    digitalWrite(ain1, HIGH); 
    digitalWrite(ain2, LOW);
    analogWrite(pwm_a, Velocidad_motor_der); 
  }

  else if (Velocidad_motor_der < 0)
  {
    digitalWrite(ain1, LOW); 
    digitalWrite(ain2, HIGH);
    analogWrite(pwm_a, -Velocidad_motor_der); 
  }

  else
  {
    digitalWrite(ain1, LOW); 
    digitalWrite(ain2, LOW);
    analogWrite(pwm_a, Velocidad_motor_der); 
  }

  if (Velocidad_motor_izq > 0)
  {
    digitalWrite(bin1, HIGH); 
    digitalWrite(bin2, LOW);
    analogWrite(pwm_b, Velocidad_motor_izq); 
  }

  else if (Velocidad_motor_izq < 0)
  {
    digitalWrite(bin1, LOW); 
    digitalWrite(bin2, HIGH);
    analogWrite(pwm_b, -Velocidad_motor_izq); 
  }

  else
  {
    digitalWrite(bin1, LOW); 
    digitalWrite(bin2, LOW);
    analogWrite(pwm_b, Velocidad_motor_izq); 
  }
}

//Los sensores IR estan en los canales io4-A7-A6-A0-A3-io26 de izquierda a derecha
#include <Encoder.h>

Encoder encoder_der(23, 5);
Encoder encoder_izq(19, 18);

float sensorValues[24]; //16 sensores en mux io39 (Delantero y trasero), 8 en io63 (Central)
float sensoresMax[24];
float sensoresMin[24];
float delta_angle;

//Memoria delta-angle
float medicionActual[8] = {0, 0, 0, 0, 0, 0, 0, 0};
byte contador = 0;

//Valores para encoders
int izq;
int lstizq;
int der;
int lstder;
int di;
int dd;

// Modo multilinea
const bool linea = 0;

const int iteraciones = 100000;
void calibrarSensores() {
  for (int i = 0; i <= 23; i++) {
    sensoresMax[i] = 0;
    sensoresMin[i] = 1023;
  }
  for (int i = 0; i < iteraciones; i++) {
    // TO DO: Autocalibración
    if (i < 50000) {
      motor(-80, -80);
      //delay(200);
    }
    else {
      motor(80, 80);
      //delay(200);
    }
    for (byte i = 0; i <= 15; i++) {
      digitalWrite(25, i & 0x01  );
      digitalWrite(26, i & 0x02  );
      digitalWrite(2, i & 0x04  );
      digitalWrite(4, i & 0x08 );
      sensorValues[i] = analogRead(39);
      if (i < 7) {
        // Toma mediciones en "paralelo" con el sensor central y registra con el valor i+16, es decir, 16,17...23
        digitalWrite(25, i & 0x01  );
        digitalWrite(26, i & 0x02  );
        digitalWrite(2, i & 0x04  );
        digitalWrite(4, i & 0x08 );
        sensorValues[(i + 16)] = analogRead(36);
      }
    }
    setMaxMin();
  }
  motor(0, 0);
}
void setMaxMin() {
  for (int i = 0; i <= 23; i++) {
    if (sensoresMax[i] < sensorValues[i]) {
      sensoresMax[i] = sensorValues[i];
    }
    if (sensoresMin[i] > sensorValues[i]) {
      sensoresMin[i] = sensorValues[i];
    }
  }
}

float getposition(byte direccion) {
  float sensorNormalizado[8];
  float pose_sensor = 0;
  float inte[8];
  float sum1;
  float area;
  if (direccion == 1) { // Sensor frontal
    for (byte i = 0; i <= 7; i++) {
      digitalWrite(25, i & 0x01  );
      digitalWrite(26, i & 0x02  );
      digitalWrite(2, i & 0x04  );
      digitalWrite(4, i & 0x08 );
      sensorNormalizado[i] = (1 - 2 * linea) * 100 * (analogRead(39) - sensoresMin[i]) / (sensoresMax[i] - sensoresMin[i]); //MAPEO DE SENSORES
    }
  }
  else if (direccion == 0) { // Sensor trasero
    for (byte i = 0; i <= 7; i++) {
      digitalWrite(25, (i + 8) & 0x01  );
      digitalWrite(26, (i + 8) & 0x02  );
      digitalWrite(2, (i + 8) & 0x04  );
      digitalWrite(4, (i + 8) & 0x08 );
      sensorNormalizado[i] = (1 - 2 * linea) * 100 * (analogRead(39) - sensoresMin[i + 8]) / (sensoresMax[i + 8] - sensoresMin[i + 8]); //MAPEO DE SENSORES
    }
  }
  else { // todas las demas direcciones son el sensor central
    for (byte i = 0; i <= 7; i++) {
      digitalWrite(25, (i) & 0x01  );
      digitalWrite(26, (i) & 0x02  );
      digitalWrite(2, (i) & 0x04  );
      digitalWrite(4, (i) & 0x08 );
      sensorNormalizado[i] = (1 - 2 * linea) * 100 * (analogRead(36) - sensoresMin[0]) / (sensoresMax[0] - sensoresMin[0]); //MAPEO DE SENSORES
    }
  }
  for (int i = 0; i <= 7; i++) {
    inte[i] = 2 * sensorNormalizado[i] - sensorNormalizado[i] * sensorNormalizado[i] / 100;
  }
  //Las medidas del sum están en milimetros y pueden ser de coma flotante. TO DO: Usar un pie de metro para seleccionar los valores de forma precisa
  sum1 = -28 * inte[0] - 20 * inte[1] - 12 * inte[2] - 4 * inte[3] + 4 * inte[4] + 12 * inte[5] + 20 * inte[6] + 28 * inte[7];
  area = inte[0] + inte[1] + inte[2] + inte[3] + inte[4] + inte[5] + inte[6] + inte[7];
  pose_sensor = sum1 / area; //centroide
  //pose_sensor=(2.4*sensorNormalizado[0] + 1.44*sensorNormalizado[1] + 0.48*sensorNormalizado[2] - 0.48*sensorNormalizado[3] - 1.44*sensorNormalizado[4] - 2.4*sensorNormalizado[5])/100; //promedio ponderado
  /*for(int i=0; i<=5;i++){
    Serial.print(' ');
    Serial.print(sensorNormalizado[i]);
    }
    Serial.print(' ');
    Serial.print(pose_sensor);
    Serial.println(' '); */
  return pose_sensor;
}
float getangle(bool direccion) {
  /*
     Función GetAngle v 13.08
     TO DO: Mejorar calidad de mediciones
            Conseguir una pista que permita pruebas dinámicas
            Implementar sistema multilineas
            Incorporar más información de los encoders.
     Sistema de funcionamiento: Toma mediciones del sensor frontal y les da "peso" a partir del sensor central. Toma 8 mediciones para obtener un promedio de la pista y ser más inmune a perturbaciones.
  */
  float pose_sensorfrontal[7];
  float pose_sensorcentral[7];
  float pose_sensorcentralapert;
  float sum = 0;
  float angle_senseless;
  pose_sensorcentralapert = abs(getposition(2));
  izq = encoder_izq.read();
  der = encoder_der.read();
  di = izq - lstizq; //Cambio del encoder izquierdo
  dd = der - lstder; //Cambio del encoder derecho
  if (pose_sensorcentralapert > 2) { //Modo Fulcro
    for (int i = 0; i < 8; i++) {
      pose_sensorfrontal[i] = getposition(direccion);
      pose_sensorcentral[i] = getposition(2);
      sum += pose_sensorfrontal[i] * pose_sensorcentral[i];
    }
    angle_senseless = sum * 0.125; // Toma el promedio de las últimas 8 mediciones. Multiplicar consume menos recursos.
    angle_senseless = 10.89 * angle_senseless; //Convierte a grados sexagesimales [A refinar correctamente]. El factor multiplicativo está obtenido empíricamente
  }
  else if (pose_sensorcentralapert > 1) { // Modo intermedio
    for (int i = 0; i < 8; i++) {
      pose_sensorfrontal[i] = getposition(direccion);
      pose_sensorcentral[i] = getposition(2);
      sum += pose_sensorfrontal[i] * pose_sensorcentral[i];
    }
    angle_senseless = sum * 0.125; // Toma el promedio de las últimas 8 mediciones
    angle_senseless = 12.1638 * angle_senseless;
  }
  else { // Modo pequeña señal
    for (int i = 0; i < 8; i++) {
      pose_sensorfrontal[i] = getposition(direccion);
      pose_sensorcentral[i] = getposition(2);
      sum += pose_sensorfrontal[i] * pose_sensorcentral[i];
    }
    angle_senseless = sum * 0.125; // Toma el promedio de las últimas 8 mediciones
    angle_senseless = 96.25592 * angle_senseless;
  }
  if (contador < 8) {
    contador = contador + 1;
  }
  else {
    contador = 0;
  }
  // Aplica un shifting al vector de mediciones actuales
  for (int i =  7; i > 0; i--) {
    medicionActual[i] = medicionActual[i - 1];
  }
  /*
  for (int i= 0; i< 8; i++) {
    Serial.print("[");
    Serial.print(medicionActual[i]);
    Serial.print("]");
  }
  */
  //Registra las mediciones en un vector
  medicionActual[0] = angle_senseless;
  delta_angle = abs(medicionActual[7] - angle_senseless);

  if (di > dd) {
    delta_angle = delta_angle;
  }
  else {
    delta_angle = delta_angle * -1;
  }
  lstizq = encoder_izq.read();
  lstder = encoder_der.read();
  return delta_angle;
}

// Se busca conseguir un delta angulo más "filtrado", para ello, se promedian 32 valores, para impedir el efecto de deltas de ángulo.
float getangle_promediada(bool direccion) {
  float valor_promediado = 0;
  for (int i = 0; i < 32; i++) {
    valor_promediado += getangle(direccion)*0.03125;
  }
  // Factor de corrección en base a mediciones.
  valor_promediado += 0;
  return valor_promediado;
}
/*void print_MaxMin() {
  for (int i = 0; i <= 5; i++) {
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(" ----> ");
    Serial.print(sensoresMin[i]);
    Serial.print(" - ");
    Serial.print(sensoresMax[i]);
    umbral[i]=(sensoresMax[i]+sensoresMin[i])/4;
    Serial.print(" - ");
    Serial.print(umbral[i]);
  }
  Serial.println(' ');
  }*/

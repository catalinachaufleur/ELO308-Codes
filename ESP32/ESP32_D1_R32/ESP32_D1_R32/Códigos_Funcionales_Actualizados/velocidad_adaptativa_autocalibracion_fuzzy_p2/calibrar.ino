//Los sensores IR estan en los canales io4-A7-A6-A0-A3-io26 de izquierda a derecha
float sensorValues[12];
float sensoresMax[12];
float sensoresMin[12];

// VARIABLES FUNCION getposition
float sensorNormalizado[6];
float pose_sensor=0;
float inte[6];
float sum1;
float area;

bool linea=0;  // valor logico 1 corresponde a una pista con trayectoria color blanco; Valor lógico 0 corresponde a una pista con trayectoria color negro. 

void calibrarSensores() {
  for (int i = 0; i <= 11; i++) {
    sensoresMax[i] = 0;
    sensoresMin[i] = 1023;
  }
  int vcal=200;
  for(int j=0; j<6; j++){
    encoder_der.write(0);
    encoder_izq.write(0);
    while(abs(PI*radio_rueda*(encoder_der.read()+encoder_izq.read())/(210*12))< 12){
      motor(vcal,vcal);
      for(byte i=0;i<=11;i++){
        digitalWrite(25, i & 0x01  );
        digitalWrite(26, i & 0x02  );
        digitalWrite(2, i & 0x04  );
        digitalWrite(4, i & 0x08 );
        sensorValues[i]=analogRead(39);
      }
      setMaxMin();
    }
    vcal=-vcal;
  }
  digitalWrite(25, 2 & 0x01  );
  digitalWrite(26, 2 & 0x02  );
  digitalWrite(2, 2 & 0x04  );
  digitalWrite(4, 2 & 0x08 );
  while((((1-2*linea)*100*(analogRead(39)-sensoresMin[2])/(sensoresMax[2]-sensoresMin[2])) + 100*linea)<80){
    motor(abs(vcal),abs(vcal));
  }
  encoder_der.write(0);
  encoder_izq.write(0);
  while((2*PI*radio_rueda*encoder_der.read()/(210*12)) < 11.2*PI/2){
    motor(0,abs(vcal));
  }
  motor(0,0);
  d_ref=sensor.readReg16Bit(sensor.RESULT_RANGE_STATUS + 10)*0.1;
}
void setMaxMin() {
  for (int i = 0; i <= 11; i++) {
    if (sensoresMax[i] < sensorValues[i]) {
      sensoresMax[i] = sensorValues[i];
    }
    if (sensoresMin[i] > sensorValues[i]) {
      sensoresMin[i] = sensorValues[i];
    }
  }
}

float getposition(bool direccion){
  if(direccion){
    for(byte i=0;i<=5;i++){
      digitalWrite(25, i & 0x01  );
      digitalWrite(26, i & 0x02  );
      digitalWrite(2, i & 0x04  );
      digitalWrite(4, i & 0x08 );
      sensorNormalizado[i]=((1-2*linea)*100*(analogRead(39)-sensoresMin[i])/(sensoresMax[i]-sensoresMin[i])) + 100*linea; //MAPEO DE SENSORES
    }
  }
  else{
    for(byte i=0;i<=5;i++){
      digitalWrite(25, (i+6) & 0x01  );
      digitalWrite(26, (i+6) & 0x02  );
      digitalWrite(2, (i+6) & 0x04  );
      digitalWrite(4, (i+6) & 0x08 );
      sensorNormalizado[i]=((1-2*linea)*100*(analogRead(39)-sensoresMin[i+6])/(sensoresMax[i+6]-sensoresMin[i+6])) + 100*linea; //MAPEO DE SENSORES
    }
  }
  for(int i=0; i<=5;i++){
    inte[i]=2*sensorNormalizado[i] - sensorNormalizado[i]*sensorNormalizado[i]/100;
  }  
  sum1=-24*inte[0] - 14.4*inte[1] - 4.8*inte[2] + 4.8*inte[3] + 14.4*inte[4] + 24*inte[5];
  area=inte[0]+inte[1]+inte[2]+inte[3]+inte[4]+inte[5];
  pose_sensor=sum1/area; //centroide
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

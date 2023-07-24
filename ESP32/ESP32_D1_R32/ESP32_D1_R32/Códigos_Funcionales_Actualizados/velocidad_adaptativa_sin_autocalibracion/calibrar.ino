//Los sensores IR estan en los canales io4-A7-A6-A0-A3-io26 de izquierda a derecha
float sensorValues[12];
float sensoresMax[12];
float sensoresMin[12];

void calibrarSensores() {
  for (int i = 0; i <= 11; i++) {
    sensoresMax[i] = 0;
    sensoresMin[i] = 1023;
  }
  for (int i = 0; i < 150000; i++) {
    for(byte i=0;i<=11;i++){
      digitalWrite(25, i & 0x01  );
      digitalWrite(26, i & 0x02  );
      digitalWrite(2, i & 0x04  );
      digitalWrite(4, i & 0x08 );
      sensorValues[i]=analogRead(39);
    }
    setMaxMin();
  }
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
  float sensorNormalizado[6];
  float pose_sensor=0;
  float inte[6];
  float sum1;
  float area;
  if(direccion){
    for(byte i=0;i<=5;i++){
      digitalWrite(25, i & 0x01  );
      digitalWrite(26, i & 0x02  );
      digitalWrite(2, i & 0x04  );
      digitalWrite(4, i & 0x08 );
      sensorNormalizado[i]=100*(analogRead(39)-sensoresMin[0])/(sensoresMax[0]-sensoresMin[0]); //MAPEO DE SENSORES
    }
  }
  else{
    for(byte i=0;i<=5;i++){
      digitalWrite(25, (i+6) & 0x01  );
      digitalWrite(26, (i+6) & 0x02  );
      digitalWrite(2, (i+6) & 0x04  );
      digitalWrite(4, (i+6) & 0x08 );
      sensorNormalizado[i]=100*(analogRead(39)-sensoresMin[0])/(sensoresMax[0]-sensoresMin[0]); //MAPEO DE SENSORES
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

//Los sensores IR estan en los canales io4-A7-A6-A0-A3-io26 de izquierda a derecha
float sensorValues[6];
float sensoresMax[6];
float sensoresMin[6];

bool linea=0;  // valor logico 1 corresponde a una pista con trayectoria color blanco; Valor lógico 0 corresponde a una pista con trayectoria color negro. 

void calibrarSensores() {
  for (int i = 0; i <= 5; i++) {
    sensoresMax[i] = 0;
    sensoresMin[i] = 1023;
  }
  int vcal=200;
  for(int i=0; i<6; i++){
    encoder_der.write(0);
    encoder_izq.write(0);
    while(abs(PI*radio_rueda*(encoder_der.read()+encoder_izq.read())/(50*12))< 10){
      motor(vcal,vcal);
      sensorValues[0]=digitalRead(4);
      sensorValues[1]=analogRead(A7);
      sensorValues[2]=analogRead(A6);
      sensorValues[3]=analogRead(A0);
      sensorValues[4]=analogRead(A3);
      sensorValues[5]=digitalRead(26);
      setMaxMin();
    }
    vcal=-vcal;
  }
  while((((1-2*linea)*100*(analogRead(A6)-sensoresMin[2])/(sensoresMax[2]-sensoresMin[2]))+ 100*linea)<80){
    motor(vcal,vcal);
  }
  encoder_der.write(0);
  encoder_izq.write(0);
  while((2*PI*radio_rueda*encoder_der.read()/(50*12)) < 11.2*PI/2){
    motor(0,vcal);
  }
}
void setMaxMin() {
  for (int i = 0; i <= 5; i++) {
    if (sensoresMax[i] < sensorValues[i]) {
      sensoresMax[i] = sensorValues[i];
    }
    if (sensoresMin[i] > sensorValues[i]) {
      sensoresMin[i] = sensorValues[i];
    }
  }
}

float getposition(){
  float sensorNormalizado[6];
  float pose_sensor=0;
  float inte[6];
  float sum1;
  float area;
  sensorNormalizado[0]=((1-2*linea)*100*(digitalRead(4)-sensoresMin[0])/(sensoresMax[0]-sensoresMin[0]))+ 100*linea; //MAPEO DE SENSORES
  sensorNormalizado[1]=((1-2*linea)*100*(analogRead(A7)-sensoresMin[1])/(sensoresMax[1]-sensoresMin[1])+ 100*linea);
  sensorNormalizado[2]=((1-2*linea)*100*(analogRead(A6)-sensoresMin[2])/(sensoresMax[2]-sensoresMin[2]))+ 100*linea;
  sensorNormalizado[3]=((1-2*linea)*100*(analogRead(A0)-sensoresMin[3])/(sensoresMax[3]-sensoresMin[3]))+ 100*linea;
  sensorNormalizado[4]=((1-2*linea)*100*(analogRead(A3)-sensoresMin[4])/(sensoresMax[4]-sensoresMin[4]))+ 100*linea;
  sensorNormalizado[5]=((1-2*linea)*100*(digitalRead(26)-sensoresMin[5])/(sensoresMax[5]-sensoresMin[5]))+ 100*linea;
 
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

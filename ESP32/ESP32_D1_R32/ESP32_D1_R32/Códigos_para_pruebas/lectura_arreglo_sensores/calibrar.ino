//Los sensores IR estan en los canales A18-A19-A7-A6-A0-A3 de izquierda a derecha
float sensorValues[6];
float sensoresMax[6];
float sensoresMin[6];

void calibrarSensores() {
  for (int i = 0; i <= 5; i++) {
    sensoresMax[i] = 0;
    sensoresMin[i] = 1023;
  }
  for (int i = 0; i < 300000; i++) {
    sensorValues[0]=analogRead(A18);
    sensorValues[1]=analogRead(A19);
    sensorValues[2]=analogRead(A7);
    sensorValues[3]=analogRead(A6);
    sensorValues[4]=analogRead(A0);
    sensorValues[5]=analogRead(A3);
    setMaxMin();
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
  sensorNormalizado[0]=100*(analogRead(A18)-sensoresMin[0])/(sensoresMax[0]-sensoresMin[0]);
  sensorNormalizado[1]=100*(analogRead(A19)-sensoresMin[1])/(sensoresMax[1]-sensoresMin[1]);
  sensorNormalizado[2]=100*(analogRead(A7)-sensoresMin[2])/(sensoresMax[2]-sensoresMin[2]);
  sensorNormalizado[3]=100*(analogRead(A6)-sensoresMin[3])/(sensoresMax[3]-sensoresMin[3]);
  sensorNormalizado[4]=100*(analogRead(A0)-sensoresMin[4])/(sensoresMax[4]-sensoresMin[4]);
  sensorNormalizado[5]=100*(analogRead(A3)-sensoresMin[5])/(sensoresMax[5]-sensoresMin[5]);
 
  for(int i=0; i<=5;i++){
    inte[i]=2*sensorNormalizado[i] - sensorNormalizado[i]*sensorNormalizado[i]/100;
  }  
  sum1=24*inte[0] + 14.4*inte[1] + 4.8*inte[2] - 4.8*inte[3] - 14.4*inte[4] - 24*inte[5];
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

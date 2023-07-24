//Los sensores IR estan en los canales A18-A19-A7-A6-A0-A3 de izquierda a derecha
void setup() {
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  digitalWrite(2, HIGH);
  Serial.begin(9600);
  //calibrarSensores();
  digitalWrite(2, LOW);
}

void loop() {
 // Serial.println(getposition(0));
  /*for(byte i=0;i<=11;i++){
    digitalWrite(25, i & 0x01  );
    digitalWrite(26, i & 0x02  );
    digitalWrite(2, i & 0x04  );
    digitalWrite(4, i & 0x08 );
    sensorValues[i]=analogRead(39);
  }
  for(byte i=0;i<=11;i++){
    Serial.print(sensorValues[i]);
    Serial.print("  -  ");
  }
  Serial.println();*/
  for(byte i=0;i<=15;i++){ // Cambiar for a 15 en el caso 39
    digitalWrite(25, i & 0x01  );
    digitalWrite(26, i & 0x02  );
    digitalWrite(2, i & 0x04  );
    digitalWrite(4, i & 0x08 );
    Serial.print(analogRead(39)); // 36 sensor central, 39 completo
    Serial.print("  -  ");}
  Serial.println();
}

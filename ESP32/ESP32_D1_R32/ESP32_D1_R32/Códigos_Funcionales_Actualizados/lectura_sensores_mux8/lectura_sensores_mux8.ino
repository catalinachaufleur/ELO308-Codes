
//Los sensores IR estan en los canales A18-A19-A7-A6-A0-A3 de izquierda a derecha
void setup() {
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  digitalWrite(2, HIGH);
  Serial.begin(9600);
  calibrarSensores();
  digitalWrite(2, LOW);
}

void loop() {
  Serial.print("Posicion Central:");
  Serial.println(getposition(1));

}

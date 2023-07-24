//Los sensores IR estan en los canales A18-A19-A7-A6-A0-A3 de izquierda a derecha
void setup() {
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  Serial.begin(9600);
  calibrarSensores();
  digitalWrite(2, LOW);
}

void loop() {
  Serial.println(getposition());
}

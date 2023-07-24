#include <Encoder.h>

Encoder encoder_der(23, 5);
Encoder encoder_izq(19, 18);
void setup() {
  Serial.begin(9600);

}

void loop() {
  Serial.print(encoder_izq.read());
  Serial.print(" - ");
  Serial.print(encoder_der.read());
  Serial.println();
}

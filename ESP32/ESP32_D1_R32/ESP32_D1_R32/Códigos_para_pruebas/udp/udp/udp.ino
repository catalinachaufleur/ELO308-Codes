//Author: Bc. Martin Chlebovec
//Personal website: https://arduino.php5.sk?lang=en
//DONATE FOR MORE FREE INSTRUCTABLES: https://paypal.me/chlebovec
#include "WiFi.h"
#include "AsyncUDP.h"
const char* ssid = "DPTO. WIFI BERGEN";
const char* pass = "110725011";
const int rele = 23;
AsyncUDP udp;
void setup()
{
  Serial.begin(115200);
  pinMode(rele, OUTPUT);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if (udp.listen(1234)) {
    Serial.print("UDP Listening on IP: ");
    Serial.println(WiFi.localIP());
    udp.onPacket([](AsyncUDPPacket packet) {
      Serial.print("UDP Packet Type: ");
      Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
      Serial.print(", From: ");
      Serial.print(packet.remoteIP());
      Serial.print(":");
      Serial.print(packet.remotePort());
      Serial.print(", To: ");
      Serial.print(packet.localIP());
      Serial.print(":");
      Serial.print(packet.localPort());
      Serial.print(", Length: ");
      Serial.print(packet.length()); 
      Serial.print(", Data: ");
      Serial.write(packet.data(), packet.length());
      Serial.println();
      String myString = (const char*)packet.data();
      //Serial.print(myString);
      if (myString == "ZAP") {
        Serial.println("prender rele");
        digitalWrite(rele, LOW);
      } else if (myString == "VYP") {
        Serial.println("apagar rele");
        digitalWrite(rele, HIGH);
      }
      packet.printf("Got %u bytes of data", packet.length());
    });
  }
}

void loop()
{
  delay(1000);
  udp.broadcast("Anyone here?");
}
#include <WiFi.h>
#include <analogWrite.h>

#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;
String velocidad = "0";
int vel;

WiFiServer server(1234);  // Define el objeto del servidor en el puerto 1234

const char* ssid = "fvp";              //"Turing";      // Nombre de la red Wi-Fi
const char* password = "nomeacuerdo";  //"PorgyB329";  // Contraseña de la red Wi-Fi

//const char* ssid = "Turing";      // Nombre de la red Wi-Fi
//const char* password = "PorgyB329";  // Contraseña de la red Wi-Fi

/*  Pines motor 1 (derecho)*/
#define ain1 16
#define ain2 27
#define pwm_a 17

/*  Pines motor 2 (izquierdo)*/
#define bin1  13
#define bin2  12
#define pwm_b  14

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Wire.begin();

  pinMode(ain1, OUTPUT);
  pinMode(ain2, OUTPUT);
  
  pinMode(bin1, OUTPUT); 
  pinMode(bin2, OUTPUT); 

  sensor.setTimeout(500);
  if (!sensor.init()) {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }
  sensor.setSignalRateLimit(0.25);
  sensor.setMeasurementTimingBudget(33000);
  sensor.startContinuous(100);  //toma medidas cada {Argumento} ms


  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.println("Conexión WiFi establecida");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  server.begin();  // Inicia el servidor
  Serial.println("Servidor iniciado");
}


void loop() {
  WiFiClient client = server.available();  // Espera a que se conecte un cliente
  if (client) {
    Serial.println("Cliente conectado");
    while (client.connected()) {
      if (client.available()) {
        String command = client.readStringUntil('\n');  // Lee el comando enviado por el cliente
        command.trim();                                 // Elimina espacios en blanco al inicio y al final
        Serial.print("Comando recibido: ");
        Serial.println(command);
        client.println("Comando recibido: " + command);  // Envía una respuesta al cliente
        //Serial.println("Esperando código");
    
        if (command == "On") {
          
          while (1) {
            velocidad = client.readStringUntil('\n');
            velocidad.trim();
            if (velocidad == "stop"){
              break;
            }
            if (velocidad != ""){
              vel = velocidad.toInt();
            } 
            Serial.println("Velocidad: " + vel);
            motor(vel,vel);
            client.println(sensor.readRangeContinuousMillimeters() * 0.1);
            Serial.print("Disntancia: ");
            Serial.println((sensor.readRangeContinuousMillimeters() * 0.1));        
          }
        }
      }
    }
    motor(0,0);
    client.stop();  // Cierra la conexión con el cliente
    Serial.println("Cliente desconectado");
}
  }
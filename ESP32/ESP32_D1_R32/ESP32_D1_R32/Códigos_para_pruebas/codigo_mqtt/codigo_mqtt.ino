#include <WiFi.h>
#include <PubSubClient.h>

///////////////////////////////////////
//     Configuración MQTT   ///////////
///////////////////////////////////////
//const char* ssid = "ac3e";  // Nombre WiFi a conectarse
//const char* password = "rac3e/07"; // Contraseña WiFi


const char* ssid = "DPTO. WIFI BERGEN"; //casa de Andrés
const char* password = "110725011";


//const char* mqtt_server = "192.168.137.1";
const char* mqtt_server = "192.168.100.12";  // ip del dispositivo que tiene instalado mosquitto (ip desktop Andres)
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];


///////////////////////////////////////////////////////////////////////////////
//           VARIABLES PID (ejemplo) pueden ser editadas por MQTT            //
///////////////////////////////////////////////////////////////////////////////
double dato_enviado = 0, mierror, v_medida, v_min = 1000;
double u = 0;       // Actuacion (?)
double 
setpoint = 0,   //valor de referencia del PID
error_velocidad;             //valor de entrada, mas adelante definido
//Output;            //salida que controla la velocidad del motor
double      //variables de las Konstantes
Kp = 10,
Ki = 0,
Kd = 0;
double
umin = -2200,  //valores maximinos y minimos que tomara el pid
umax = 2200;

////////////////////////////////////////////
//      Varaibles para código general //////
////////////////////////////////////////////
int dead = 1;     // flag buscar zona muerta
int deadband = 300;     //valor de deadband inicial en caso de no querer buscar deadband
int tiempo_inicial = 0;
bool flag = true; //flag para sincronizar
bool start = false; // variable para iniciar la prueba

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    WiFi.begin(ssid, password);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


///////////////////////////////////////////////////////////////

void callback(char* topic, byte* payload, unsigned int length) {
  String mensaje;
  for (int i = 0; i < length; i++) {
    char nuevo = (char)payload[i];
    mensaje.concat(nuevo);
  }
  Serial.println("Mensaje Recibido por MQTT");
  ////////////////////////////////
  // QUE HACE AL RECIBIR DATOS ///
  ////////////////////////////////
  
  if (strcmp(topic , "trenes/carrol/p") == 0) {
    Kp = mensaje.toFloat();
    Serial.println(Kp);
  }
  if (strcmp(topic , "trenes/carrol/i") == 0) {
    Ki = mensaje.toFloat();
    Serial.println(Ki);
  }
  if (strcmp(topic , "trenes/carrol/d") == 0) {
    Kd = mensaje.toFloat();
    Serial.println(Kd);
  }
  if (strcmp(topic , "trenes/carrol/velref") == 0) {
    dato_enviado = mensaje.toFloat();
    //Serial.println(Kd);
  }
  if (strcmp(topic , "trenes/carrol/u") == 0) {
    u = mensaje.toFloat();
    Serial.println(u);
  }

}

/////////////////////////////////////////////////////

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("CarroLider")) {  // "CarroLider" es el identificador de esta ESP. Debe ser distinto para cada micro. 
      Serial.println("connected");
      // LISTA SUBSCRIPCIONES (ejemplos para control pid)
      client.subscribe("trenes/carrol/p"); 
      client.subscribe("trenes/carrol/i");
      client.subscribe("trenes/carrol/d");
      client.subscribe("trenes/carrol/u");
      client.subscribe("trenes/sync");
      client.subscribe("trenes/carrol/velref");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
    Serial.begin(115200);
    Serial.println("setup!");
    /////////////////////////////////////
    ///    Inicio Comunicacion MQTT   ///
    /////////////////////////////////////
    setup_wifi(); // ver abajo
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    if (!client.connected()) {
        reconnect();
        Serial.println("Sub a los topicos ");
    }
    client.loop();
    //esp_wifi_set_ps(WIFI_PS_NONE);    // Modo No Power Saving, podria mejorar latencias

    Serial.println("Setup Completado");
    String ini = String(11111111, 2);
    ini.toCharArray(msg, ini.length() + 1);                    
    client.publish("esp32/estado", msg); //verificar estado de esp32 por MQTT
    tiempo_inicial = millis();
    
    
}


//////////////////////////////////////////////////////

void loop() {
  
  //Verificación de conexion MQTT
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
 
  //ejemplo: enviar v_medida por sensor por MQTT
  v_medida = 10; 
  String dato_enviado = String(v_medida, 2);
  dato_enviado.toCharArray(msg, dato_enviado.length() + 1);                    // Datos enviados para analizar controlador
  client.publish("trenes/carrol/dato_enviado", msg); 

  delay(1000);
      

  
}

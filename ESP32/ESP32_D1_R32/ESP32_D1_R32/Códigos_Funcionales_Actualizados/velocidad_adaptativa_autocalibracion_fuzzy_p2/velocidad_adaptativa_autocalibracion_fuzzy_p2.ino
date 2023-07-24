#include <PID_v1.h>
//Define Variables we'll be connecting to PID de posicion
double Input_theta, Output_theta;
double theta_ref=0;
double SampleTime_theta=2;
//Define the aggressive and conservative Tuning Parameters
/*PID posición*/
double Kp_theta = 100;
double Ki_theta = 350;
double Kd_theta = 1;

//Define Variables we'll be connecting to PID de Velocidad
double Input_vel, Output_vel;
double vel_ref=0;
double SampleTime_vel=5;
//Define the aggressive and conservative Tuning Parameters
/*PID velocidad*/
double Kp_vel = 150;
double Ki_vel = 250;
double Kd_vel = 0;

//Define Variables we'll be connecting to PID de distancia
double Input_d, Output_d;
double d_ref=10;
float delta=1;
double SampleTime_d=40;
//Define the aggressive and conservative Tuning Parameters
/*PID velocidad*/
double Kp_d = 4;
double Ki_d = 1;
double Kd_d = 0;
//Specify the links and initial tuning parameters
PID PID_theta(&Input_theta, &Output_theta, &theta_ref, Kp_theta, Ki_theta, Kd_theta, DIRECT);
PID PID_vel(&Input_vel, &Output_vel, &vel_ref, Kp_vel, Ki_vel, Kd_vel, DIRECT);
PID PID_d(&Input_d, &Output_d, &d_ref, Kp_d, Ki_d, Kd_d, REVERSE);
/////////////////////////////////////////////////////////////////////////////////////////////
///     WIFI                                                                        
///
//#include <esp_wifi.h>
#include <WiFi.h>
#include <PubSubClient.h>
//#include <WiFiUdp.h>
#include "AsyncUDP.h"

///////////////////////////////////////
//     Configuración MQTT   ///////////
///////////////////////////////////////
//const char* ssid = "ac3e";  // Nombre WiFi a conectarse
//const char* password = "rac3e/07"; // Contraseña WiFi


const char* ssid = "ESCOBAR"; //casa de Carlos
const char* password = "pruebaltx";


//const char* mqtt_server = "192.168.137.1";
const char* mqtt_server = "192.168.18.7";  // ip del dispositivo que tiene instalado mosquitto (ip desktop Carlos)
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
///
///     WIFI
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////7
#include <Encoder.h>
#include <Wire.h>
#include <VL53L0X.h>


//const int udpPort = 5555;                   //puerto de envio UDP*
String estado;
//WiFiUDP udp;
double t_old;
double t_v;
AsyncUDP udp;
////////////////////////

VL53L0X sensor;     //SENSOR DE DISTANCIA I2C

#define led 2
int radio_rueda=2; //RADIO EN [CM]
Encoder encoder_der(23, 5);
Encoder encoder_izq(19, 18);

// Variables de monitoreo

float mediciones[10]={0,0,0,0,0,0,0,0,0,0};

////////////////////// FUZZY //////////////////////////////
#include <Fuzzy.h>

Fuzzy *fuzzy = new Fuzzy();

// FuzzyInput
FuzzySet *pequenno = new FuzzySet(0, 0, 0, 6);
FuzzySet *mediano = new FuzzySet(1.5, 7, 7, 15);
FuzzySet *grande = new FuzzySet(7.5, 13.6, 15, 15);

// FuzzyInput
FuzzySet *recta = new FuzzySet(0, 0, 0, 8.3);
FuzzySet *curva_suave = new FuzzySet(0.5, 16, 16, 41);
FuzzySet *curva_cerrada = new FuzzySet(20, 41.6, 45, 45);


// FuzzyOutput
FuzzySet *simple = new FuzzySet(0, 0, 0.03, 0.18);
FuzzySet *doble_ponderado = new FuzzySet(0.04, 0.5, 0.5, 0.97);
FuzzySet *doble = new FuzzySet(0.8, 0.95, 1, 1);

float curva=0;

///////////////////////////////CONFIGURACION WIFI//////////////// 
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
  udp.listen(1234);
}
///////////////////////////////////////////////////////////////

void callback(char* topic, byte* payload, unsigned int length) {
  String mensaje;
  for (int i = 0; i < length; i++) {
    char nuevo = (char)payload[i];
    mensaje.concat(nuevo);
  }
  //Serial.println("Mensaje Recibido por MQTT");
  ////////////////////////////////
  // QUE HACE AL RECIBIR DATOS ///
  ////////////////////////////////
  
  if (strcmp(topic , "flota/robot0/controlador_posicion/p") == 0) {
    Kp_theta = mensaje.toFloat();
    //Serial.println(Kp_map);
    PID_theta.SetTunings(Kp_theta, Ki_theta, Kd_theta);
  }
  if (strcmp(topic , "flota/robot0/controlador_posicion/i") == 0) {
    Ki_theta = mensaje.toFloat();
    //Serial.println(Ki_map);
    PID_theta.SetTunings(Kp_theta, Ki_theta, Kd_theta);
  }
  if (strcmp(topic , "flota/robot0/controlador_posicion/d") == 0) {
    Kd_theta = mensaje.toFloat();
    //Serial.println(Kd_map);
    PID_theta.SetTunings(Kp_theta, Ki_theta, Kd_theta);
  }
  if (strcmp(topic , "flota/robot0/controlador_velocidad/p") == 0) {
    Kp_vel = mensaje.toFloat();
    //Serial.println(Kp_map);
    PID_vel.SetTunings(Kp_vel, Ki_vel, Kd_vel);
  }
  if (strcmp(topic , "flota/robot0/controlador_velocidad/i") == 0) {
    Ki_vel = mensaje.toFloat();
    //Serial.println(Ki_map);
    PID_vel.SetTunings(Kp_vel, Ki_vel, Kd_vel);
  }
  if (strcmp(topic , "flota/robot0/controlador_velocidad/d") == 0) {
    Kd_vel = mensaje.toFloat();
    //Serial.println(Kd_map);
    PID_vel.SetTunings(Kp_vel, Ki_vel, Kd_vel);
  }
  if (strcmp(topic , "flota/robot0/controlador_distancia/p") == 0) {
    Kp_d = mensaje.toFloat();
    //Serial.println(Kp_map);
    PID_d.SetTunings(Kp_d, Ki_d, Kd_d);
  }
  if (strcmp(topic , "flota/robot0/controlador_distancia/i") == 0) {
    Ki_d = mensaje.toFloat();
    //Serial.println(Ki_map);
    PID_d.SetTunings(Kp_d, Ki_d, Kd_d);
  }
  if (strcmp(topic , "flota/robot0/controlador_distancia/d") == 0) {
    Kd_d = mensaje.toFloat();
    //Serial.println(Kd_map);
    PID_d.SetTunings(Kp_d, Ki_d, Kd_d);
  }
  if (strcmp(topic , "flota/robot0/controlador_distancia/distancia") == 0) {
    d_ref = mensaje.toFloat();
    //Serial.println(Kd);
  }
  if (strcmp(topic , "flota/robot0/controlador_distancia/delta") == 0) {
    delta = mensaje.toFloat();
    //Serial.println(Kd);
  }  
  if (strcmp(topic , "flota/robot0/fuzzy/curva") == 0) {
    curva = mensaje.toFloat();
  }
/*  if (strcmp(topic , "flota/robot0/vel") == 0) {
    //vel_ref = mensaje.toFloat();
    vel_ref = 0;
    //Serial.println(Kd);
  }
/*  if (strcmp(topic , "trenes/carrol/u") == 0) {
    u = mensaje.toFloat();
    Serial.println(u);
  }
*/
}

/////////////////////////////////////////////////////

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("RobotZero")) {  // "RobotLider" es el identificador de esta ESP. Debe ser distinto para cada micro. 
      Serial.println("connected");
      // LISTA SUBSCRIPCIONES (ejemplos para control pid)
      client.subscribe("flota/robot0/controlador_posicion/p"); 
      client.subscribe("flota/robot0/controlador_posicion/i");
      client.subscribe("flota/robot0/controlador_posicion/d");
      client.subscribe("flota/robot0/controlador_velocidad/p"); 
      client.subscribe("flota/robot0/controlador_velocidad/i");
      client.subscribe("flota/robot0/controlador_velocidad/d");
      client.subscribe("flota/robot0/controlador_distancia/p"); 
      client.subscribe("flota/robot0/controlador_distancia/i");
      client.subscribe("flota/robot0/controlador_distancia/d");
      client.subscribe("flota/robot0/controlador_distancia/delta");
      client.subscribe("flota/robot0/controlador_distancia/distancia");
      client.subscribe("flota/robot0/fuzzy/curva");
      //client.subscribe("flota/robot0/u");
      //client.subscribe("flota/sync");
      //client.subscribe("flota/robot0/vel");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
///////////////////////////////////////////////////////////////

void setup() 
{
  Serial.begin(115200);
  Serial.println("setup!");
  /////////////////////////////////////
  ///    Inicio Comunicacion MQTT   ///
  /////////////////////////////////////
  WiFi.mode(WIFI_STA);
  //esp_wifi_set_ps(WIFI_PS_NONE);
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
//  
// CONFIGURACION SENSOR DE DISTANCIA ////////////////////////////////////                         
  Wire.begin();                                                       ///
  sensor.setTimeout(500);                                             ///
  if (!sensor.init())                                                 ///
  {                                                                   ///
    Serial.println("Failed to detect and initialize sensor!");        ///
    while (1) {}                                                      ///
  }                                                                   ///
  sensor.setSignalRateLimit(0.25);                                    ///
  sensor.setMeasurementTimingBudget(33000);                           ///
  sensor.startContinuous(100); //toma medidas cada {Argumento} ms     ///
/////////////////////////////////////////////////////////////////////////

//turn the PID on
  PID_theta.SetMode(MANUAL);
  PID_theta.SetSampleTime(SampleTime_theta);
  PID_theta.SetOutputLimits(-1023,1023);
  Output_theta=0;  
//
//turn the PID on
  PID_vel.SetMode(AUTOMATIC);
  PID_vel.SetSampleTime(SampleTime_vel);
  PID_vel.SetOutputLimits(-1023,1023);
//
//turn the PID on
  PID_d.SetMode(AUTOMATIC);
  PID_d.SetSampleTime(SampleTime_d);
  PID_d.SetOutputLimits(-40,40);
//
// FUZZY
  set_fuzzy();
//
  pinMode(led, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  digitalWrite(led, HIGH);
  calibrarSensores();
  digitalWrite(led, LOW);
  encoder_der.write(0);
  encoder_izq.write(0);
}


void loop() 
{
//  client.loop(); // Recibe variables desde MQTT
  /*
  unsigned long T;
  if (micros() > T  + h*1000000)
  {      
    u = PID_NN(Kp_map, Ki_map, Kd_map);
    velocidad(Vel - u, Vel + u);
    T = micros();
  }
*/
/*Serial.println(getposition()); 

  Serial.print("         :       ");
  Serial.print(digitalRead(4));
  Serial.print(" ");
  Serial.print(analogRead(A7));
  Serial.print(" ");
  Serial.print(analogRead(A6));
  Serial.print(" ");
  Serial.print(analogRead(A0));
  Serial.print(" ");
  Serial.print(analogRead(A3));
  Serial.print(" ");
  Serial.print(digitalRead(26));
  Serial.println();
*/
//Serial.print(encoder_der.read());
//Serial.print("            ");
//Serial.println(encoder_izq.read());
//motor(Vel,-Vel);
//Serial.println(sensor.readRangeContinuousMillimeters()*0.1);
//Input=0.5*2*(encoder_der.read()+encoder_izq.read())*1000/(SampleTime);
Input_d=sensor.readReg16Bit(sensor.RESULT_RANGE_STATUS + 10)*0.1;
//fuzzy
fuzzy->setInput(1, abs(d_ref-Input_d));
fuzzy->setInput(2, curva);

fuzzy->fuzzify();
float alpha_fuzzy = fuzzy->defuzzify(1);

//////////////////////////////////////////////////////
if(Input_d>=40){
  Input_d=40;
}
if(abs(d_ref-Input_d)>=delta){
  PID_theta.SetMode(AUTOMATIC);
  PID_vel.SetMode(AUTOMATIC);
  PID_d.SetMode(AUTOMATIC);
}
uint32_t time_now = millis() - t_v;
if(time_now>=2){
  Input_vel=PI*radio_rueda*(encoder_der.read()+encoder_izq.read())*1000/(time_now*210*12); // ENCODERS:12CPR---CAJA_REDUCTORA:210:1
  curva=5*abs(encoder_der.read()-encoder_izq.read());
  mediciones[3]=Input_vel;
  mediciones[4]=2*PI*radio_rueda*encoder_der.read()*1000/(time_now*210*12);
  mediciones[5]=2*PI*radio_rueda*encoder_izq.read()*1000/(time_now*210*12);
  encoder_der.write(0);
  encoder_izq.write(0);
  t_v=millis();
}
if(PID_d.Compute()){
  mediciones[0]=Input_d;
  mediciones[1]=d_ref;
}
vel_ref=Output_d*alpha_fuzzy + (1-alpha_fuzzy)*8;
PID_vel.Compute();
/*if(PID_vel.Compute()){
  mediciones[3]=Input_vel;
  mediciones[4]=2*PI*radio_rueda*encoder_der.read()*1000/(SampleTime_vel*210*12);
  mediciones[5]=2*PI*radio_rueda*encoder_izq.read()*1000/(SampleTime_vel*210*12);
  encoder_der.write(0);
  encoder_izq.write(0);
}*/
if(Output_vel>0 && (abs(d_ref-Input_d)>=delta)){
//  Kp_theta = 100;
//  Ki_theta = 350;
//  Kd_theta = 1;
  Input_theta=getposition(1);
  if(PID_theta.Compute()){
    mediciones[7]=Input_theta;
    mediciones[8]=1;
  }
  motor(Output_vel-Output_theta,Output_vel+Output_theta); 
}
else if(Output_vel<0&& (abs(d_ref-Input_d)>=delta)){
//  Kp_theta = 30;
//  Ki_theta = 0;
//  Kd_theta = 5;
  Input_theta=getposition(0);
  if(PID_theta.Compute()){
    mediciones[7]=Input_theta;
    mediciones[8]=0;
  }
  motor(Output_vel+Output_theta,Output_vel-Output_theta);
}
else if(Output_vel==0 || (abs(d_ref-Input_d)<delta)){
  PID_d.SetMode(MANUAL);
  PID_vel.SetMode(MANUAL);
  PID_theta.SetMode(MANUAL);
  Output_d=0;
  Output_vel=0;
  Output_theta=0;
  motor(Output_vel-Output_theta,Output_vel+Output_theta);
}
mediciones[2]=vel_ref;
mediciones[6]=Output_vel;
mediciones[9]=Output_theta;

/*t_old = millis();
getposition(0);
uint32_t time_now = millis() - t_old;
Serial.println(time_now);
*/
time_now = millis() - t_old;
if(time_now>=40){
  estado = "S,"+String(mediciones[0])+","+String(mediciones[1])+","+String(mediciones[2])+","+String(mediciones[3])+","+String(mediciones[4])+","+String(mediciones[5])+",";
  estado.toCharArray(msg, estado.length() + 1);  
  udp.broadcast(msg);
  estado = String(curva)+","+String(alpha_fuzzy)+","+String(mediciones[6])+","+String(mediciones[7])+","+String(mediciones[8])+","+String(mediciones[9])+",E";
  //estado = String(mediciones[0])+","+String(mediciones[1])+","+String(mediciones[2])+","+String(mediciones[3])+","+String(mediciones[4])+","+String(mediciones[5])+","+String(mediciones[6])+","+String(mediciones[7])+","+String(mediciones[8])+","+String(mediciones[9])+","+String(curva)+","+String(alpha_fuzzy);
  estado.toCharArray(msg, estado.length() + 1);  
  udp.broadcast(msg);
  t_old = millis(); 
}

//estado = String(time_now)+","+String(mediciones[0])+","+String(mediciones[1])+","+String(mediciones[2])+","+String(mediciones[3])+","+String(mediciones[4])+","+String(mediciones[5])+","+String(mediciones[6])+","+String(mediciones[7])+","+String(mediciones[8])+","+String(mediciones[9]);
//estado.toCharArray(msg, estado.length() + 1);                                                                           

//Serial.println(estado);
//client.publish("esp/datos", msg);
//------------------------------------
//udp.beginPacket(mqtt_server,udpPort);
//udp.printf(msg);      //
//udp.endPacket();

}

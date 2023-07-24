#include <PID_v1.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "SPI.h"
#include <Wire.h>
#include <VL53L0X.h>
#include <esp_wifi_types.h>
#include <esp_wifi_internal.h>
#include <esp_wifi.h>
  
////////////////////////////////////////////
///////////////////////////////////////////
String carro = "carro" + String(random(100));
//const char carro[] = "carroD2";
char treneserror[] = "trenes/carroD/error";
char trenesp[] = "trenes/carroD/p";
char trenesi[] = "trenes/carroD/i";
char trenesd[] = "trenes/carroD/d";
char trenesp_v[] = "trenes/carroD/p_v";
char trenesi_v[] = "trenes/carroD/i_v";
char trenesd_v[] = "trenes/carroD/d_v";
char trenesdesfase[] = "trenes/carroD/desfase";
char trenesestado[] = "trenes/estado/carroD"; 
char trenestest[] = "trenes/carroD/test";     // Donde se envian los datos a analizar separados por ","
char trenessampletime[] = "trenes/carroD/ts"; // Lista para modificar el tiempo de sampleo del PID
char trenestransmision[] = "trenes/envio";    // Lista para modificar el tiempo de transmision
int t_envio = 50;                             // Tiempo de transmision por defecto;
///////////////////////////////////////////
/////////////////////////////////////////// 

struct MD
{
    byte motion;
    char dx, dy;
    byte squal;
    word shutter;
    byte max_pix;
    int over;
};


MD md;
int count_cam=0;


int yy=0;  // coordenadas obtenidas por sensor ADNS3080 (dead reckoning)
double temp_cal;
double scale; // escalamiento de cuentas ADNS a cm 
double medi = 10;
double xx = 10;
double temp_cal_final;
double temp_cal_dif;
int vel_est;
int Gain_K;

// filtro ToF
int INDEX = 0;
int VALUE = 0;
int SUM = 0;
int READINGS[10];
double AVERAGED = 0;

// filtro ADNS3080
int INDEXv = 0;
double VALUEv = 0;
double SUMv = 0;
double READINGSv[10];
double AVERAGEDv = 0;

//moving avg
int arrNumbers[5] = {0};

int pos = 0;
float newAvg = 0;
float oldAvg = 0;
int newSum = 0;
long sum = 0;
int len = sizeof(arrNumbers) / sizeof(int);
//int count = sizeof(sample) / sizeof(int);




///////////////////////////////////////
//     Configuracion MQTT   ///////////
///////////////////////////////////////
const char* ssid = "TUWIFI";
const char* password = "TUPASS";
const char* mqtt_server = "TU DIRECCION IPV4";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;  
char msg[50];


////////////////////////////////////////////////////
//          Configuracion motor DC      ////////////
////////////////////////////////////////////////////
const int Control_fwd = 25;                //  Pin AIN1  [Control del sentido de rotaciÃ³n +]
const int Control_back = 26;            //  Pin AIN2   [Control del sentido de rotaciÃ³n -]
const int Control_v = 12;                 //  Pin PWMA    [Control de la velocidad de rotaciÃ³n]
int MotorSpeed = 0;                   // Velocidad del motor  0..1024
int MotorDirection = 1;               // Avanzar (1) o Retroceder (0)

////////////////////////////////////////////////
//         Variables para Sensor Distancia /////
////////////////////////////////////////////////
VL53L0X SensorToF;
double distancia = 25;
double old_d = 0;                         
double old = 0;  
double t_distancia = 0;
double t_old;
double t1; // para temporizar la medición de la velocidad
double t2;

////////////////////////////////////////////
//      Variables para codigo general //////
////////////////////////////////////////////
int dead = 1;     // flag buscar zona muerta
int deadband = 300;     //valor de deadband inicial en caso de no querer buscar deadband
int tiempo_inicial = 0;
bool flag = true; //flag para sincronizar
bool start = false; // variable para iniciar la prueba
String estado;      //Variable para saber el estado del carro

////////////////////////////////////////////
// Variables y parametros PIDs /////////////
////////////////////////////////////////////
double v_lider = 0, mierror;
float v_medida = 0;
float v_ref = 0;
double x_ref = 10;
double u_distancia;                         //Actuacion calculada por distancia                           
double u_velocidad;                         //Actuacion calculada por velocidad
double u;                                   //Actuacion ponderada
int umin = -400, umax = 400;//C5
double Kp = 40, Ki = 20, Kd = 0;             //Ganancias PID control de distancia
double Kp_v = 20, Ki_v = 5, Kd_v = 0;       //Ganancias PID control de velocidad   
int SampleTime = 100;                       //Tiempo de muestreo ambos controladores
double etha = 0.5;
int Frequency = 100;
int PreviousFrequency;
int deadband1 = 0;
double alpha = 0; // constante para el control con velocidad lider

double N = 10; //Constante Filtro Derivativa D(s)=s/(1+s/N)
double error_distancia;
double error_velocidad;
double ponderado;
double rf = 0;
int lim = 10;
PID myPID(&error_distancia, &u_distancia, &rf, Kp, Ki, Kd, DIRECT);
PID myPID_v(&error_velocidad, &u_velocidad, &rf, Kp_v, Ki_v, Kd_v, DIRECT);

//////////////////////////////////////////////////////

void loop() {
    if (!client.connected()) {
        reconnect();
        estado = String(carro) + " Reconectando";
        estado.toCharArray(msg, estado.length() + 1);                                                                           
        client.publish(trenesestado, msg);
        Serial.println("vivo2");
    }
    
    client.loop();
    if(flag==false){   // Envia desfase para sincronizar datos de la prueba
        String delta = String((millis()-tiempo_inicial)*0.001);
        delta.toCharArray(msg, delta.length() + 1);                                                                           
        client.publish(trenesdesfase, msg);
        flag = true;
        start = true;
        Serial.println("Sync!");
    }  // Da comienzo a la prueba

    else if (start)
    {   
        xx=medi; // memoria para filtrar medi (medicion de distancia)
        medi=0;
      
        count_cam += (int8_t) md.dy;        
        old=micros();
        SUM=count_cam;
        for (int i = 0; i < 10 ; i++) {
            if (i % 5 == 1) {    // solo algunas mediciones de ToF. No se actualiza muy seguido. Pero ya no hay tiempo perdido entre mediciones.
                uint16_t range = SensorToF.readReg16Bit(SensorToF.RESULT_RANGE_STATUS + 10);
                medi=medi+range;
                
            }
            mousecam_read_motion(&md);
            count_cam += (int8_t) md.dy;
            delayMicroseconds(3000);
        }

        medi = medi/20;
        mousecam_read_motion(&md);
        count_cam += (int8_t) md.dy;// medición de distancia entre old y la última medición
        v_medida=((scale*1000000)*(count_cam-SUM))/((micros()-old)); 
        newSum = movingSum(arrNumbers, &sum, pos, len, v_medida);
        newAvg = newSum/(float)len;
        
        pos++;
        if (pos >= len){
          pos = 0;
        }
        Serial.print(medi);
        Serial.print(" ,");
        Serial.print(v_medida);
        Serial.print(" ,");
        Serial.println(0.7*newAvg+0.3*oldAvg);

        oldAvg = newAvg;
            
        error_distancia = (x_ref - medi);
        error_velocidad = (0.7*newAvg+0.3*oldAvg - v_ref); 
    
        myPID.SetTunings(Kp, Ki, Kd);
        myPID.SetOutputLimits(umin, umax);
        myPID.Compute();
        myPID.SetSampleTime(SampleTime);
        myPID_v.SetTunings(Kp_v, Ki_v, Kd_v);
        myPID_v.SetOutputLimits(umin, umax);
        myPID_v.Compute();
        myPID_v.SetSampleTime(SampleTime);
    
        ponderado = abs(v_medida);  
        
        u = (1 - etha) * u_distancia + etha * ( u_velocidad); 
  
          u =  u_distancia + alpha * ( u_velocidad); 
          u=u_distancia;
        // Envio cada 50 ms
    
        ////////////////////////////////////////////////////////
        //        Rutina del Carro                 /////////////
        ////////////////////////////////////////////////////////
    
        if ((medi > 200))   // si no hay nada enfrente: detenerse o desacelerar
        {
            if (MotorSpeed < 200) MotorSpeed = MotorSpeed -  10 ; 
            else MotorSpeed = 0; 
            myPID.SetMode(MANUAL); // para que no siga integrando
            myPID_v.SetMode(MANUAL); // para que no siga integrando
        }
  
        else
        {
            if (u < -lim)  //lim es un valor arbitrario donde se desea que el carro no se mueva si |u|<=lim
            {
                MotorDirection = 0;
                MotorSpeed = int(-u + deadband ); //- 40);
            }
            else if (u > lim)
            {
                MotorDirection = 1;
                MotorSpeed = int(u + deadband ); //- 40);
            }
  
            if (((u >= -lim)) && ((u <= lim)))
            {
                myPID.SetMode(MANUAL);            // apaga el PID de distancia
                myPID_v.SetMode(MANUAL);          // apaga el PID de velocidad
                MotorSpeed = 1.5 * int(abs(u));
                if (u > 0)  MotorDirection = 1;
                else  MotorDirection = 0;
            }
  
            if (((u >= -lim) && (u <= lim) && (abs(ponderado) <= 0.75))) // detener si estamos en reposo
            {
                MotorSpeed = 1;
                if (u < 0)  MotorDirection = 1;
                else  MotorDirection = 0;
            }
            if( (medi < 200)){ // condicion para revivir el PID
                myPID.SetMode(AUTOMATIC);
                myPID_v.SetMode(AUTOMATIC);
            }        
        }
        
        int test_sin_control=0;
        if (test_sin_control)
        {
          myPID.SetMode(MANUAL);            // apaga el PID de distancia
          myPID_v.SetMode(MANUAL);          // apaga el PID de velocidad
          MotorSpeed=0;
        }
        
        
        SetMotorControl();
    }
}

/////////////////////////////////////////////////////////////

void SetMotorControl()
{
    if (MotorDirection == 1)            //Avanzar
    {
        digitalWrite(Control_fwd, LOW);
        digitalWrite(Control_back, HIGH);
    }
    else                                //Retroceder
    {
        digitalWrite(Control_fwd, HIGH);
        digitalWrite(Control_back, LOW);
    }
    
    ledcWrite(Control_v, MotorSpeed); // esp32
}


/* MQTT Callback */
void callback(char* topic, byte* payload, unsigned int length) {
    String mensaje;
    for (int i = 0; i < length; i++) {
        char nuevo = (char)payload[i];
        mensaje.concat(nuevo);
    }
    ////////////////////////////////
    // QUE HACE AL RECIBIR DATOS ///
    ////////////////////////////////
  
    if (String(topic) == "trenes/sync") {
        if (String(mensaje)=="True") {
            tiempo_inicial = millis();
            Serial.println("Sync Recibido");
            flag = false;
            myPID.SetMode(AUTOMATIC);            // prende el PID de distancia  
            myPID_v.SetMode(AUTOMATIC);          // prende el PID de velocidad   
        }
        else if (String(mensaje)=="False"){
            Serial.println("Detener");
            flag = true;
            start = false;
            MotorSpeed = 0;
            SetMotorControl();
            myPID.SetMode(MANUAL);            // apaga el PID de distancia  
            myPID_v.SetMode(MANUAL);          // apaga el PID de velocidad     
            u_distancia = 0;
            u_velocidad = 0; 
        }
    }
    if (strcmp(topic , "setear/freq") == 0) {
        Serial.println("Freq Recibido");
        MotorDirection = 1;
        MotorSpeed = 0;
        SetMotorControl();
        flag = true;
        start = false;
        myPID.SetMode(MANUAL);            // apaga el PID de distancia  
        myPID_v.SetMode(MANUAL);          // apaga el PID de velocidad     
        u_distancia = 0;
        u_velocidad = 0; 
        deadband = Busca_Dead(mensaje.toInt());
        MotorSpeed = 0;
        SetMotorControl();
        flag = true;
        start = false;
        myPID.SetMode(MANUAL);            // apaga el PID de distancia  
        myPID_v.SetMode(MANUAL);          // apaga el PID de velocidad     
        u_distancia = 0;
        u_velocidad = 0; 
    }        
    if (strcmp(topic ,trenesp) == 0) {
        Kp = mensaje.toFloat();
        Serial.println("Kp Recibido");
    }
    if (strcmp(topic , trenesi) == 0) {
        Ki = mensaje.toFloat();
    }
    if (strcmp(topic , trenesd) == 0) {
        Kd = mensaje.toFloat();
    }
    if (strcmp(topic ,trenesp_v) == 0) {
        Kp_v = mensaje.toFloat();
    }
    if (strcmp(topic , trenesi_v) == 0) {
        Ki_v = mensaje.toFloat();
    }
    if (strcmp(topic , trenesd_v) == 0) {
        Kd_v = mensaje.toFloat();
    }
    if (strcmp(topic , "trenes/etha") == 0) {
        etha = mensaje.toFloat();
    }
    if (strcmp(topic , "trenes/u_lim") == 0) {
        umax = mensaje.toInt();
        umin = -umax;
    }
    if (strcmp(topic , "trenes/carroL/v_lider") == 0) {
        v_lider = mensaje.toFloat();
    }
    if (strcmp(topic , "trenes/carroL/vref") == 0) {
        v_ref = mensaje.toFloat();
    }
    if (strcmp(topic , "trenes/ref") == 0) {
        x_ref = mensaje.toFloat();
    }
    if (strcmp(topic , trenessampletime) == 0) {
        SampleTime = mensaje.toInt();
    }
    if (strcmp(topic , trenestransmision) == 0) {
        t_envio = mensaje.toInt();
    }
}

float movingSum(int *ptrArrNumbers, long *ptrSum, int pos, int len, int nextNum)
{
  *ptrSum = *ptrSum - ptrArrNumbers[pos] + nextNum;
  ptrArrNumbers[pos] = nextNum;
  return *ptrSum;
}

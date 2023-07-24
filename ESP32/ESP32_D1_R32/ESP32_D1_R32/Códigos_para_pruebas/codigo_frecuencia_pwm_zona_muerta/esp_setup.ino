////////////////////////////////////////////
//                                        //
//       DEFINICIONES SENSOR OPTICO       //
//                                        //
////////////////////////////////////////////

//#define PIN_SS       D8
#define PIN_MISO 19
#define PIN_MOSI 23
#define PIN_SCK 18

#define PIN_MOUSECAM_RESET 14
#define PIN_MOUSECAM_CS 16 //D3

#define ADNS3080_PIXELS_X 30
#define ADNS3080_PIXELS_Y 30

#define ADNS3080_PRODUCT_ID 0x00
#define ADNS3080_REVISION_ID 0x01
#define ADNS3080_MOTION 0x02
#define ADNS3080_DELTA_X 0x03
#define ADNS3080_DELTA_Y 0x04
#define ADNS3080_SQUAL 0x05
#define ADNS3080_PIXEL_SUM 0x06
#define ADNS3080_MAXIMUM_PIXEL 0x07
#define ADNS3080_CONFIGURATION_BITS 0x0a
#define ADNS3080_EXTENDED_CONFIG 0x0b
#define ADNS3080_DATA_OUT_LOWER 0x0c
#define ADNS3080_DATA_OUT_UPPER 0x0d
#define ADNS3080_SHUTTER_LOWER 0x0e
#define ADNS3080_SHUTTER_UPPER 0x0f
#define ADNS3080_FRAME_PERIOD_LOWER 0x10
#define ADNS3080_FRAME_PERIOD_UPPER 0x11
#define ADNS3080_MOTION_CLEAR 0x12
#define ADNS3080_FRAME_CAPTURE 0x13
#define ADNS3080_SROM_ENABLE 0x14
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_LOWER 0x19
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_UPPER 0x1a
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_LOWER 0x1b
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_UPPER 0x1c
#define ADNS3080_SHUTTER_MAX_BOUND_LOWER 0x1e
#define ADNS3080_SHUTTER_MAX_BOUND_UPPER 0x1e
#define ADNS3080_SROM_ID 0x1f
#define ADNS3080_OBSERVATION 0x3d
#define ADNS3080_INVERSE_PRODUCT_ID 0x3f
#define ADNS3080_PIXEL_BURST 0x40
#define ADNS3080_MOTION_BURST 0x50
#define ADNS3080_SROM_LOAD 0x60

#define ADNS3080_PRODUCT_ID_VAL 0x17

void setup()
{

    Serial.begin(115200);
    Serial.println("");
    /////////////////////////////////////
    ///       Inicio Motor         //////
    /////////////////////////////////////
    pinMode(Control_fwd, OUTPUT);  // 1A - Definicion Pin como salida
    pinMode(Control_back, OUTPUT); // 2A - Definicion Pin como salida
    pinMode(Control_v, OUTPUT);    // 1,2 EN - Definicion Pin como salida
    digitalWrite(Control_v, HIGH); // Motor off - O Volts (HIGH = 0, por algÃºn motivo)
    ledcSetup(Control_v, 100, 10); //Freq a 100 Hz ESP32 resolución 10 bits
    ledcAttachPin(12, Control_v);

    /////////////////////////////////////
    ///  Inicio Comunicacion ESP-NOW  ///
    /////////////////////////////////////

    WiFi.mode(WIFI_STA);
    // Solo usar las lineas de abajo para 'custom' wifi
    //WiFi.disconnect();
    //setup_custom_wifi();
    esp_wifi_set_ps(WIFI_PS_NONE);



    /////////////////////////////////////
    ///    Inicio Comunicacion MQTT   ///
    /////////////////////////////////////
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    if (!client.connected())
    {
        reconnect();
        Serial.println("Sub a los topicos ");
    }
    client.loop();

    uint8_t ch;
    wifi_second_chan_t ch2;
    esp_wifi_get_channel(&ch, &ch2);
    float freq = 2.407 + 0.005 * (int)ch;
    Serial.printf("Conectado al canal Wi-Fi: %d (%.3f GHz)\n", ch, freq);
    //wifi_ps_type_t(WIFI_PS_NONE);     // Modo No Power Saving

    ///////////////////////////////////////////
    //         Inicio de Sensores           ///
    ///////////////////////////////////////////
    Wire.begin();
    SensorToF.init();
    SensorToF.setTimeout(500);
    SensorToF.setMeasurementTimingBudget(20000);
    SensorToF.startContinuous();
    //VL53L0X_RangingMeasurementData_t measure;
    //lox.rangingTest(&measure, false);

    //  Serial.println("Iniciando Adafruit VL53L0X");
    //  if (!lox.begin()) {
    //    Serial.println(F("Fallo en inicio VL53L0X"));
    //    while(1);
    //  }
    //  lox.rangingTest(&measure, false);
    //
    //  if (measure.RangeStatus != 4)
    //  {
    //    distancia = measure.RangeMilliMeter*0.1;
    //  }
    //  Serial.println(distancia);
    //  Serial.println("VL53L0X iniciado correctamente");
    pinMode(PIN_MISO, INPUT);
    pinMode(PIN_MOSI, OUTPUT);
    pinMode(PIN_SCK, OUTPUT);
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    SPI.setDataMode(SPI_MODE3);
    SPI.setBitOrder(MSBFIRST);
    Serial.println("Iniciando Cámara");
    if (mousecam_init() == -1)
    {
        Serial.println("Fallo en inicio Cámara");
        while (0)
            ;
    }
    Serial.println("Cámara Iniciada Correctamente");
    MD md;

    double xx = 0; // memoria para filtrar medi (medicion de distancia)
    uint16_t range = 0;
    int WINDOW_SIZE = 5;
    int WINDOW_SIZEv = 5;
    Serial.println("Iniciando Deadband");

    mousecam_read_motion(&md);
    range = SensorToF.readReg16Bit(SensorToF.RESULT_RANGE_STATUS + 10);
    medi = range;
    //Serial.print(md.squal);
    //Serial.print("   ");

    int Frequencies[] = {200};
    
    int len2 = sizeof(Frequencies) / sizeof(int);

    for (int i = 0; i < len2; i++)
    {
        deadband = Busca_Dead(Frequencies[i]);
        
    }
    Serial.println(Frequency);
    if (!dead) // Una vez detectada la zona muerta, calibrar sensor ADNS3080
    {
        // Medir distancia a objeto en reposo
        medi = 0;
        uint16_t range = SensorToF.readReg16Bit(SensorToF.RESULT_RANGE_STATUS + 10); // por alguna razón la primera medición de todo el loop sale muy mal. Posiblemente por el timebudget
        delay(200);
        for (int i = 0; i < 8; i++)
        {
            range = SensorToF.readReg16Bit(SensorToF.RESULT_RANGE_STATUS + 10);

            medi = medi + range;
            // Serial.println(medi);
            delay(20);
        }
        temp_cal = medi / 80;
        Serial.print("Distancia Antes de tranciente: "); //suponiendo que son 500 ms
        Serial.println(temp_cal);
        Serial.print(" ");
        int yy1 = 0;
        MD md;
        MotorSpeed = 200;
        SetMotorControl();
        old = millis();

        while (millis() - old < 3000)
        {
          
            mousecam_read_motion(&md);

            yy1 = yy1 + (int8_t)md.dy;
            delayMicroseconds(2800);
        }
        MotorSpeed = 0;
        SetMotorControl();
        while (millis() - old < 1100)
        {
            mousecam_read_motion(&md);

            yy1 = yy1 + (int8_t)md.dy;
            delayMicroseconds(800);
        }
        delay(500); //asegurar que se detuvo
        
        medi = 0;
        range = SensorToF.readReg16Bit(SensorToF.RESULT_RANGE_STATUS + 10); // por alguna razón la primera medición de todo el loop sale muy mal. Posiblemente por el timebudget
        delay(200);
        for (int i = 0; i < 8; i++)
        {
            range = SensorToF.readReg16Bit(SensorToF.RESULT_RANGE_STATUS + 10);

            medi = medi + range;
            // Serial.println(medi);
            delay(20);
        }
        temp_cal_final = medi / 80;
       
        for (int i = 0; i < 8; i++)
        {
            range = SensorToF.readReg16Bit(SensorToF.RESULT_RANGE_STATUS + 10);
            medi = medi + range;
            delay(20);
        }
        medi = medi / 80;
        Serial.println(medi);
        Serial.println(yy1);

        scale = (temp_cal - medi) / yy1;
        Serial.println(scale, 4);

// Estimar ganancia
        medi = 0;
        MotorDirection = 0; //hago que retroceda
        MotorSpeed = 200;
        SetMotorControl();
        delay(500);
        range = SensorToF.readReg16Bit(SensorToF.RESULT_RANGE_STATUS + 10);
        delay(1000);
        medi = SensorToF.readReg16Bit(SensorToF.RESULT_RANGE_STATUS + 10);
        MotorSpeed = 0;
        SetMotorControl();
        vel_est = ((range - medi)/10)/1;   //dividido en 1 porque avanzo 1 seg
    int Gain_K_BW = vel_est/200;   //ganancia sin zona muerta
    int Gain_K_DB_BW = vel_est/(200 - deadband);  //ganancia con zona muerta

        delay(500);
        MotorDirection = 1; //hago que retroceda
        MotorSpeed = 200;
        SetMotorControl();
        delay(500);
        range = SensorToF.readReg16Bit(SensorToF.RESULT_RANGE_STATUS + 10);
        delay(1000);
        medi = SensorToF.readReg16Bit(SensorToF.RESULT_RANGE_STATUS + 10);
        MotorSpeed = 0;
        SetMotorControl();
        vel_est = ((range - medi)/10)/1;   //dividido en 1 porque avanzo 1 seg
    int Gain_K_FW = vel_est/200;   //ganancia sin zona muerta
    int Gain_K_DB_FW = vel_est/(200 - deadband);  //ganancia con zona muerta
        
    }

    MotorSpeed = 0;
    SetMotorControl();
    old = 0;
    Serial.println("Setup Completado");
    String ini = String(11111111, 2);       // + ", " + String(millis());
    ini.toCharArray(msg, ini.length() + 1); // Datos enviados para analizar controlador
    client.publish("trenes/carroL/v_lider", msg);

    myPID.SetMode(AUTOMATIC);
    myPID.SetOutputLimits(umin, umax);
    myPID.SetSampleTime(SampleTime);
    myPID_v.SetMode(AUTOMATIC);
    myPID_v.SetOutputLimits(umin, umax);
    myPID_v.SetSampleTime(SampleTime);

    tiempo_inicial = millis();

    xx = 10;
}

///////////////////////////////////////////////
void setup_wifi()
{

    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(3000);
        Serial.print(".");
        WiFi.begin(ssid, password);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(carro.c_str()))
        {
            // LISTA SUBSCRIPCIONES
            client.subscribe(trenesp);
            client.subscribe(trenesi);
            client.subscribe(trenesd);
            client.subscribe(trenesp_v);
            client.subscribe(trenesi_v);
            client.subscribe(trenesd_v);
            client.subscribe(trenesestado);
            client.subscribe("trenes/ref");
            client.subscribe("trenes/etha");
            client.subscribe("trenes/u_lim");
            client.subscribe("trenes/carroL/v_lider");
            client.subscribe("trenes/carroL/vref");
            client.subscribe("trenes/sync");
            client.subscribe(trenessampletime);
            client.subscribe(trenestransmision);
            client.subscribe("setear/freq");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

int Busca_Dead(int Freq)
{
        mousecam_read_motion(&md);
        
        dead = 1;
        Frequency = Freq;
        ledcSetup(Control_v, Frequency, 10);
        //Serial.println("hola");
        while (dead)
        {
            mousecam_read_motion(&md);
            //Serial.println(md.squal);
            if (md.squal > 16)
            {
                MotorDirection = 1;
                MotorSpeed += 1;
                SetMotorControl();
                delay(40);
                //Serial.println("hola");
                if ((int8_t)md.dy > 0)
                {
                    dead = 0;
                    deadband = MotorSpeed;

                    SetMotorControl();
                    Serial.print("DeadBand Listo!! ");
                    Serial.print(Frequency);
                    Serial.print("[Hz], DeadBand en: ");
                    Serial.println(MotorSpeed);
                    MotorSpeed = 0;
                    mousecam_read_motion(&md);
                    delay(1000);
                    mousecam_read_motion(&md);
                }
            }
        }
        return deadband;
}

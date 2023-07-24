#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

/*Descomentar LONG_RANGE para usar. Incrementa la sensibilidad del
sensor y extiende su rango potencial, pero incrementa el error en 
la medicion leida. Funciona mejor en condiciones oscuras.
*/

//#define LONG_RANGE

/*Descomentar uno de los 2 para:
 - Mas velocidad pero menos precision
 - Mas precision pero menos velocidad
Respectivamente
*/

//#define HIGH_SPEED
//#define HIGH_ACCURACY

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }
  sensor.setSignalRateLimit(0.25);
  sensor.setMeasurementTimingBudget(33000);
  sensor.startContinuous(100); //toma medidas cada {Argumento} ms


/*#if defined LONG_RANGE
  // lower the return signal rate limit (default is 0.25 MCPS)
  sensor.setSignalRateLimit(1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  //sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  //sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
#endif

#if defined HIGH_SPEED
  // reduce timing budget to 20 ms (default is about 33 ms)
  sensor.setMeasurementTimingBudget(33000);
#else if defined HIGH_ACCURACY
  // increase timing budget to 200 ms
  sensor.setMeasurementTimingBudget(200000);
#endif*/
}
void loop()
{
  Serial.println(" ");
  Serial.print(sensor.readRangeContinuousMillimeters()*0.1);
  Serial.print(" CM.");
  Serial.println("");
  Serial.print(sensor.getMeasurementTimingBudget());
  Serial.println("");
  Serial.println(sensor.getSignalRateLimit());

  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  Serial.println();
}

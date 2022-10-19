#include <SoftwareSerial.h>


SoftwareSerial sim(2, 3); // RX, TX

void setup()
{
  Serial.begin(9600);
  sim.begin(9600);
  Serial.println("USB-Port bereit");
}

String s;

void loop()
{
  while (Serial.available())
  {
    sim.write(Serial.read());
  }
  //delay(100);
  while (sim.available())   
  {
    Serial.write(sim.read());
  }
}

// !!! muss in Verbindung mit Raspi_Encoding.py auf dem Raspi ausgeführt werden !!!

#include <SoftwareSerial.h>


SoftwareSerial sim(2, 3); // RX, TX

void setup()
{
  Serial.begin(9600);
  sim.begin(9600);
}

void loop()
{
  while(Serial.available())
  {
    String rcv = Serial.readStringUntil('#'); // receiving: empfangende Zeichen vom Raspi
    switch(rcv[0])
    {
      case('s'): send_sim (rcv); break;
      case('l'): send_lora(rcv); break;
      default  : error(rcv);break;
    }
  }
//  delay(10);
}




int send_sim(String str)
{
  str.remove(0,2);
  Serial.print("--SIM -Modul--   " + str + "#");
  if((str[0] == 'i') && (str[1] == ';'))
  {
    info_sim(str);
  }
  else
  {
    sim.write("AT");
//    updateSerial();
    sim.write("AT+CMGF=1");
//    updateSerial();
    sim.write("AT+CMGS=+4917682958784");  //um die nummer müssen noch ""
//    updateSerial();
    sim.print(str);
    sim.write(26);
//    updateSerial();
  }
  return 1;
}


int info_sim(String str)
{
  Serial.print("info");
  str.remove(0,2);
  sim.print(str);
//  updateSerial();
  delay(1000);
  str = "";
  while(sim.available())
    Serial.write(sim.read());
  Serial.write('#');
  return 1;
}


int send_lora(String str)
{
  str.remove(0,2);
  Serial.print("--LoRa-Modul--   " + str + "#");
  return 1;
}


int error(String str)
{
  Serial.print("--  Error   --   " + str + "#");
  return 1;
}

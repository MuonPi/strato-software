// !!! muss in Verbindung mit Raspi_Encoding.py auf dem Raspi ausgeführt werden !!!

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  while(Serial.available())
  {
    String rcv = Serial.readStringUntil('#'); // receiving: empfangende Zeichen vom Raspi
    switch(rcv[0])
    {
      case('s'): sim(rcv);  break;
      case('l'): lora(rcv); break;
      default  : error(rcv);break;
    }
  }
//  delay(10);
}




int sim(String str)
{
  str.remove(0,2);
  Serial.print("--SIM -Modul--   " + str + "#");
  return 1;
}


int lora(String str)
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

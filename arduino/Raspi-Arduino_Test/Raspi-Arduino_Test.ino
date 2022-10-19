// !!! muss in Verbindung mit Raspi-Arduino_Test.py auf dem Raspi ausgeführt werden !!!

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  if (Serial.available())
  {
    String str = Serial.readStringUntil('#');
    Serial.print("Empfangener String: ");
    Serial.print(str);
    Serial.print('#');
  }
  delay(500);
}

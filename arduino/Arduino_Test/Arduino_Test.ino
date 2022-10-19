void setup()
{
  Serial.begin(9600);
}

void loop()
{
  for(int i = 1; i < 9; i++)
  {
    Serial.print("lol");
    delay(100);
  }
  Serial.println();
}

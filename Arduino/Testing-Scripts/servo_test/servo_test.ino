#include"ServoTimer2.h"
ServoTimer2 servo;
int pulse = 0;
void setup()
{
  servo.attach(4);
  Serial.begin(115200);
}
void loop()
{ // put your main code here, to run repeatedly:
  servo1(0);  //min pulse width for 0 degree
  delay(1000);
  servo1(90);//pulse width for 90 degree
  delay(1000);
  servo1(180);
  Serial.println(servo.read());
  delay(1000);
}

void servo1(int angle) {
  pulse = map(angle, 0, 180, 750, 2250);
  servo.write(pulse);
}

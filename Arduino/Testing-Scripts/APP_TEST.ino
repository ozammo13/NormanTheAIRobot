#include <NeoSWSerial.h>
NeoSWSerial EEBlue(A4, A5); // RX | TX
String a;

int buzzer = 10;// add the pin number that you want to control.

void setup()
{
 
  Serial.begin(115200);
  EEBlue.begin(19200);  //Default Baud for comm, it may be different for your Module
  EEBlue.listen(); 
  Serial.println("The bluetooth gates are open.\n Connect to HC-05 from any other bluetooth device with 1234 as pairing key!.");
 
}

void loop() {

  while (EEBlue.available()) {

    a = EEBlue.read();

    Serial.println(a);

    if (a == "6")

    {

      tone(buzzer, 1020);
      delay(100);
      noTone(buzzer);
      tone(buzzer, 720);
      delay(100);
      noTone(buzzer);
      tone(buzzer, 920);
      delay(100);
      noTone(buzzer);

    }


  }

}

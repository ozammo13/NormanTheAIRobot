#include <SoftwareSerial.h>
SoftwareSerial EEBlue(A4, A5); // RX | TX
char t;
int LeftMotorR = 11;
int LeftMotorF = 5;
int RightMotorF = 6;
int RightMotorR = 9;
int buzzer = 10;
int servo = 3;
int MSpeed = 0;


void setup() {
  pinMode(LeftMotorF, OUTPUT);  //left motors f
  pinMode(LeftMotorR, OUTPUT);  //left motors r
  pinMode(RightMotorF, OUTPUT);  //right motors f
  pinMode(RightMotorR, OUTPUT);  //right motors r
  pinMode(servo, OUTPUT);
  Serial.begin(9600);
  
  EEBlue.begin(9600);

}

void loop() {
  if (EEBlue.available()) {
    t = EEBlue.read();
    Serial.println("start");
    Serial.println(t);
  }

  if (t == 'F') {          //move forward(all motors rotate in forward direction)
    analogWrite(LeftMotorR, 0);
    analogWrite(RightMotorR, 0);
    analogWrite(LeftMotorF, MSpeed);
    analogWrite(RightMotorF, MSpeed);
  }

  else if (t == 'B') {    //move reverse (all motors rotate in reverse direction)
    digitalWrite(3, HIGH);
    analogWrite(LeftMotorR, MSpeed);
    analogWrite(RightMotorR, MSpeed);
    analogWrite(LeftMotorF, 0);
    analogWrite(RightMotorF, 0);
  }

  else if (t == 'L') {    //turn right (left side motors rotate in forward direction, right side motors doesn't rotate)
    analogWrite(LeftMotorR, MSpeed);
    analogWrite(RightMotorF, MSpeed);
  }

  else if (t == 'R') {    //turn left (right side motors rotate in forward direction, left side motors doesn't rotate)
    analogWrite(LeftMotorF, MSpeed);
    analogWrite(RightMotorR, MSpeed);
  }



  else if (t == 'S') {    //STOP (all motors stop)
    analogWrite(LeftMotorF, 0);
    analogWrite(RightMotorF, 0);
    analogWrite(LeftMotorR, 0);
    analogWrite(RightMotorR, 0);
  }
  else if (t == 'V') {
    tone (buzzer, 1000);
    
  }
  else if (t =='v'){
    noTone(buzzer);
  }
  else if ((t =='1') or (t =='2')  or (t =='3')  or (t =='4')  or (t =='5')  or (t =='6')  or (t =='7')  or (t =='8') or (t =='9')) {
    MSpeed = (255/9) * int(t);
  }








}

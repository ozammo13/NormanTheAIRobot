//Author: Tom Brothers, Robotics Student
//servo setup----------------------------
#include"ServoTimer2.h"
ServoTimer2 servo;
int pulse = 0;
//---------------------------------------- -
//HuskyLens Setup------------------------
#include "HUSKYLENS.h"
#include "SoftwareSerial.h"
SoftwareSerial mySerial( 12, 13 );
HUSKYLENS huskylens;

//HUSKYLENS green line >> Pin 10; blue line >> Pin 11
void printResult(HUSKYLENSResult result);
void HuskyRead();
int tracker = 0;
//statemachine setup---------------------
void state_machine_run();
void Wait();
void Watch();
void Follow();
void Retrieve();
int state = 4; //jump to state 4 for peer review
//---------------------------------------
// Bluetooth Setup------------------------
SoftwareSerial EEBlue(A4, A5); // RX | TX
String a;
//Buzzer Setup---------------------------
int buzzer = 10;
//---------------------------------------
//Motor Setup----------------------------
int leftA = 9;
int leftB = 6;
int rightA = 11;
int rightB = 5;
int mspeed = 0;
//---------------------------------------

void setup() {
  pinMode(leftA, OUTPUT);  //left motors f
  pinMode(leftB, OUTPUT);  //left motors r
  pinMode(rightA, OUTPUT);  //right motors f
  pinMode(rightB, OUTPUT);  //right motors r
  pinMode(buzzer, OUTPUT);  servo.attach(3);
  servo.write(0);  //min pulse width for 0 degree
  delay(1);
  servo.write(90);//pulse width for 90 degree
  delay(10);
  Serial.begin(115200); //for debugging
  mySerial.begin(9600); // start a software serial
  huskylens.begin(mySerial);//start the husylens on myserial
  EEBlue.begin(9600);// bluetooth software serial
  EEBlue.available();// start bluetooth
  EEBlue.listen();//listen to the bluetooth port and not the huskylens port
  startupsound();
}

void loop() {
  state_machine_run(); //run the statemachine
  delay(10);
}
void state_machine_run() {
  switch (state)
  {
    case 1: Serial.println("Wait State");
      while (state == 1) {
        EEBlue.listen(); //listen
        a = EEBlue.read();  // when a button is clicked on the app, see what value is returned.
        Serial.println(a);

        if (a == "50") {
          mySerial.listen(); //listen to the camera and not the bluetooth
          state = 2; //go to watch state (facetracking)

        }

        if (a == "51") {
          mySerial.listen();
          servo.write(10);// put the servo straight down
          delay(10);
          state = 4;
        }
      }
    case 2:

      Serial.println("Entering Watch State");
      huskylens.writeAlgorithm(ALGORITHM_FACE_RECOGNITION); //switch the camera to facetracking mode
      while (state == 2) {
        EEBlue.listen(); // listen to bluetooth
        delay(300);
        a = EEBlue.read();
        Serial.println(a);
        if (a == "49")
        {
          state = 1;
        }
        mySerial.listen(); // switch back to camera
        huskylens.request(); // request data from camera
        HUSKYLENSResult result = huskylens.read();
        if ((result.width < 80 ) and (result.ID == 1)) { // if the width of the frame is 80 (small)
          state = 3;//switch to the follow state

        }


        if (result.width > 120 ) { //if the user gets too close
          motors_reverse();//back away
          scaredsound(); //and scream

        }



        else {

          Watch(); // watch the user
        }
      }
      break;
    case 3:
      Serial.println("Entering State 3");

      while (state == 3)
      {
        mySerial.listen(); // listen to camera
        followsound();
        huskylens.request();
        HUSKYLENSResult result = huskylens.read();

        if (result.width < 70) {
          Follow(); //go towards the user
        }
        else {
          state = 2; // go back to watch state
        }

      }

      break;

    case 4:
      while (state == 4)
      {
        huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION); //switch to colour recognition mode
        Catmode(); // catmode(laser-following)
      }


      break;
  }
}
void leftMotorPower(int power) { //left motor controller
  //error handling-------------------------------------
  if (power > 255) { //if power is larger than 255
    power = 255; //reduce it to 255
  }
  if (power < -255) { //if power is smaller than -255
    power = -255; // brings it up to -255
  }
  //---------------------------------------------------

  if (power == 0) {
    analogWrite(leftA, 0); // turn off the motor Forward
    analogWrite(leftB, 0); // turn off the motor Background
  }
  if ((power > 0) && (power <= 255)) { // forward
    analogWrite(leftA, power);
    analogWrite(leftB, 0);
  }
  if ((power < 0) && (power >= -255) ) { //reverse
    analogWrite(leftB, abs(power));
    analogWrite(leftA, 0);
  }
}
void rightMotorPower(int power) { //right motor controller
  //error handling-------------------------------------
  if (power > 255) { //if power is larger than 255
    power = 255; //reduce it to 255
  }
  if (power < -255) { //if power is smaller than -255
    power = -255; // brings it up to -255
  }
  //---------------------------------------------------


  if (power == 0) {
    analogWrite(rightA, 0); // turn off the motor Forward
    analogWrite(rightB, 0); // turn off the motor Reverse
  }
  if ((power > 0) && (power <= 255)) { //forward
    analogWrite(rightA, power);
    analogWrite(rightB, 0);
  }
  if ((power < 0) && (power >= -255) ) { //Reverse
    analogWrite(rightB, abs(power));
    analogWrite(rightA, 0);
  }
}
void Watch() { //watches user
  huskylens.request();
  HUSKYLENSResult result = huskylens.read();
  printResult(result);
  if ((result.xOrigin < 180) and (result.xOrigin > 130)) // if user is in the middle
  {
    motors_stop();
    huskylens.read();
  }
  else
  {
    if ((result.xOrigin < 110) and (tracker == 1)) // if the user is too the left
    {
      motors_left();
    }
    if ((result.xOrigin > 210) and (tracker == 1))// if the user is too the right
    {
      motors_right();
    }
  }

}
void Follow() { // follows the users face
  mySerial.listen(); // listen to the camera 
  huskylens.request();
  HUSKYLENSResult result = huskylens.read();
  printResult(result);
  if ((result.xOrigin < 210) and (result.xOrigin > 90)) // if the user is in the middle go forward
  {
    motors_forward();
    delay(100);
    huskylens.read();
  }
  else
  {
    if ((result.xOrigin < 110) and (tracker == 1))
    {
      motors_left();
    }
    if ((result.xOrigin > 210) and (tracker == 1))
    {
      motors_right();
    }
  }

}
void motors_stop() { //stop motor
  Serial.println("MOVING - STOP");
  leftMotorPower(0);
  rightMotorPower(0);

}
void motors_reverse() { // reverse motor
  Serial.println("MOVING - REVERSE");
  leftMotorPower(-80);
  rightMotorPower(-80);
}
void motors_forward() { // motor forward
  Serial.println("MOVING - FORWARD");
  leftMotorPower(80);
  rightMotorPower(80);

}
void motors_left() { //motor left
  motorspeed();
  Serial.println(mspeed);
  Serial.println("MOVING - LEFT");
  leftMotorPower(-mspeed);
  rightMotorPower(mspeed);
}
void motors_right() { //motor right
  motorspeed();
  Serial.println(mspeed);
  Serial.println("MOVING - RIGHT");
  leftMotorPower(mspeed);
  rightMotorPower(-mspeed);
}
void motorspeed() { //motor speed controller
  huskylens.request();
  HUSKYLENSResult result = huskylens.read();
  if (result.xOrigin < 110)
  {
    mspeed = map(result.xOrigin, 110, 0, 70, 100);//map speeds for turning left
    return;
  }
  if (result.xOrigin > 210)
  {
    mspeed = map(result.xOrigin, 190, 310, 70, 100);//maps speed for turning right
    return;
  }

}
void printResult(HUSKYLENSResult result) { //debugging
  if (result.command == COMMAND_RETURN_BLOCK) {
    Serial.println(String() + F("Block:xCenter=") + result.xCenter + F(",yCenter=") + result.yCenter + F(",width=") + result.width + F(",height=") + result.height + F(",ID=") + result.ID);
    tracker = 1;
  }
  else if (result.command == COMMAND_RETURN_ARROW) {
    Serial.println(String() + F("Arrow:xOrigin=") + result.xOrigin + F(",yOrigin=") + result.yOrigin + F(",xTarget=") + result.xTarget + F(",yTarget=") + result.yTarget + F(",ID=") + result.ID);
  }
  else {
    Serial.println("Object unknown!");
    tracker = 0;
  }
}
void startupsound() {
  tone (buzzer, 520);
  delay(200);
  noTone(buzzer);
  tone (buzzer, 720);
  delay(200);
  noTone(buzzer);
  tone (buzzer, 920);
  delay(200);
  noTone(buzzer);
  tone (buzzer, 1120);
  delay(200);
  noTone(buzzer);
}
void scaredsound() {
  tone (buzzer, 400);
  delay(50);
  noTone(buzzer);
}
void followsound() {
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
void Catmode() { // laserpointer following
  huskylens.request();
  servo1(180);
  HUSKYLENSResult result = huskylens.read();
  printResult(result);
  if ((result.yOrigin < 130) and (tracker == 1)) // if at the top of the screen
  {
    motors_forward();
    delay(500);
  }

  if ((result.yOrigin > 150) and (tracker == 1))// if at the bottom of the screen
  {
    motors_reverse();

  }

  if ((result.xOrigin < 210) and (result.xOrigin > 110) and (result.yOrigin < 140) and (result.yOrigin > 90)) //if the pointer is in the centre
  {
    motors_stop();
    delay(500);
  }
  else
  {
    if ((result.xOrigin < 110) and (tracker == 1) and (result.xOrigin < 210))// if pointer is to the left
    {
      motors_left();
      HUSKYLENSResult result = huskylens.read();
      printResult(result);
    }
    if ((result.xOrigin > 210) and (tracker == 1) and (result.xOrigin < 110))// if pointer is to the right
    {
      motors_right();
      HUSKYLENSResult result = huskylens.read();
      printResult(result);
    }
  }
}
void servo1(int angle) {
  pulse = map(angle, 0, 180, 750, 2250);
  servo.write(pulse);

}

#include <AFMotor.h>
AF_DCMotor motorL1(1);
AF_DCMotor motorL2(3);
AF_DCMotor motorR1(2);
AF_DCMotor motorR2(4);

byte minRoboSpeed = 50;
byte maxRoboSpeed = 150;
byte roboSpeed = 150;

bool upPressed = false, downPressed = false, rightPressed = false, leftPressed = false;

void setup() {
  Serial.begin(115200);
  motorL1.setSpeed(0);
  motorL2.setSpeed(0);
  motorR1.setSpeed(0);
  motorR2.setSpeed(0);
  Serial.println("sssssssssssssss");
}

void loop() {
  if (Serial.available() > 1) {
    byte command = Serial.read();
    byte p1;
    switch (command) {
      case 'C':
        // Process 'C' command (as before)
        Serial.println("Command 'C' received");
        break;
      case 'W':
      case 'w':
        p1 = Serial.read();
        if (p1 == 'P') upPressed = true;
        else if (p1 == 'R') upPressed = false;
        break;
      case 'S':
      case 's':
        p1 = Serial.read();
        if (p1 == 'P') downPressed = true;
        else if (p1 == 'R') downPressed = false;
        break;
      case 'A':
      case 'a':
        p1 = Serial.read();
        if (p1 == 'P') leftPressed = true;
        else if (p1 == 'R') leftPressed = false;
        break;
      case 'D':
      case 'd':
        p1 = Serial.read();
        if (p1 == 'P') rightPressed = true;
        else if (p1 == 'R') rightPressed = false;
        break;
      default:
        break;
    }
  }

  roboWalk();

}

// 20 mili-second timer
long lastRoboWalkMillis=millis();
void roboWalk() {

  if( (millis() -lastRoboWalkMillis ) > 10)
  {
      motorL1.setSpeed(roboSpeed);
      motorL2.setSpeed(roboSpeed);
      motorR1.setSpeed(roboSpeed);
      motorR2.setSpeed(roboSpeed);

    // 0 0 0 0 -1
    if( !downPressed && !leftPressed && !rightPressed && !upPressed)
    {
      //Serial.println("stop  X");
      motorL1.run(BRAKE);
      motorL2.run(BRAKE);
      motorR1.run(BRAKE);
      motorR2.run(BRAKE);
      motorL1.setSpeed(0);
      motorL2.setSpeed(0);
      motorR1.setSpeed(0);
      motorR2.setSpeed(0);

    }
    // 0 0 0 1 -2
    else if( !downPressed && !leftPressed && !rightPressed && upPressed)
    {
      //Serial.println(" ^  F");
      motorL1.run(FORWARD);
      motorL2.run(FORWARD);
      motorR1.run(FORWARD);
      motorR2.run(FORWARD);
    }
    // 0 0 1 0 -3
    else if( !downPressed && !leftPressed && rightPressed && !upPressed)
    {
      //Serial.println(" ->  F");
      motorL1.run(FORWARD);
      motorL2.run(FORWARD);
      motorR1.run(BACKWARD);
      motorR2.run(BACKWARD);
    }
    // 0 0 1 1 -4
    else if( !downPressed && !leftPressed && rightPressed && upPressed)
    {
      //Serial.println(" /  F");
      motorL1.run(FORWARD);
      motorL2.run(FORWARD);
      motorR1.run(BRAKE);
      motorR2.run(BRAKE);
      motorR1.setSpeed(0);
      motorR2.setSpeed(0);
    }
    // 0 1 0 0 -5
    else if( !downPressed && leftPressed && !rightPressed && !upPressed)
    {
      //Serial.println("<-  F");
      motorL1.run(BACKWARD);
      motorL2.run(BACKWARD);
      motorR1.run(FORWARD);
      motorR2.run(FORWARD);
    }
    // 0 1 0 1 -6
    else if( !downPressed && leftPressed && !rightPressed && upPressed)
    {
      //Serial.println(" \\  F");
      motorL1.run(BRAKE);
      motorL2.run(BRAKE);
      motorR1.run(FORWARD);
      motorR2.run(FORWARD);
      motorL1.setSpeed(0);
      motorL2.setSpeed(0);
    }
    // 0 1 1 0 -7
    else if( !downPressed && leftPressed && rightPressed && !upPressed)
    {
      //Serial.println("<-> F");
    }
    // 0 1 1 1 -8
    else if( !downPressed && leftPressed && rightPressed && upPressed)
    {
      //Serial.println("<^> F");
    }
    // 1 0 0 0 -9
    else if( downPressed && !leftPressed && !rightPressed && !upPressed)
    {
      //Serial.println(" v  B");
      motorL1.run(BACKWARD);
      motorL2.run(BACKWARD);
      motorR1.run(BACKWARD);
      motorR2.run(BACKWARD);
    }
    // 1 0 0 1 -10
    else if( downPressed && !leftPressed && !rightPressed && upPressed)
    {
      //Serial.println(" X  X");
      motorL1.run(BRAKE);
      motorL2.run(BRAKE);
      motorR1.run(BRAKE);
      motorR2.run(BRAKE);
      motorL1.setSpeed(0);
      motorL2.setSpeed(0);
      motorR1.setSpeed(0);
      motorR2.setSpeed(0);
    }
    // 1 0 1 0 -11
    else if( downPressed && !leftPressed && rightPressed && !upPressed)
    {
      //Serial.println(" \\  B");
      motorL1.run(BACKWARD);
      motorL2.run(BACKWARD);
      motorR1.run(BRAKE);
      motorR2.run(BRAKE);
      motorR1.setSpeed(0);
      motorR2.setSpeed(0);
    }
    // 1 0 1 1 -12
    else if( downPressed && !leftPressed && rightPressed && upPressed)
    {
      //Serial.println(" \\  X");
      motorL1.run(BRAKE);
      motorL2.run(BRAKE);
      motorR1.run(BRAKE);
      motorR2.run(BRAKE);
      motorL1.setSpeed(0);
      motorL2.setSpeed(0);
      motorR1.setSpeed(0);
      motorR2.setSpeed(0);
    }
    // 1 1 0 0 -13
    else if( downPressed && leftPressed && !rightPressed && !upPressed)
    {
      //Serial.println(" /  B");
      motorL1.run(BRAKE);
      motorL2.run(BRAKE);
      motorR1.run(BACKWARD);
      motorR2.run(BACKWARD);
      motorL1.setSpeed(0);
      motorL2.setSpeed(0);
    }
    // 1 1 0 1 -14
    else if( downPressed && leftPressed && !rightPressed && upPressed)
    {
      //Serial.println(" /  X");
      motorL1.run(BRAKE);
      motorL2.run(BRAKE);
      motorR1.run(BRAKE);
      motorR2.run(BRAKE);
      motorL1.setSpeed(0);
      motorL2.setSpeed(0);
      motorR1.setSpeed(0);
      motorR2.setSpeed(0);
    }
    // 1 1 1 0 -15
    else if( downPressed && leftPressed && rightPressed && !upPressed)
    {
      //Serial.println("<V> B");
    }
    // 1 1 1 1 -16
    else if( downPressed && leftPressed && rightPressed && upPressed)
    {
      //Serial.println("<o> X");
    }
    lastRoboWalkMillis=millis();
  }
}

String readParameter() {
  String parameter = "";
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == ' ' || c == '\n' || c == '\r') {
      break;
    }
    parameter += c;
  }
  return parameter;
}

int readIntParameter() {
  String parameter = readParameter();
  return parameter.toInt();
}

#include <Servo.h>
#include <AFMotor.h>
AF_DCMotor motorL1(1);
//AF_DCMotor motorL2(3);
AF_DCMotor motorR1(2);
//AF_DCMotor motorR2(4);
Servo grabber;
Servo arm;


const byte minRoboSpeed = 80;
const byte mediumRoboSpeed = 150;
const byte maxRoboSpeed = 255;
byte roboSpeed = 150;

int grabberPosition=90;
int armPosition=90;
int targetArmPosition=90;

bool upPressed = false, downPressed = false, rightPressed = false, leftPressed = false;
bool armUp = false, armDown = false;

void setup() {
  Serial.begin(115200);
  motorL1.setSpeed(0);
  motorR1.setSpeed(0);
  Serial.println("Hi Bhai..this is me ..your robot");
  grabber.attach(9);
  arm.attach(10);

  // try to smooth initial jerk
  grabber.write(grabberPosition-10);
  delay(200);
  grabber.write(grabberPosition);

  // 
  arm.write(armPosition-10);
  delay(200);
  arm.write(armPosition);
  
}

void loop() {
  if (Serial.available() > 1) {
    byte command = Serial.read();
    //Serial.println("command "+command);
    byte p1;
    switch (command) {
      case 'C':
        // Process 'C' command (as before)
        Serial.println("Command 'C' received");
        break;
      case 38:
      case 'W':
      case 'w':
        p1 = Serial.read();
        if (p1 == 'P') upPressed = true;
        else if (p1 == 'R') upPressed = false;
        break;
      case 40:
      case 'S':
      case 's':
        p1 = Serial.read();
        if (p1 == 'P') downPressed = true;
        else if (p1 == 'R') downPressed = false;
        break;
      case 37:
      case 'A':
      case 'a':
        p1 = Serial.read();
        if (p1 == 'P') leftPressed = true;
        else if (p1 == 'R') leftPressed = false;
        break;
      case 39:
      case 'D':
      case 'd':
        p1 = Serial.read();
        if (p1 == 'P') rightPressed = true;
        else if (p1 == 'R') rightPressed = false;
        break;
      case 17: //ctrl
        p1 = Serial.read();
        if (p1 == 'P') roboSpeed = minRoboSpeed;
        else if (p1 == 'R') roboSpeed = mediumRoboSpeed;
        break;
      case 16: //shift
        p1 = Serial.read();
        if (p1 == 'P') roboSpeed = maxRoboSpeed;
        else if (p1 == 'R') roboSpeed = mediumRoboSpeed;
        break;
      case 33: //pageUp
        p1 = Serial.read();
        if (p1 == 'P') armUp = true;
        else if (p1 == 'R') armUp = false;;
        break;
      case 34: //pageDown
        p1 = Serial.read();
        if (p1 == 'P') armDown = true;
        else if (p1 == 'R') armDown = false;
        break;
      case 36: //home
        p1 = Serial.read();
        if (p1 == 'P') targetArmPosition = 0;
        //else if (p1 == 'R') armDown = false;
        break;
      case 35: //end
        p1 = Serial.read();
        if (p1 == 'P') targetArmPosition = 160;
        //else if (p1 == 'R') armDown = false;
        break;
      default:
        break;
    }
  }

  roboWalk();

}



// 10 mili-second timer
long lastRoboWalkMillis=millis();
long lastStatusUpdateMillis=millis();
void roboWalk() {

  if( (millis() -lastRoboWalkMillis ) > 10)
  {
      motorL1.setSpeed(roboSpeed);
      motorR1.setSpeed(roboSpeed);

    // 0 0 0 0 -1
    if( !downPressed && !leftPressed && !rightPressed && !upPressed)
    {
      //Serial.println("stop  X");
      motorL1.run(BRAKE);
      motorR1.run(BRAKE);
      motorL1.setSpeed(0);
      motorR1.setSpeed(0);

    }
    // 0 0 0 1 -2
    else if( !downPressed && !leftPressed && !rightPressed && upPressed)
    {
      //Serial.println(" ^  F");
      motorL1.run(FORWARD);
      motorR1.run(FORWARD);
    }
    // 0 0 1 0 -3
    else if( !downPressed && !leftPressed && rightPressed && !upPressed)
    {
      //Serial.println(" ->  F");
      motorL1.run(FORWARD);
      motorR1.run(BACKWARD);
    }
    // 0 0 1 1 -4
    else if( !downPressed && !leftPressed && rightPressed && upPressed)
    {
      //Serial.println(" /  F");
      motorL1.run(FORWARD);
      motorR1.run(BRAKE);
      motorR1.setSpeed(0);
    }
    // 0 1 0 0 -5
    else if( !downPressed && leftPressed && !rightPressed && !upPressed)
    {
      //Serial.println("<-  F");
      motorL1.run(BACKWARD);
      motorR1.run(FORWARD);
    }
    // 0 1 0 1 -6
    else if( !downPressed && leftPressed && !rightPressed && upPressed)
    {
      //Serial.println(" \\  F");
      motorL1.run(BRAKE);
      motorR1.run(FORWARD);
      motorL1.setSpeed(0);
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
      motorR1.run(BACKWARD);
    }
    // 1 0 0 1 -10
    else if( downPressed && !leftPressed && !rightPressed && upPressed)
    {
      //Serial.println(" X  X");
      motorL1.run(BRAKE);
      motorR1.run(BRAKE);
      motorL1.setSpeed(0);
      motorR1.setSpeed(0);
    }
    // 1 0 1 0 -11
    else if( downPressed && !leftPressed && rightPressed && !upPressed)
    {
      //Serial.println(" \\  B");
      motorL1.run(BACKWARD);
      motorR1.run(BRAKE);
      motorR1.setSpeed(0);
    }
    // 1 0 1 1 -12
    else if( downPressed && !leftPressed && rightPressed && upPressed)
    {
      //Serial.println(" \\  X");
      motorL1.run(BRAKE);
      motorR1.run(BRAKE);
      motorL1.setSpeed(0);
      motorR1.setSpeed(0);
    }
    // 1 1 0 0 -13
    else if( downPressed && leftPressed && !rightPressed && !upPressed)
    {
      //Serial.println(" /  B");
      motorL1.run(BRAKE);
      motorR1.run(BACKWARD);
      motorL1.setSpeed(0);
    }
    // 1 1 0 1 -14
    else if( downPressed && leftPressed && !rightPressed && upPressed)
    {
      //Serial.println(" /  X");
      motorL1.run(BRAKE);
      motorR1.run(BRAKE);
      motorL1.setSpeed(0);
      motorR1.setSpeed(0);
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

    //######################################
    // ARM
    //######################################

    if(armUp && !armDown)
    {
      targetArmPosition--;
    }
    else if(!armUp && armDown)
    {
      targetArmPosition++;
    }

    grabber.write(grabberPosition);
    if(targetArmPosition !=armPosition)
    {
      if(targetArmPosition>=180)
      {
        targetArmPosition=180;
      }
      else if(targetArmPosition<=0)
      {
        targetArmPosition=0;
      }

      if((targetArmPosition-armPosition)>0)
      {
        armPosition++;
      }else if((targetArmPosition-armPosition)<0)
      {
        armPosition--;
      }
      arm.write(armPosition);

    }





    lastRoboWalkMillis=millis();
  }

  if( (millis() -lastStatusUpdateMillis ) > 200)
  {
    Serial.println("A"+String(armPosition));
    lastStatusUpdateMillis=millis();
  }
}

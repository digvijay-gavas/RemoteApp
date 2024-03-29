#include <Servo.h>
#include <AFMotor.h>
AF_DCMotor motorL1(2);
//AF_DCMotor motorL2(3);
AF_DCMotor motorR1(4);
//AF_DCMotor motorR2(4);
Servo grabber;
Servo arm;


const byte minRoboSpeed = 80;
const byte mediumRoboSpeed = 150;
const byte maxRoboSpeed = 200;
byte roboSpeed = 150;

int grabberPosition=90;
int targetGrabberPosition=90;
int maxGrabberPosition=134;
int minGrabberPosttion=0;
int maxCycleGrabberPosition=134;
int minCycleGrabberPosttion=47;

int armPosition=90;
int targetArmPosition=90;
int maxArmPostion=152;
int minArmPostion=37;
int maxCycleArmPostion=124;
int minCycleArmPostion=66;

bool upPressed = false, downPressed = false, rightPressed = false, leftPressed = false;
bool armUp = false, armDown = false;
bool grabberOpen = false, grabberClose = false;

// line follow mode
bool lineFollowMode=false;
bool leftProximity = true,rightProxymity=true;
bool previousLeftProximity = true,previousRightProxymity=true;
int leftProximityPin = A1, rightProxymityPin=A0;
#define _LEFT 0
#define _RIGHT 1

int movedDirection=_LEFT;
int stuckCount;

void setup() {
  Serial.begin(115200);
  Serial.println("Hi Bhai..this is me ..your robot");

  pinMode(A3, INPUT);
  lineFollowMode = digitalRead(A3) == HIGH;

  motorL1.setSpeed(0);
  motorR1.setSpeed(0);
  grabber.attach(10);
  arm.attach(9);

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
      case 'L':
      case 'l':
        p1 = Serial.read();
        if (p1 == 'P') {lineFollowMode = true;roboSpeed=minRoboSpeed;}
        else if (p1 == 'R') {lineFollowMode = false;upPressed = false; downPressed = false; rightPressed = false; leftPressed = false;}
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
        if (p1 == 'P') targetArmPosition = minCycleArmPostion;
        //else if (p1 == 'R') armDown = false;
        break;
      case 35: //end
        p1 = Serial.read();
        if (p1 == 'P') targetArmPosition = maxCycleArmPostion;
        //else if (p1 == 'R') armDown = false;
      case 155: //insert
        p1 = Serial.read();
        if (p1 == 'P') targetGrabberPosition = maxCycleGrabberPosition;
        //else if (p1 == 'R') armDown = false;
        break;
      case 127: //delete
        p1 = Serial.read();
        if (p1 == 'P') targetGrabberPosition = minCycleGrabberPosttion;
        //else if (p1 == 'R') armDown = false;
      case 49: //1
        p1 = Serial.read();
        if (p1 == 'P') grabberOpen = true;
        else if (p1 == 'R') grabberOpen = false;
        break;
      case 50: //2
        p1 = Serial.read();
        if (p1 == 'P') grabberClose = true;
        else if (p1 == 'R') grabberClose = false;
        break;
      case 48: //0
        p1 = Serial.read();
        if (p1 == 'P') 
        {
          if(targetGrabberPosition>90 && targetGrabberPosition!=180)
          {
            targetGrabberPosition=180;
          }
          else if (targetGrabberPosition!=0)
          {
            targetGrabberPosition=0;
          }
          else
          {
            targetGrabberPosition=180;
          }
        }
        //else if (p1 == 'R') armDown = false;
        break;
      default:
        break;
    }
  }

  roboWalk();

}



// 10 mili-second timer
int count=0;
long lastRoboWalkMillis=millis();
long lastStatusUpdateMillis=millis();
long lineFollowerMillis=millis();
void roboWalk() {


  if( lineFollowMode && (millis() -lineFollowerMillis ) > 50)
  {

    leftProximity = digitalRead(leftProximityPin) == HIGH;
    rightProxymity = digitalRead(rightProxymityPin) == HIGH;


    if(previousLeftProximity == leftProximity && previousRightProxymity == rightProxymity)
    {
      stuckCount++;
    }
    else
    {
      stuckCount=0;
    }
    // 0 0
    if(leftProximity == rightProxymity)
    {
      if(count==4)
      {
        
        motorL1.setSpeed(roboSpeed-50);
        motorR1.setSpeed(roboSpeed-50);
        motorL1.run(FORWARD);
        motorR1.run(FORWARD);
        count=0;
      }
      else
      {
        motorL1.setSpeed(0);
        motorR1.setSpeed(0);
        motorL1.run(BRAKE);
        motorR1.run(BRAKE);
      }
      count++;
      
    }
    // 0 1
    else if(!leftProximity && rightProxymity)
    {
      //count=0;
      if(stuckCount<5)
      {
        motorL1.setSpeed(roboSpeed);
        motorR1.setSpeed(roboSpeed/2);
        motorL1.run(FORWARD);
        motorR1.run(BACKWARD);
        movedDirection=_RIGHT;
      }
      else
      {
        int lineFollowRoboSpeed=roboSpeed+stuckCount;
        if(lineFollowRoboSpeed>=255) lineFollowRoboSpeed=255;
        motorL1.setSpeed(lineFollowRoboSpeed);
        motorR1.setSpeed(lineFollowRoboSpeed);
        motorL1.run(FORWARD);
        motorR1.run(BACKWARD);
        movedDirection=_RIGHT;
      }
    }
    // 1 0
    else if(leftProximity && !rightProxymity)
    {
      //count=0;
      if(stuckCount<5)
      {
        motorL1.setSpeed(roboSpeed/2);
        motorR1.setSpeed(roboSpeed);
        motorL1.run(BACKWARD);
        motorR1.run(FORWARD);
        movedDirection=_LEFT;
      }
      else
      {
        int lineFollowRoboSpeed=roboSpeed+stuckCount;
        if(lineFollowRoboSpeed>=255) lineFollowRoboSpeed=255;
        motorL1.setSpeed(lineFollowRoboSpeed);
        motorR1.setSpeed(lineFollowRoboSpeed);
        motorL1.run(BACKWARD);
        motorR1.run(FORWARD);
        movedDirection=_LEFT;
      }

    }
    

    previousLeftProximity=leftProximity;
    previousRightProxymity=rightProxymity;

    lineFollowerMillis=millis();
  }

  if( !lineFollowMode && (millis() -lastRoboWalkMillis ) > 10)
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

    
    if(targetArmPosition !=armPosition)
    {
      if(targetArmPosition>=maxArmPostion)
      {
        targetArmPosition=maxArmPostion;
      }
      else if(targetArmPosition<=minArmPostion)
      {
        targetArmPosition=minArmPostion;
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

    if(grabberOpen && !grabberClose)
    {
      targetGrabberPosition--;
    }
    else if(!grabberOpen && grabberClose)
    {
      targetGrabberPosition++;
    }

      
    if(targetGrabberPosition !=grabberPosition)
    {
      if(targetGrabberPosition>=maxGrabberPosition)
      {
        targetGrabberPosition=maxGrabberPosition;
      }
      else if(targetGrabberPosition<=minGrabberPosttion)
      {
        targetGrabberPosition=minGrabberPosttion;
      }

      if((targetGrabberPosition-grabberPosition)>0)
      {
        grabberPosition++;
      }else if((targetGrabberPosition-grabberPosition)<0)
      {
        grabberPosition--;
      }
      grabber.write(grabberPosition);
    }





    lastRoboWalkMillis=millis();
  }

  if( (millis() -lastStatusUpdateMillis ) > 200)
  {
    if(lineFollowMode)
      Serial.println( String(leftProximity?"o":"-") + String(rightProxymity?"o":"-")+"  "+String(count));
    else
      Serial.println("A"+String(armPosition)+" G"+String(grabberPosition));

    lastStatusUpdateMillis=millis();
  }
}

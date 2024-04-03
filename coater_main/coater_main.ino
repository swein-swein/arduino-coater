#include "AccelStepper.h"
#include "ezButton.h"
#include "LiquidCrystal_I2C.h"

#define XdirPin 1
#define XstepPin 2
#define ZdirPin 3
#define ZstepPin 4
#define pumpDirPin 5
#define pumpStepPin 6
#define motorInterfaceType 1

LiquidCrystal_I2C lcd(0x27,  16, 2);

int menu = 1;
int xEndPos = 0;
int xSpeed = 0;
int pumpSpeed = 0;
int pumpEnd = 100;

ezButton upButton(7);
ezButton downButton(8);
ezButton leftButton(9);
ezButton rightButton(10);
ezButton selectButton(11);
ezButton limitXLow(12);
ezButton limitXHigh(13);

AccelStepper stepperX = AccelStepper(motorInterfaceType, XstepPin, XdirPin);
AccelStepper stepperZ = AccelStepper(motorInterfaceType, ZstepPin, ZdirPin);
AccelStepper stepperPump = AccelStepper(motorInterfaceType, pumpStepPin, pumpDirPin);

///////////////////////////////////////////////////

void setup() {
  lcd.begin(16,2);
  lcd.backlight();
  
//  Serial.begin(9600);

  stepperX.setMaxSpeed(1000);
  stepperZ.setMaxSpeed(1000);
  stepperPump.setMaxSpeed(1000);

  upButton.setDebounceTime(50);
  downButton.setDebounceTime(50);
  leftButton.setDebounceTime(50);
  rightButton.setDebounceTime(50);
  selectButton.setDebounceTime(50);
  limitXLow.setDebounceTime(5);
  limitXHigh.setDebounceTime(5);
       
  updateMenu();
}

///////////////////////////////////////////////////

void loop() {

  int down = downButton.getStateRaw();
  int up = upButton.getStateRaw();
  int select = selectButton.getStateRaw();

  if (down == 0){
    menu++;
    updateMenu();
    delay(300);
  }
  if (up == 0){
    menu--;
    updateMenu();
    delay(300);
  }
  if (select == 0){
    executeAction();
    updateMenu();
    delay(300);
  }
}

///////////////////////////////////////////////////

void updateMenu() {
  switch (menu) {
    case 0:
      menu = 1;
      break;
    case 1:
      lcd.clear();
      lcd.print(">Calibrate X axis");
      lcd.setCursor(0, 1);
      lcd.print(" Set X axis end pos.");
      break;
    case 2:
      lcd.clear();
      lcd.print(" Calibrate X axis");
      lcd.setCursor(0, 1);
      lcd.print(">Set X axis end pos.");
      break;
    case 3:
      lcd.clear();
      lcd.print(">Set Z height");
      lcd.setCursor(0, 1);
      lcd.print(" Set X axis speed");
      break;
    case 4:
      lcd.clear();
      lcd.print(" Set Z height");
      lcd.setCursor(0, 1);
      lcd.print(">Set X axis speed");
      break;
    case 5:
      lcd.clear();
      lcd.print(">Set pump speed");
      lcd.setCursor(0, 1);
      lcd.print(" Set pump end");
      break;
    case 6:
      lcd.clear();
      lcd.print(" Set pump speed");
      lcd.setCursor(0, 1);
      lcd.print(">Set pump end");
      break;
    case 7:
      lcd.clear();
      lcd.print(">Prime pump");
      lcd.setCursor(0, 1);
      lcd.print(" Start coating");
      break;
    case 8:
      lcd.clear();
      lcd.print("Prime pump");
      lcd.setCursor(0, 1);
      lcd.print(">Start coating");
      break;
    case 9:
      menu = 8;
      break;
  }
}

///////////////////////////////////////////////////

void executeAction() {
  switch (menu) {
    case 1:
      calibrateX();
      break;
    case 2:
      setXStop();
      break;
    case 3:
      setZHeight();
      break;
    case 4:
      setXSpeed();
      break;
    case 5:
      setPumpSpeed();
      break;
    case 6:
      setPumpEnd();
      break;
    case 7:
      primePump();
      break;
    case 8:
      startCoat();
      break;
  }
}

///////////////////////////////////////////////////

void calibrateX() {
  lcd.clear();
  lcd.print("Zeroing X axis");

  bool isStopped = false;

  while(!isStopped)
  {
    limitXLow.loop();
    // Set the speed in steps per second:
    stepperX.setSpeed(-300);
    // Step the motor with a constant speed as set by setSpeed():
    stepperX.runSpeed();

    if (limitXLow.isPressed())
    {
      isStopped = !isStopped;
    }
  }
  
  // Set current position to new 0
  stepperX.setCurrentPosition(0);
  
  lcd.clear();
  lcd.print("X axis calibrated");
  delay(1500);
}

///////////////////////////////////////////////////

void setXStop() {
  lcd.clear();
  lcd.print("Set X stop position");
  delay(1000);

  bool isSet = false;
  int targetPos = 0;

  while(!isSet) {
    int right = rightButton.getStateRaw();
    int left = leftButton.getStateRaw();
    int up = upButton.getStateRaw();
    int down = downButton.getStateRaw();
    int select = selectButton.getStateRaw();

    if (right == 0)
    {
      targetPos = stepperX.currentPosition() + 10;
      while(stepperX.currentPosition() != targetPos)
      {
        stepperX.setSpeed(200);
        stepperX.runSpeed();
      }
    }

    if (left == 0)
    {
      targetPos = stepperX.currentPosition() - 10;
      while(stepperX.currentPosition() != targetPos)
      {
        stepperX.setSpeed(-200);
        stepperX.runSpeed();
      }
    }

    if (up == 0)
    {
      targetPos = stepperX.currentPosition() + 10;
      while(stepperX.currentPosition() != targetPos)
      {
        stepperX.setSpeed(600);
        stepperX.runSpeed();
      }
    }

    if (down == 0)
    {
      targetPos = stepperX.currentPosition() - 10;
      while(stepperX.currentPosition() != targetPos)
      {
        stepperX.setSpeed(-600);
        stepperX.runSpeed();
      }
    }

    if (select == 0)
    {
      isSet = true;
    }
  }

  xEndPos = stepperX.currentPosition();

  lcd.clear();
  lcd.print("X end position set to");
  lcd.print(xEndPos);

  delay(1500);
}

///////////////////////////////////////////////////

void setZHeight() {
  lcd.clear();
  lcd.print("Set Z height");
  delay(1000);
  bool isSet = false;
  int targetPos = 0;

  while(!isSet) {
    int right = rightButton.getStateRaw();
    int left = leftButton.getStateRaw();
    int up = upButton.getStateRaw();
    int down = downButton.getStateRaw();
    int select = selectButton.getStateRaw();

    if (right == 0)
    {
      targetPos = stepperZ.currentPosition() + 10;
      while(stepperZ.currentPosition() != targetPos)
      {
        stepperZ.setSpeed(200);
        stepperZ.runSpeed();
      }
    }

    if (left == 0)
    {
      targetPos = stepperZ.currentPosition() - 10;
      while(stepperZ.currentPosition() != targetPos)
      {
        stepperZ.setSpeed(-200);
        stepperZ.runSpeed();
      }
    }

    if (up == 0)
    {
      targetPos = stepperZ.currentPosition() + 10;
      while(stepperZ.currentPosition() != targetPos)
      {
        stepperZ.setSpeed(600);
        stepperZ.runSpeed();
      }
    }

    if (down == 0)
    {
      targetPos = stepperZ.currentPosition() - 10;
      while(stepperZ.currentPosition() != targetPos)
      {
        stepperZ.setSpeed(-600);
        stepperZ.runSpeed();
      }
    }

    if (select == 0)
    {
      isSet = true;
    }
  }

  stepperZ.setCurrentPosition(0);

  lcd.clear();
  lcd.print("Z height set");

  delay(1500);
}

///////////////////////////////////////////////////

void setXSpeed() {

  lcd.clear();
  lcd.print("Set X speed");
  delay(1000);

  bool isSet = false;
  int xSpeedRef = 99;

  while(!isSet)
  {
    int right = rightButton.getStateRaw();
    int left = leftButton.getStateRaw();
    int up = upButton.getStateRaw();
    int down = downButton.getStateRaw();
    int select = selectButton.getStateRaw();

    if (xSpeedRef != xSpeed) // update lcd only if value has changed
    {
      lcd.clear();
      lcd.print("Set X speed");
      lcd.setCursor(0, 1);
      lcd.print(xSpeed);
      xSpeedRef = xSpeed;
    }

    if (right == 0)
    {
      xSpeed++;
      delay(100);
    }

    if (left == 0)
    {
      xSpeed--;
      delay(100);
    }

    if (up == 0)
    {
      xSpeed+=10;
      delay(100);
    }

    if (down == 0)
    {
      xSpeed-=10;
      delay(100);
    }

    if (select == 0)
    {
      isSet = true;
    }
  }

  lcd.clear();
  lcd.print("X speed set to");
  lcd.setCursor(0, 1);
  lcd.print(xSpeed);

  delay(1500);
}

///////////////////////////////////////////////////

void setPumpSpeed() {

  bool isSet = false;
  int pumpSpeedRef = 99;
  delay(1000);

  while(!isSet)
  {
    int right = rightButton.getStateRaw();
    int left = leftButton.getStateRaw();
    int up = upButton.getStateRaw();
    int down = downButton.getStateRaw();
    int select = selectButton.getStateRaw();

    if (pumpSpeedRef != pumpSpeed) // update lcd only if value has changed
    {
      lcd.clear();
      lcd.print("Set pump speed");
      lcd.setCursor(0, 1);
      lcd.print(pumpSpeed);
      pumpSpeedRef = pumpSpeed;
    }

    if (right == 0)
    {
      pumpSpeed++;
      delay(100);
    }

    if (left == 0)
    {
      pumpSpeed--;
      delay(100);
    }

    if (up == 0)
    {
      pumpSpeed+=10;
      delay(100);
    }

    if (down == 0)
    {
      pumpSpeed-=10;
      delay(100);
    }

    if (select == 0)
    {
      isSet = true;
    }
  }

  lcd.clear();
  lcd.print("Pump speed set to");
  lcd.setCursor(0, 1);
  lcd.print(pumpSpeed);

  delay(1500);
}

///////////////////////////////////////////////////

void setPumpEnd() {

  bool isSet = false;
  int pumpEndRef = 99;
  delay(1000);

  while(!isSet)
  {
    int right = rightButton.getStateRaw();
    int left = leftButton.getStateRaw();
    int up = upButton.getStateRaw();
    int down = downButton.getStateRaw();
    int select = selectButton.getStateRaw();

    if (pumpEndRef != pumpEnd) // update lcd only if value has changed
    {
      lcd.clear();
      lcd.print("Set pump end percentage");
      lcd.setCursor(0, 1);
      lcd.print(pumpEnd);
      lcd.print("%")
      pumpEndRef = pumpEnd;
    }

    if (right == 0)
    {
      pumpEnd++;
      delay(100);
    }

    if (left == 0)
    {
      pumpEnd--;
      delay(100);
    }

    if (up == 0)
    {
      pumpEnd+=10;
      delay(100);
    }

    if (down == 0)
    {
      pumpEnd-=10;
      delay(100);
    }

    if (select == 0)
    {
      isSet = true;
    }
  }

  lcd.clear();
  lcd.print("Pump will at stop");
  lcd.setCursor(0, 1);
  lcd.print(pumpEnd);
  lcd.print("%")

  delay(1500);
}

///////////////////////////////////////////////////

void primePump() {
  lcd.clear();
  lcd.print("Prime pump");
  delay(1000);

  bool isSet = false;
  int targetPos = 0;

  while(!isSet) {
    int right = rightButton.getStateRaw();
    int left = leftButton.getStateRaw();
    int up = upButton.getStateRaw();
    int down = downButton.getStateRaw();
    int select = selectButton.getStateRaw();

    if (right == 0)
    {
      targetPos += 10;
      while(stepperPump.currentPosition() != targetPos)
      {
        stepperPump.setSpeed(200);
        stepperPump.runSpeed();
      }
    }

    if (left == 0)
    {
      targetPos = stepperPump.currentPosition() - 10;
      while(stepperPump.currentPosition() != targetPos)
      {
        stepperPump.setSpeed(-200);
        stepperPump.runSpeed();
      }
    }

    if (up == 0)
    {
      targetPos = stepperPump.currentPosition() + 10;
      while(stepperPump.currentPosition() != targetPos)
      {
        stepperPump.setSpeed(600);
        stepperPump.runSpeed();
      }
    }

    if (down == 0)
    {
      targetPos = stepperPump.currentPosition() - 10;
      while(stepperPump.currentPosition() != targetPos)
      {
        stepperPump.setSpeed(-600);
        stepperPump.runSpeed();
      }
    }

    if (select == 0)
    {
      isSet = true;
    }
  }

  lcd.clear();
  lcd.print("Pump primed");

  delay(1500);
}

///////////////////////////////////////////////////

void startCoat() {

  bool isFinished = false;
  double progressRef = 101;
  double progress = 0;
  double endPos = xEndPos;
  //stepperX.moveTo(0);
//  stepperZ.moveTo(0);
  stepperX.setAcceleration(200);
  stepperX.runToNewPosition(0);
//  stepperZ.runToPosition();

  while((stepperX.currentPosition() < endPos) && (!limitXHigh.isPressed()))
  {
    limitXHigh.loop();
    progress = ((stepperX.currentPosition() / endPos) * 100);

    if ((int) progressRef != (int) progress)
    {
      lcd.clear();
      lcd.print("Coating progress");
      lcd.setCursor(0, 1);
      lcd.print((int) progress);
      progressRef = progress;
    }

    stepperX.setSpeed(xSpeed);
    stepperX.runSpeed();

    while (pumpEnd <= progress)
    {    
      stepperPump.setSpeed(pumpSpeed);
      stepperPump.runSpeed();
    }
  }

  lcd.clear();
  lcd.print("Coating done");

  delay(1500);
}

///////////////////////////////////////////////////
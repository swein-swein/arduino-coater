// Include the AccelStepper library:
#include "AccelStepper.h"
#include "ezButton.h"
//#include "LiquidCrystal.h"
#include "LiquidCrystal_I2C.h"

#define XdirPin 1
#define XstepPin 2
#define ZdirPin 3
#define ZstepPin 4
#define pumpDirPin 5
#define pumpStepPin 6
#define motorInterfaceType 1

//const int rs = 8, en = 9, d4 = 10, d5 = 11, d6 = 12, d7 = 13;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
LiquidCrystal_I2C lcd(0x27,  16, 2);

int menu = 1;
int xEndPos = 0;
int xSpeed = 0;
int pumpSpeed = 0;

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
  //lcd.init();
  lcd.backlight();
  
  Serial.begin(9600);

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
      lcd.print(" Prime pump");
      break;
    case 6:
      lcd.clear();
      lcd.print(" Set pump speed");
      lcd.setCursor(0, 1);
      lcd.print(">Prime pump");
      break;
    case 7:
      lcd.clear();
      lcd.print(">Start coating");
      break;
    case 8:
      menu = 7;
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
      primePump();
      break;
    case 7:
      startCoat();
      break;
  }
}

///////////////////////////////////////////////////

void calibrateX() {
  lcd.clear();
  lcd.print("Zeroing X axis");

  bool isStopped = false;
  Serial.println("start x calibration");
  while(!isStopped)
  {
    limitXLow.loop();
    // Set the speed in steps per second:
    stepperX.setSpeed(100);
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
  Serial.println("end x calibrated");
  delay(1500);
}

///////////////////////////////////////////////////

void setXStop() {
  lcd.clear();
  lcd.print("Set X stop position");

  bool isSet = false;

  while(!isSet)
  {
    rightButton.loop();
    leftButton.loop();
    selectButton.loop();
    
    if (rightButton.isPressed())
    {
      stepperX.moveTo(stepperX.currentPosition()+100);
      stepperX.runToPosition();
    }

    if (leftButton.isPressed())
    {
      stepperX.moveTo(stepperX.currentPosition()-100);
      stepperX.runToPosition();
    }
    if (selectButton.isPressed())
    {
      isSet = true;
    }
  }

  // Set current position to new 0
  xEndPos = stepperX.currentPosition();//(stepperX.currentPosition());

  lcd.clear();
  lcd.print("X end position sest");

  delay(1500);
}

///////////////////////////////////////////////////

void setZHeight() {
  lcd.clear();
  lcd.print("Set Z height");

  bool isSet = false;

  while(!isSet)
  {
    rightButton.loop();
    leftButton.loop();
    selectButton.loop();
    
    if (rightButton.isPressed())
    {
      stepperZ.moveTo(stepperZ.currentPosition()+100);
      stepperZ.runToPosition();
    }

    if (leftButton.isPressed())
    {
      stepperZ.moveTo(stepperZ.currentPosition()-100);
      stepperZ.runToPosition();
    }
    if (selectButton.isPressed())
    {
      isSet = true;
    }
  }

  stepperZ.setCurrentPosition(0);//=(stepperZ.currentPosition());

  lcd.clear();
  lcd.print("Z height set");

  delay(1500);
}

///////////////////////////////////////////////////

void setXSpeed() {

  bool isSet = false;

  while(!isSet)
  {
    rightButton.loop();
    leftButton.loop();
    selectButton.loop();

    lcd.clear();
    lcd.print("Set X speed");
    lcd.setCursor(0, 1);
    lcd.print(xSpeed);

    if (rightButton.isPressed())
    {
      xSpeed++;
    }

    if (leftButton.isPressed())
    {
      xSpeed--;
    }
    if (selectButton.isPressed())
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

  while(!isSet)
  {
    rightButton.loop();
    leftButton.loop();
    selectButton.loop();

    lcd.clear();
    lcd.print("Set pump speed");
    lcd.setCursor(0, 1);
    lcd.print(pumpSpeed);

    if (rightButton.isPressed())
    {
      pumpSpeed++;
    }

    if (leftButton.isPressed())
    {
      pumpSpeed--;
    }
    if (selectButton.isPressed())
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

void primePump() {
  lcd.clear();
  lcd.print("Prime pump");

  bool isSet = false;

  while(!isSet)
  {
    rightButton.loop();
    leftButton.loop();
    selectButton.loop();

    if (rightButton.isPressed())
    {
      stepperPump.moveTo(stepperPump.currentPosition()+100);
      stepperPump.runToPosition();
    }

    if (leftButton.isPressed())
    {
      stepperPump.moveTo(stepperPump.currentPosition()-100);
      stepperPump.runToPosition();
    }
    if (selectButton.isPressed())
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

  stepperX.moveTo(0);
  stepperZ.moveTo(0);
  stepperX.runToPosition();
  stepperZ.runToPosition();

  while((stepperX.currentPosition() < xEndPos) && (!limitXHigh.isPressed()))
  {
    limitXHigh.loop();
    lcd.clear();
    lcd.print("Coating progress");
    lcd.setCursor(0, 1);
    lcd.print( (stepperX.currentPosition()) / xEndPos);

    stepperX.setSpeed(xSpeed);
    stepperPump.setSpeed(pumpSpeed);
    stepperX.runSpeed();
    stepperPump.runSpeed();
  }

  lcd.clear();
  lcd.print("Coating done");
  lcd.setCursor(0, 1);
  lcd.print(pumpSpeed);

  delay(1500);
}

///////////////////////////////////////////////////
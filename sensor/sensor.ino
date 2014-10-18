/* 
  Requires RGB LCD Library
  
  A0: First Sensor
  A1: Second Sensor
  
  Pin 4: Touch button to reset counter
  
*/

#include <rgb_lcd.h>
#include <Wire.h>

rgb_lcd lcd;

const int colorR = 0;
const int colorG = 0;
const int colorB = 255;

const int PIN_LED = 13;
const int PIN_TOUCH = 4;

const int INPUT_LIGHT_FIRST = 0;
const int INPUT_LIGHT_SECOND = 1;

const int NUM_PREVIOUS_SENSOR_VALUES = 5;
const int THRESHOLD = 35;
const int SENSOR_INTERVAL = 100;

boolean flagOne;
boolean flagTwo;

int numPeople;

int curOne;
int curTwo;

int prevOne;
int prevTwo;

void setup() {                
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TOUCH, INPUT);

  Serial.begin(9600);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);
  lcd.print("Number of People: ");
  
  delay(1000);
}


void loop() {
  delay(SENSOR_INTERVAL);
  
  if (isTouchPressed()) {
    numPeople = 0;
  }
  
  // Light
  int lightSensorValue = analogRead(INPUT_LIGHT_FIRST); 
  curOne = (int) (1023 - lightSensorValue) * 10 / lightSensorValue;
  
  lightSensorValue = analogRead(INPUT_LIGHT_SECOND); 
  curTwo = (int) (1023 - lightSensorValue) * 10 / lightSensorValue;  
    
  Serial.println(curOne);
  Serial.println(curTwo);
  
  // If the difference in the last 2 values is > threshold --> we have a person
  if(isSensorActivated(prevOne, curOne)) {
    Serial.println("Sensor One Activated");
     if(flagTwo) {
       flagTwo = flagOne = false;
       numPeople--;
     } else {
       flagOne = true;
     }
  }
  
  if(isSensorActivated(prevTwo, curTwo)) {
    Serial.println("Sensor Two Activated");
     if(flagOne) {
       flagTwo = flagOne = false;
       numPeople++;
     } else {
       flagTwo = true;
     }
  }
  
  
  lcd.setCursor(0, 1);
  lcd.print(numPeople);
  
  prevOne = curOne;
  prevTwo = curTwo;
}


boolean isTouchPressed() {
  int sensorValue = digitalRead(PIN_TOUCH);
  return sensorValue > 0.5;
}

boolean isSensorActivated(int previous, int current) {
  return previous < THRESHOLD && current - previous > THRESHOLD;
}

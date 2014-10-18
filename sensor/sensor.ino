/* 
  Requires RGB LCD Library

*/

#include <rgb_lcd.h>
#include <Wire.h>

rgb_lcd lcd;

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

const int PIN_LED = 13;
const int PIN_TOUCH = 4;

const int INPUT_LIGHT = 2;

const int NUM_PREVIOUS_SENSOR_VALUES = 5;
const int THRESHOLD = 75;


int numPeople;
int previousAverage;

float previousSensorValues[NUM_PREVIOUS_SENSOR_VALUES];

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
  delay(100);
  if (isTouchPressed()) {
    numPeople = 0;
  }
  
  // Light
  int lightSensorValue = analogRead(INPUT_LIGHT); 
  int light = (int) (1023 - lightSensorValue) * 10 / lightSensorValue;
  addSensorValue(light);
    
  Serial.println(light);
  
  int average = getAvgSensorValues();
  
  Serial.println(average);
  
  // If the difference in the last 2 values is > threshold --> we have a person
  if(previousSensorValues[NUM_PREVIOUS_SENSOR_VALUES - 1] - previousSensorValues[NUM_PREVIOUS_SENSOR_VALUES - 2] > THRESHOLD) {
     numPeople++;
  }
  
  lcd.setCursor(0, 1);
  lcd.print(numPeople);
  
  previousAverage = average;
}


boolean isTouchPressed() {
  int sensorValue = digitalRead(PIN_TOUCH);
  return sensorValue > 0.5;
}


// Add value to array of previous values. Last In, First Out.
void addSensorValue(int value) {
  for(int i = NUM_PREVIOUS_SENSOR_VALUES - 1; i >= 0; i--) {
    if(i - 1 > 0) {
      previousSensorValues[i-1] = previousSensorValues[i];
    }
  }
  
  previousSensorValues[NUM_PREVIOUS_SENSOR_VALUES - 1] = value;
}


float getAvgSensorValues() {
  int count = 0;
  
  for(int i = 0; i < NUM_PREVIOUS_SENSOR_VALUES; i++) {
    count += previousSensorValues[i];
  }
  
  return (float) count / (float) NUM_PREVIOUS_SENSOR_VALUES;
}

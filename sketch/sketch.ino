#include <rgb_lcd.h>

#include <Wire.h>
#include <math.h>

#define ROTARY_ANGLE_SENSOR A1
#define LED 2//the Grove - LED is connected to D3 of Arduino
#define ADC_REF 5//reference voltage of ADC is 5v.If the Vcc switch on the seeeduino
				 //board switches to 3V3, the ADC_REF should be 3.3
#define GROVE_VCC 5//VCC of the grove interface is normally 5v
#define FULL_ANGLE 300//full value of the rotary angle is 300 degrees

rgb_lcd lcd;

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

const int PIN_LED = 13;
const int PIN_TOUCH = 4;

const int INPUT_TEMP = 0;
const int INPUT_LIGHT = 2;

int tempRead;
float temperature;
int B = 3975;
float resistance;

void setup() {                
    // initialize the digital pin as an output.
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_TOUCH, INPUT);
    pinMode(ROTARY_ANGLE_SENSOR, INPUT);

    Serial.begin(9600);
    
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    lcd.setRGB(colorR, colorG, colorB);
    
    // Print a message to the LCD.
    lcd.print("hello, world!");
    
    delay(1000);
}

void loop() {
    if (isTouchPressed()) {
        digitalWrite(PIN_LED, HIGH);   
    } else {
        digitalWrite(PIN_LED, LOW);
    }
    
    tempRead = analogRead(INPUT_TEMP);
    resistance = (float) (1023 - tempRead) * 10000 / tempRead; //get the resistance of the sensor;
    temperature = 1 / (log(resistance / 10000) / B + 1 / 298.15) - 273.15;//convert to temperature via datasheet ;
    delay(1000);
    
    lcd.setCursor(0, 1);
    
    char buffer[50];
    sprintf(buffer, "Temp is: %f", temperature);
    lcd.print(buffer);
    
    // Rotation
    int degrees = getDegree();
    
    // Light
    int lightSensorValue = analogRead(INPUT_LIGHT); 
    float light = (float) (1023 - lightSensorValue) * 10 / lightSensorValue;
    Serial.println(light);
    
    int red = map(temperature, 20, 30, 0, 255);
    int blue = map(degrees, 0, FULL_ANGLE, 0, 255);
    int green = light;
    
    lcd.setRGB(red, green, blue);
    lcd.setCursor(0, 0);
}


boolean isTouchPressed() {
    int sensorValue = digitalRead(PIN_TOUCH);
    return sensorValue > 0.5;
}


/************************************************************************/
/*Function: Get the angle between the mark and the starting position	*/
/*Parameter:-void							*/
/*Return:	-int,the range of degrees is 0~300			*/
int getDegree()
{
	int sensor_value = analogRead(ROTARY_ANGLE_SENSOR);
	float voltage;
	voltage = (float)sensor_value*ADC_REF/1023;
	float degrees = (voltage*FULL_ANGLE)/GROVE_VCC;
	return degrees;
}


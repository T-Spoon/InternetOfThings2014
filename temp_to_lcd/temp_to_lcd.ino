#include <Wire.h>
#include <rgb_lcd.h>
#include <math.h>


#include <IoTkit.h>    // include IoTkit.h to use the Intel IoT Kit
#include <Ethernet.h>  // must be included to use IoTkit

IoTkit iotkit;

rgb_lcd lcd;

int colorR = 100;
int colorG = 50;
const int colorB = 0;

const int PIN_LED = 13;
const int PIN_TOUCH = 4;

int tempRead;
float temperature;
int B = 3975;
float resistance;

void setup() {                
    // initialize the digital pin as an output.
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_TOUCH, INPUT);

    Serial.begin(9600);
    
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    
    //now we will register sensor names
    system("iotkit-admin activate XIE85FHH > /dev/ttyGS0 2>&1");
    
    iotkit.begin();
    iotkit.send("{ \"n\": \"temp\", \"t\": \"temperature.v1.0\"}\n");
    
//    iotkit.send("{ \"n\": \"humidity sensor\", \"t\": \"humidity.v1.0\"}\n");
    
    // Print a message to the LCD.
    lcd.print("hello, world!");
    
    delay(500);
}

void loop() {
    // put your main code here, to run repeatedly: 
    //digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    //delay(100);               // wait for a second
    //digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
    //delay(100);               // wait for a second
    if (isTouchPressed()) {
        digitalWrite(PIN_LED, HIGH);   
    } else {
        digitalWrite(PIN_LED, LOW);
    }
    
      
    
    
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    //lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    //lcd.print(millis()/1000);

    delay(100);
    
    tempRead = analogRead(0);
    colorR = analogRead(1) % 255;
    colorG = analogRead(2) % 255;
    lcd.setRGB(colorR, colorG, colorB);
//    Serial.print("red:");
//    Serial.println(colorR);
//    Serial.print("green:");
//    Serial.println(colorG);
    
    resistance=(float)(1023-tempRead)*10000/tempRead; //get the resistance of the sensor;
    temperature = 1 / (log(resistance/10000)/B+1/298.15)-273.15;//convert to temperature via datasheet ;
    
//    thanks to guys from intel - doesn't work
//    iotkit.send("temperature sensor", temperature);
    char cmd[50];
    sprintf(cmd, "iotkit-admin observation temp %.2f", temperature);
    
    system(cmd);
    Serial.println(cmd);
    
    Serial.println(temperature);
    delay(1000);
    
    lcd.setCursor(0, 1);
    
    char buffer[50];
    sprintf(buffer, "Temp is: %.2f C", temperature);
    lcd.print(buffer);
}


boolean isTouchPressed() {
    int sensorValue = digitalRead(PIN_TOUCH);
    return sensorValue > 0.5;
}

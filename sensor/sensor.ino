/* 
  Requires RGB LCD Library
  
  A0: First Sensor
  A1: Second Sensor
  
  Pin 4: Touch button to reset counter
  
*/

#include <rgb_lcd.h>
#include <Wire.h>

#include <IoTkit.h>    // include IoTkit.h to use the Intel IoT Kit
#include <Ethernet.h>  // must be included to use IoTkit


IoTkit iotkit;

rgb_lcd lcd;
int inter = 0;
const int divide = 20;

const int colorR = 0;
const int colorG = 0;
const int colorB = 255;

const int PIN_LED = 13;
const int PIN_TOUCH = 4;

const int INPUT_LIGHT_FIRST = 0;
const int INPUT_LIGHT_SECOND = 1;
const int INPUT_TEMPERATURE = 2;

const int NUM_PREVIOUS_SENSOR_VALUES = 5;
const int THRESHOLD = 20;
const int SENSOR_INTERVAL = 50;

// For temperature conversion
int B = 3975;

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
  
  //now we will register sensor names
  system("iotkit-admin activate 3IWXxyTM > /dev/ttyGS0 2>&1");
  iotkit.begin();
  iotkit.send("{ \"n\": \"temp\", \"t\": \"temperature.v1.0\"}\n");
    
  //iotkit.send("{ \"n\": \"humidity sensor\", \"t\": \"humidity.v1.0\"}\n");
  
  
  delay(1000);
}


void loop() {
  delay(SENSOR_INTERVAL);
  
  if (isTouchPressed()) {
    numPeople = 0;
  }
  
  float temperature = 0;
  // Temperature
  int tempRead = analogRead(INPUT_TEMPERATURE);
  float resistance = (float) (1023 - tempRead) * 10000 / tempRead;
  temperature = 1 / (log(resistance/10000)/B+1/298.15)-273.15; // convert to temperature via datasheet
  
  //send every number of cycles 
  if (inter % divide == 0) {    
      char cmd[50];
      sprintf(cmd, "iotkit-admin observation temp %.2f", temperature);
//      sprintf(cmd, "/bin/sh -c \"echo 'observation temp %.2f' >> /tmp/test\"", temperature);
      start(cmd);
    //  system(cmd);
      Serial.println(cmd);
  }
  inter++;
  
  //iotkit.send("temp", temperature);
  //iotkit.receive(callback);
  
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
  
  
  // Print Values to LCD
  char buffer[50];
  sprintf(buffer, "Num People: %2d", numPeople);
  lcd.setCursor(0, 0);
  lcd.print(buffer);
  sprintf(buffer, "Temp: %.2f", temperature);
  lcd.setCursor(0, 1);
  lcd.print(buffer);
  
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

void callback(char* json) {
  Serial.println(json);
  aJsonObject* parsed = aJson.parse(json);
  if (&parsed == NULL) {
    // invalid or empty JSON
    Serial.println("recieved invalid JSON");
    return;
  }
  aJsonObject* component = aJson.getObjectItem(parsed, "component");
  aJsonObject* command = aJson.getObjectItem(parsed, "command");
  if ((component != NULL)) {
    if (strcmp(component->valuestring, "led") == 0) {
      if ((command != NULL)) {
        if (strcmp(command->valuestring, "off") == 0) {
          pinMode(13, OUTPUT);
          digitalWrite(13, false);
        }
        if (strcmp(command->valuestring, "on") == 0) {
          pinMode(13, OUTPUT);
          digitalWrite(13, true);
        }
      }
    }
  }
}

//pid_t start(char *cmd, char **argv, char **envp) {
pid_t start(char *cmd) {  
    pid_t child = fork();
    Serial.println("id");
    Serial.println(child);
    if(child == 0) {
        //execl("iotkit-admin", cmd, NULL);
        //execl("/bin/sh", cmd, NULL);
        system(cmd);
        _exit(1);
    }
    return child;
}

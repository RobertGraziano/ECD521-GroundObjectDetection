#include <AltSoftSerial.h>
AltSoftSerial BTserial; 
#include "MPU6050.h"
#include "I2Cdev.h"     
#include "Wire.h"

char c=' ';
boolean NL = true;
const int trigPin = 11;
const int echoPin = 10;
float duration, distance;
MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t detectionCounter = 0;
boolean detected = false;

struct GyroData {
  byte X;
  byte Y;
  byte Z;
};

GyroData data;

void setup() 
{
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    Serial.begin(9600);
    Serial.print("Sketch:   ");   Serial.println(__FILE__);
    Serial.print("Uploaded: ");   Serial.println(__DATE__);
    Serial.println(" ");
 
    BTserial.begin(9600);  
    Serial.println("BTserial started at 9600");

    Wire.begin();
    mpu.initialize();
}
 
void loop()
{

    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    data.X = map(ax, -17000, 17000, 0, 255 ); // X axis data
    data.Y = map(ay, -17000, 17000, 0, 255); 
    data.Z = map(az, -17000, 17000, 0, 255);  // Y axis data
    Serial.print("Axis X = ");
    Serial.print(data.X);
    Serial.print("  ");
    Serial.print("Axis Y = ");
    Serial.print(data.Y);
    Serial.print("  ");
    Serial.print("Axis Z  = ");
    Serial.println(data.Z);
    
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration*.0343)/2;
	  // Serial.print("Distance: ");  
	  // Serial.println(distance);  
	  delay(100); 

    // Read from the Bluetooth module and send to the Arduino Serial Monitor
    if (BTserial.available())
    {
        c = BTserial.read();;
        Serial.write(c);
        if (c == '1'){
          digitalWrite(10,HIGH);
          digitalWrite(11, LOW);
        }
        else{
          digitalWrite(11, HIGH);
          digitalWrite(10,LOW);
        }
    }
    if(data.X < 200 && data.X > 80)
    {
      if(distance < 20 && detected == false)
      {
          BTserial.write("Alert");
          detectionCounter++;
          detected = true;
      }
      else if (distance > 20)
      {
          BTserial.write("Nothing");
          detected = false;
      }

    }
    else
    {
      BTserial.write("Nothing");
      detected = false;
    }  

    // Read from the Serial Monitor and send to the Bluetooth module
    if (Serial.available())
    {
        c = Serial.read();

        // do not send line end characters to the HM-10
        if (c!=10 & c!=13 ) 
        {  
             BTserial.write(c);
        }
 
        // Echo the user input to the main window. 
        // If there is a new line print the ">" character.
        if (NL) { Serial.print("\r\n>");  NL = false; }
        Serial.write(c);
        if (c==10) { NL = true; }


    }
    Serial.println(detectionCounter);

}
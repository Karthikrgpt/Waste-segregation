#define BLYNK_TEMPLATE_NAME "BinSense"
#define BLYNK_AUTH_TOKEN "8sQdeiD2suoDkQmV4WTYmhOSvFP_TQIy"
#define BLYNK_TEMPLATE_ID "TMPL3nyWxj-JX"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include "MapFloat.h"
LiquidCrystal_I2C lcd(0x27, 20, 4);

#define BLYNK_PRINT Serial

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = " ";  // type your wifi name
char pass[] = " ";  // type your wifi password

const int servoCPin = 32;

const int DIR = 12; 
const int STEP = 13;

const int trig = 14;
const int echo = 27;

const int IR_Sensor = 15;
const int Rain_Sensor = 2;
const int Metal_Sensor = 4;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034

long duration;
int distanceCm,Wet,Dry,Metal;

Servo servoC;

void setup()
{  
 
  Serial.begin(115200);
  
    pinMode(trig, OUTPUT); // Sets the trigPin as an Output
    pinMode(echo, INPUT); // Sets the echoPin as an Input
    pinMode(IR_Sensor, INPUT); 
    pinMode(Rain_Sensor, INPUT); 
    pinMode(Metal_Sensor, INPUT); 
    
    pinMode(STEP, OUTPUT);
    pinMode(DIR, OUTPUT);
    
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    servoC.setPeriodHertz(50);
    servoC.attach(servoCPin,500,2400);
    servoC.write(120);
    
    //lcd.init();
    lcd.begin();
    lcd.backlight();
    Project_Info();
    delay(100);


    Blynk.begin(auth, ssid, pass);
    
    lcd.clear();
    lcd.setCursor(6,1);
    lcd.print("Please Wait");
    lcd.setCursor(0,2);
    lcd.print("Calculating Waste");
    delay(2000);
    Measure();
    lcd.clear();
    lcd.setCursor(4,1);
    lcd.print("Place Your");
    lcd.setCursor(4,2);
    lcd.print("Waste Here");
  }

void loop()
{
  Blynk.run();
  if(digitalRead(IR_Sensor)==LOW)
  {
    lcd.clear();
    lcd.setCursor(3,1);
    lcd.print("ThankYou For");
    lcd.setCursor(5,2);
    lcd.print("Using Bin");
    Blynk.virtualWrite(V3,"Waste Detected");
    delay(3000);
    lcd.setCursor(0,3);
    lcd.print("Waste: ");
    delay(1000);
    if(digitalRead(Metal_Sensor)==HIGH)
    {
      
      lcd.print("METAL ");
      Blynk.virtualWrite(V3,"METAL Waste Detected");
      delay(3000);
      servoC.write(60);
      delay(3000);
      while(digitalRead(IR_Sensor)==LOW);
      delay(1000);
      servoC.write(120);
      delay(3000);
      
    }
    else if(digitalRead(Rain_Sensor)==LOW)
    {
     
      lcd.print("WET ");
      Blynk.virtualWrite(V3,"Wet Waste Detected");
      delay(1000);
      digitalWrite(DIR, HIGH);//clkwise
      for(int i = 0; i<70; i++)
      {
        digitalWrite(STEP, HIGH);
        delay(30);
        digitalWrite(STEP, LOW);
        delay(30);
      }
      
      delay(3000);
      servoC.write(60);
      delay(3000);
      while(digitalRead(IR_Sensor)==LOW);
      delay(1000);
      servoC.write(120);
      delay(3000);
      digitalWrite(DIR, LOW);//counter clkwise
      for(int i = 0; i<70; i++)
      {
        digitalWrite(STEP, HIGH);
        delay(30);
        digitalWrite(STEP, LOW);
        delay(30);
      }
      delay(3000);
     
    }
    else
    {
     
      lcd.print("DRY ");
      Blynk.virtualWrite(V3,"Dry Waste Detected");
      delay(1000);
      digitalWrite(DIR, LOW);
      for(int i = 0; i<80; i++)
      {
        digitalWrite(STEP, HIGH);
        delay(30);
        digitalWrite(STEP, LOW);
        delay(30);
      }
      
      delay(3000);
      servoC.write(60);
      delay(3000);
      while(digitalRead(IR_Sensor)==LOW);
      delay(1000);
      servoC.write(120);
      delay(3000);
      digitalWrite(DIR, HIGH);
      for(int i = 0; i<80; i++)
      {
        digitalWrite(STEP, HIGH);
        delay(30);
        digitalWrite(STEP, LOW);
        delay(30);
      }
      delay(3000);
  
    }
    Measure();
  }
  else
  {
    lcd.clear(); 
    Blynk.virtualWrite(V3,"Ready To Use");
    lcd.print("Waste Calculator");
    lcd.setCursor(0,1);   
    lcd.print("WET  : ");
    lcd.print(Wet);
    Blynk.virtualWrite(V0,Wet);
    lcd.print(" % ");
    lcd.setCursor(0,2);   
    lcd.print("DRY  : ");
    lcd.print(Dry);
    Blynk.virtualWrite(V1,Dry);
    lcd.print(" % ");
    lcd.setCursor(0,3);   
    lcd.print("METAL: ");
    lcd.print(Metal);
    Blynk.virtualWrite(V2,Metal);
    lcd.print(" % ");
    delay(1000);
    lcd.clear();
    lcd.setCursor(4,1);
    lcd.print("Place Your");
    lcd.setCursor(4,2);
    lcd.print("Waste Here");
    delay(1000);
  }
 }
 void Measure()
{
    // Clears the trigPin
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    distanceCm =0;
    duration =0;
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echo, HIGH);
    distanceCm = duration * SOUND_SPEED/2;
    Wet=0;
    Wet = abs(mapFloat(distanceCm, 34.0, 0.0, 0.0, 100.0));
    lcd.clear(); 
    lcd.print("Waste Calculator");
    lcd.setCursor(0,1);   
    lcd.print("WET  : ");
    lcd.print(Wet);
    Blynk.virtualWrite(V0,Wet);
    lcd.print(" % ");
    delay(1000);
    
    digitalWrite(DIR, HIGH);//clkwise
    for(int i = 0; i<50; i++)
    {
        digitalWrite(STEP, HIGH);
        delay(30);
        digitalWrite(STEP, LOW);
        delay(30);
    } 
    delay(1000);

   
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    distanceCm =0;
    duration =0;
    
    duration = pulseIn(echo, HIGH);
    distanceCm = duration * SOUND_SPEED/2;
    Dry=0;
    Dry = abs(mapFloat(distanceCm, 34.0, 0.0, 0.0, 100.0));
    lcd.setCursor(0,2);   
    lcd.print("DRY  : ");
    lcd.print(Dry);
    Blynk.virtualWrite(V1,Dry);
    lcd.print(" % ");
    delay(1000);
    
    digitalWrite(DIR, HIGH);//clkwise
    for(int i = 0; i<80; i++)
    {
        digitalWrite(STEP, HIGH);
        delay(30);
        digitalWrite(STEP, LOW);
        delay(30);
    } 
    delay(1000);


    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    distanceCm =0;
    duration =0;
    
    duration = pulseIn(echo, HIGH);
    distanceCm = duration * SOUND_SPEED/2;
    Metal=0;
    Metal=abs(mapFloat(distanceCm, 38.0, 0.0, 0.0, 100.0));
    lcd.setCursor(0,3);   
    lcd.print("METAL: ");
    lcd.print(Metal);
    Blynk.virtualWrite(V2,Metal);
    lcd.print(" % ");
    delay(1000);
    
    digitalWrite(DIR, LOW);//clkwise
    for(int i = 0; i<130; i++)
    {
        digitalWrite(STEP, HIGH);
        delay(30);
        digitalWrite(STEP, LOW);
        delay(30);
    } 
    
    delay(3000);
}
    

void Project_Info()
{
  
    lcd.clear();
    lcd.setCursor(6,1);
    lcd.print("Automatic");
    lcd.setCursor(3,2);
    lcd.print("Waste Segregator");
    delay(2000);
    lcd.clear();
    lcd.clear();
    lcd.setCursor(6,0);
    lcd.print("Automatic");
    lcd.setCursor(3,1);
    lcd.print("Waste Segregator");
    lcd.setCursor(0,3);
    lcd.print("Connecting Wifi..");
    delay(2000);
}

 

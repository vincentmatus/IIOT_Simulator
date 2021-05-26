/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP32 chip.

  Note: This requires ESP32 support package:
    https://github.com/espressif/arduino-esp32

  Please be sure to select the right ESP32 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#define pulse_ip 12
#define DAC1 25

#include <BlynkSimpleEsp32.h>

unsigned long v1 =0;
char auth[] = "tpZ0QYNFuASYweqRJ2xjxkOo-kJtZrJI";

char ssid[] = "DesktopAP-2.4G";
char pass[] = "ap191140";

int ontime=0,offtime=0;
float freq=0,period=0;

void setup() 
{            
  pinMode(pulse_ip,INPUT);
  Serial.begin(9600);
  delay(10); 
  
  ledcAttachPin(2, 1);
  ledcAttachPin(14, 2);
  
  // Initialize channels 
  // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
  // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
  ledcSetup(1, 400, 8); // 400 Hz PWM, 10-bit resolution

  Serial.println("Start");
  Blynk.begin(auth, ssid, pass, "blynk.iot-cm.com", 8080);
  
}

BLYNK_WRITE(V0){
  unsigned long value = param.asInt();
  unsigned long y = map(value,1,100,1000,100000);
  ledcSetup(1, y, 8);
  Serial.print("value: ");
  Serial.println(value);
  Serial.print("y: ");
  Serial.println(y);
}

//BLYNK_WRITE(V1){
//  unsigned long value = param.asInt();
//  v1 = map(value,4,20,0,1023);
//  ledcSetup(2, 5000, 10);
//  ledcWrite(2, v1);
//  Serial.print("value_420: ");
//  Serial.println(value);
//  Serial.print("y_420: ");
//  Serial.println(v1);
//}

BLYNK_WRITE(V1){
  unsigned long value = param.asInt();
  v1 = map(value,4,20,0,255);
  dacWrite(DAC1, v1);
  Serial.print("value_420: ");
  Serial.println(value);
  Serial.print("y_420: ");
  Serial.println(v1);
}

void loop() 
{
  Blynk.run();
  freqMeasure();
  ledcWrite(1, 128);
  



 
}

//void freqMeasure(){
  // ontime = pulseIn(pulse_ip,HIGH);
   //offtime = pulseIn(pulse_ip,LOW);
   //period = ontime+offtime;
   //freq = 1000000.0/period;
   //Serial.print("freq = ");
   //Serial.println(freq);
   //Serial.println(period);
   //Serial.println(ontime);
   //Serial.println(offtime);
//}

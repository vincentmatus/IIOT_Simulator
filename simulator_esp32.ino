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

#include <BlynkSimpleEsp32.h>

char auth[] = "tpZ0QYNFuASYweqRJ2xjxkOo-kJtZrJI";

char ssid[] = "VCWF";
char pass[] = "12930A11";

int ontime=0,offtime=0;
float freq=0,period=0;

void setup() 
{            
  pinMode(pulse_ip,INPUT);
  Serial.begin(9600);
  delay(10); 
  
  ledcAttachPin(5, 1);
  
  // Initialize channels 
  // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
  // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
  ledcSetup(1, 4000, 10); // 400 Hz PWM, 10-bit resolution

  Serial.println("Start");
  Blynk.begin(auth, ssid, pass, "blynk.iot-cm.com", 8080);
  
}

BLYNK_WRITE(V0){
  unsigned long value = param.asInt();
  unsigned long y = map(value,1,100,1,100000);
  ledcSetup(1, y, 10);
  Serial.print("value: ");
  Serial.println(value);
  Serial.print("y: ");
  Serial.println(y);
}

void loop() 
{
  Blynk.run();
  freqMeasure();
  ledcWrite(1, 1022);




 
}

void freqMeasure(){
   ontime = pulseIn(pulse_ip,HIGH);
   offtime = pulseIn(pulse_ip,LOW);
   period = ontime+offtime;
   freq = 1000000.0/period;
   Serial.print("freq = ");
   Serial.println(freq);
   Serial.println(period);
   Serial.println(ontime);
   Serial.println(offtime);
}

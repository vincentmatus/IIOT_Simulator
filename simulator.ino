#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "ENHBE3IUPHM5lNb6E1S_BDRBYGF4aXwa";
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "VCWF";
char pass[] = "12930A11";

//pulse generator
unsigned long pM = 0;    //pulse time
unsigned long sM = 0;   //sequence time
unsigned long M = 0;    //time
int pT = 10;
int count = 0;
int out = 13;       //pulse output pin
int rn = 0;

void setup()
{
  // Debug console
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);


//pulse generator
  pinMode(out, OUTPUT);
  Serial.begin(9600);
  delay(10000);     //delay 1 s to allow time to start serial monitor
  
}
void loop()
{
  Blynk.run();

  
//pulse generator
  M = millis();      //read millis and save

  if (M < pM + pT)    //when time for a pulse - emit pulse 1% of time
    return;
  rn = random(0,2);
  digitalWrite(out, rn);
  Serial.println(rn);
  delayMicroseconds(1000);
  //digitalWrite(out, LOW);
  //Serial.println(0);
  pM = M;         //start over

  if(M < sM + 1000)   //after 1 s - next pulse time
    return;
  pT = pT + 5;
  sM = M;         //start over
  //Serial.println(pT);   //print speed

  if(pT < 100)      //when all speeds are done - start from the beginning
    return;
  pT = 10;
  
}

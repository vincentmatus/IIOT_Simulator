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
void setup()
{
  // Debug console
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
}
void loop()
{
  Blynk.run();
}

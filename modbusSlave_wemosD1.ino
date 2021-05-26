/*
  ModbusRTU ESP8266/ESP32
  Simple slave example
  (c)2019 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/modbus-esp8266
  modified 13 May 2020
  by brainelectronics
  This code is licensed under the BSD New License. See LICENSE.txt for more info.
*/

#include <ModbusRTU.h>

int REGN[] = {0,1,2,3,4};//,5,6,7,8,9};
int VALUE[] = {1,1000,700,40,50};//,60,70,80,90,100};
#define SLAVE_ID 1
#define RXTX_PIN 14
ModbusRTU mb;

void setup() {
  Serial.begin(9600, SERIAL_8N1);
  mb.begin(&Serial, RXTX_PIN);  // use RX/TX direction control pin (if required)
  mb.setBaudrate(9600);
  mb.slave(SLAVE_ID);
  for(int i=0;i<sizeof(REGN);i++){
  mb.addHreg(REGN[i]);
  mb.Hreg(REGN[i], VALUE[i]);
  }
}

void loop() {
  mb.task();
  yield();
}

#include <Ethernet.h>
#include <MQTT.h>
#include <CaptureTimer.h>
#include <ModbusMaster.h>
#include<String.h>
#define ctINPin     2   // Pin used for Input Capture (ticks count)
#define samplingPer   500
#define MAX485_DE      9
#define MAX485_RE_NEG  8

ModbusMaster device1;
ModbusMaster device2;
ModbusMaster device3;
int ipc = 13;
int tot = 2000;
int bad = 50;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[] = {192, 168, 1, 99};  // <- change to match your network
const byte adcPin = 0;  // input only 0-7 for A0-A7 and A8-A15
const int MAX_RESULTS = 8;
volatile int results [MAX_RESULTS];
volatile int resultsMap[8];

EthernetClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void connect() {
  Serial.print("connecting...");
  while (!client.connect(("", "", ""))) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("uRTU");
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

}




void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

void write_SingleRegister(ModbusMaster node, uint16_t first_addr , int value) {
  char buffer[500];

  Serial.println("----------writeSingleRegisters-----------");
  node.writeSingleRegister(first_addr, value);
  sprintf(buffer, "write addr %x = %d", first_addr, value);
  Serial.println(buffer);

}

int32_t getInputRegisters(ModbusMaster node, uint16_t first_addr , uint16_t quantity , uint16_t register_set) {
  uint8_t result;
  uint16_t data[quantity];
  uint16_t setdata[(quantity / register_set) + (quantity % register_set)];

  result = node.readInputRegisters(first_addr, quantity);
  Serial.println("----------readHoldingRegisters-----------");

  // do something with data if read is successful
  if (result == node.ku8MBSuccess)
  {
    for (int j = 0; j < quantity; j++)
    {
      data[j] = node.getResponseBuffer(j);
      Serial.println(data[j]);
    }
    if (register_set > 1) {
      int m = 0;
      for (int k = 0; k < quantity; k += register_set) {

        for (int i = k; i < register_set + k; i++)
        {
          setdata[m] = setdata[m] + data[i];
        }
        setdata[m] = setdata[m] << 16;
        m += 1;
      }
      return setdata;
    }
    return data;
  }
  else {
    Serial.println("Error readInputRegisters !!");
  }

}

int32_t getHoldingRegisters(ModbusMaster node, uint16_t first_addr , uint16_t quantity , uint16_t register_set) {
  uint8_t result;
  uint16_t data[quantity];
  uint16_t setdata[(quantity / register_set) + (quantity % register_set)];

  result = node.readHoldingRegisters(first_addr, quantity);
  Serial.println("----------readHoldingRegisters-----------");

  // do something with data if read is successful
  if (result == node.ku8MBSuccess)
  {
    for (int j = 0; j < quantity; j++)
    {
      data[j] = node.getResponseBuffer(j);
      Serial.println(data[j]);
    }
    if (register_set > 1) {
      int m = 0;
      for (int k = 0; k < quantity; k += register_set) {

        for (int i = k; i < register_set + k; i++)
        {
          setdata[m] = setdata[m] + data[i];
        }
        setdata[m] = setdata[m] << 16;
        m += 1;
      }
      return setdata;
    }
    return data;
  }
  else {
    Serial.println("Error readHoldingRegisters !!");
  }

}





void setup() {
  Serial.begin(115200);
  Ethernet.begin(mac, ip);
  client.begin("161.246.6.140", net);
   CaptureTimer::initCapTicks(samplingPer, ctINPin);
TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  TCCR1B = bit (CS11) | bit (WGM12);  // CTC, prescaler of 8
  TIMSK1 = bit (OCIE1B);
  OCR1A = 39;    
  OCR1B = 39;   // 20 uS - sampling frequency 50 kHz

  ADMUX = bit (REFS0) | (adcPin & 7); //select Voltage REFerence , ADLAR (ADc Left Adjusted Result) and pin for A0-A7
 
  ADCSRA =  bit (ADEN) | bit (ADIE) | bit (ADIF);   // turn ADC on, want interrupt on completion
  ADCSRA |= bit (ADPS2);  // Prescaler of 16
  ADCSRB = bit (ADTS0) | bit (ADTS2);  // Timer/Counter1 Compare Match B
  ADCSRA |= bit (ADATE);   // turn on automatic triggering



  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  // Init in receive mode
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
  Serial2.begin(9600, SERIAL_8N1);

  // Modbus slave ID 1
  device1.begin(1, Serial2);
    device2.begin(2, Serial2);
      device1.preTransmission(preTransmission);
  device1.postTransmission(postTransmission);
  device2.preTransmission(preTransmission);
  device2.postTransmission(postTransmission);
  getHoldingRegisters(device1, 0x0000, 5, 1);

  
  if (!client.connected()) {
    while (!client.connect("", "", "")) {
    Serial.print(".");
    delay(1000);
  }
    connect();
  }
}


ISR (ADC_vect)
{
   //results[0] = ADCL | (ADCH << 8);//alway read Low byte first!!;
    int MX5=0;
    //if(ADCSRB==0x08){MX5=8;}
    switch(ADMUX)
    {
        case 0x40:
             results[0+MX5] = ADCL | (ADCH << 8);//alway read Low byte first!!;
             ADMUX = 0x41;
          break;
        case 0x41:
             results[1+MX5] = ADCL | (ADCH << 8);//alway read Low byte first!!;
             ADMUX = 0x42;
          break;
        case 0x42:
             results[2+MX5] = ADCL | (ADCH << 8);//alway read Low byte first!!;
             ADMUX = 0x43;
         break;
        case 0x43:
             results[3+MX5] = ADCL | (ADCH << 8);//alway read Low byte first!!;
             ADMUX = 0x44;
          break;
        case 0x44:
             results[4+MX5] = ADCL | (ADCH << 8);//alway read Low byte first!!;
             ADMUX = 0x45;
         break;
        case 0x45:
             results[5+MX5] = ADCL | (ADCH << 8);//alway read Low byte first!!;
             ADMUX = 0x46;
          break;
        case 0x46:
             results[6+MX5] = ADCL | (ADCH << 8);//alway read Low byte first!!;
             ADMUX = 0x47;
         break;
        case 0x47:
             results[7+MX5] = ADCL | (ADCH << 8);//alway read Low byte first!!;
             ADMUX = 0x40;
         break;
        default:
          //default code
          break;
    }
} 

EMPTY_INTERRUPT (TIMER1_COMPB_vect);



void loop() {

  uint16_t data[10];
  uint16_t ticks;
  data = getHoldingRegisters(device1, 0x0000, 10, 1);


 resultsMap [0]= (results [0]*100.0)/1023.0;
 if (CaptureTimer::getTicks(&ticks) == true){
    (void) CaptureTimer::getFreq(&ticks);
  }



 
    String text = "{"on":"+ String(1) +" ,"ipc":"+ String(ticks) +" ,"vcl":"+ String(resultsMap [0])+" ,"tot":"+ String(data[1]) +" ,"bad":"+ String(data[2]) +"}";
     Serial.println(text);
    //Serial.println(around);
     client.publish("RTU", text);
     delay (1000);
  if (!client.connected()) {

    connect();
    }
    Serial.println(String(!client.connected()));


 

 

}


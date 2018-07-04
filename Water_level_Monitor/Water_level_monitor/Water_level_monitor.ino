#include <SoftwareSerial.h>
#include <stdlib.h>

// LED 
int ledPin = 13;
// define input pins
int high_level = 6;
int med_level= 5;
int low_level= 4;

// replace with your channel's thingspeak API key, please note that API key will be different for every channel (or experiment)
String apiKey = "899A4ZEQGQGAZUTH";

// connect 2 to TX of Serial USB
// connect 3 to RX of serial USB
SoftwareSerial ser(2, 3); // RX, TX

// this runs once
void setup() {                
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);

  pinMode(high_level,INPUT);
  pinMode(med_level,INPUT);
  pinMode(low_level,INPUT);

  digitalWrite(high_level,LOW);
  digitalWrite(med_level,LOW);
  digitalWrite(low_level,LOW);

  // enable debug serial
  Serial.begin(9600); 
  // enable software serial
  ser.begin(9600);
  
  // reset ESP8266
  ser.println("AT+RST");
}

int temp=0;

// the loop 
void loop() {
  
  // blink LED on board
  digitalWrite(ledPin, HIGH);   
  delay(200);               
  digitalWrite(ledPin, LOW);

  if (digitalRead(high_level)== LOW && digitalRead(med_level)==LOW && digitalRead(low_level)==LOW)
  {temp=0;}

  if (digitalRead(high_level)== LOW && digitalRead(med_level)==LOW && digitalRead(low_level)==HIGH)
  {temp=1;}

  if (digitalRead(high_level)== LOW && digitalRead(med_level)==HIGH && digitalRead(low_level)==HIGH)
  {temp=2;}

  if (digitalRead(high_level)== HIGH && digitalRead(med_level)==HIGH && digitalRead(low_level)==HIGH)
  {temp=3;}

  // convert to string
  char buf[16];
  String strTemp = dtostrf(temp, 4, 1, buf);
  
  Serial.println(strTemp);
  
  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  ser.println(cmd);
   
  if(ser.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }
  
  // prepare GET string
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String(strTemp);
  getStr += "\r\n\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  ser.println(cmd);

  if(ser.find(">")){
    ser.print(getStr);
  }
  else{
    ser.println("AT+CIPCLOSE");
    // alert user
    Serial.println("AT+CIPCLOSE");
  }
    
  // thingspeak needs 15 sec delay between updates
  delay(16000);  
}

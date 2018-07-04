#include <SoftwareSerial.h>
#include <stdlib.h>

// LED 
int ledPin = 13;
// Moisture Sensor pin
int Moisture = 0;

// replace with your channel's thingspeak API key
String apiKey = "03CJLA4R05BFE669";

// connect 2 to TX of ESP8266 Breakout
// connect 3 to RX of ESP8266 Breakout
SoftwareSerial ser(2, 3); // RX, TX

// this runs once
void setup() {                
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);    

  // enable debug serial
  Serial.begin(9600); 
  // enable software serial
  ser.begin(9600);
  
  // reset ESP8266
  ser.println("AT+RST");
}


// the loop 
void loop() {
  
  // blink LED on board
  digitalWrite(ledPin, HIGH);   
  delay(200);               
  digitalWrite(ledPin, LOW);

  // read the value from Moisture Sensor.
  // read 10 values for averaging.
  int val = 0;
  for(int i = 0; i < 10; i++) {
      val += analogRead(Moisture);   
      delay(500);
  }

  // Store value into temp variable:
  float temp = val/10;

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

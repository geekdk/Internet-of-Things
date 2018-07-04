#include <SoftwareSerial.h>
#include <stdlib.h>
#include <NewPing.h>

#define TRIGGER_PIN  5  
#define ECHO_PIN     4  
#define MAX_DISTANCE 300 

// LED 
int ledPin = 13;
//Buzzer Pin
int Buzzer = 6;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 

// replace with your channel's thingspeak API key
String apiKey = "OH4YP8GSSNQYBAII";

// connect 2 to TX of Serial USB
// connect 3 to RX of serial USB
SoftwareSerial ser(2, 3); // RX, TX

// this runs once
void setup() {                
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT); 
  pinMode(Buzzer, OUTPUT);    

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

  // read the value from LM35.
  // read 10 values for averaging.
  int val = 0;
  for(int i = 0; i < 10; i++) {
      unsigned int uS = sonar.ping(); //get distance from ultrasonic sensor
      val=uS / US_ROUNDTRIP_CM;  
      delay(500);
  }

  if(val<100) // make buzzer high if the value of distance is lower than 100cm
  {
  digitalWrite(Buzzer,HIGH);  
  }

  if(val>100) // make buzzer low if the value of distance is greater than 100cm
  {
  digitalWrite(Buzzer,LOW);  
  }

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

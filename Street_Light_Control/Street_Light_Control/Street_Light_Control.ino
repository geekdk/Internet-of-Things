 #include <SoftwareSerial.h>
#include <stdlib.h>

// LED 
int ledPin = 13;

int LDR_pin = 0;
int relay=10;

// replace with your channel's thingspeak API key, please note that API key will be different for every channel (or experiment)
String apiKey = "WTL5SIMLXYAP566W";

// connect 2 to TX of Serial USB
// connect 3 to RX of serial USB
SoftwareSerial ser(2, 3); // RX, TX

// this runs once
void setup() {                
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
  pinMode(relay, OUTPUT);
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

  if(analogRead(A0)< 512) //detect when light intensity low
  {
   digitalWrite(relay,LOW); //turn on the relay
   temp=1;                   // feed temp variable with 1 to indicate relay is ON
  }

  if(analogRead(A0)> 512) //detect when light intensity HIGH
  {
   digitalWrite(relay,HIGH);  //turn off the relay
   temp=0;                    // feed temp variable with 0 to indicate relay is OFF
  }

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

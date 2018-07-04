#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); // RX, TX
void setup() {
  Serial.begin(4800); // Open serial communications and wait for port to open:
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.println("Starting…..!");//Printing message to Serial Moniter

  mySerial.begin(115200); // set the data rate for the SoftwareSerial port
  mySerial.println("Hello, world?");
}
void loop() { //This will  run over and over
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}


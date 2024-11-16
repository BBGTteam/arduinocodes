/*********
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <SoftwareSerial.h>

// Configure software serial port
SoftwareSerial SIM900(7, 8);
//Variable to save incoming SMS characters
char incoming_char=0;

int gombPin = 10; 
String smsStatus,senderNumber,receivedDate,msg, pass;

void setup() {
  // Arduino communicates with SIM900 GSM shield at a baud rate of 19200
  // Make sure that corresponds to the baud rate of your module
  pinMode(gombPin, OUTPUT);
  SIM900.begin(19200);
  Serial.println("SIM900 software serial initialize");
  Serial.begin(19200); 
  Serial.println("Arduino serial initialize");
  Serial.println("Give 5 sec to your GSM shield log on to network");
  delay(5000);

  smsStatus = "";
  senderNumber="";
  receivedDate="";
  msg="";
  pass= "";

  // AT command to set SIM900 to SMS mode
  SIM900.print("ATE1"); 

  SIM900.print("AT+CMGF=1\r"); //SMS text mode
  delay(100);
  // Set module to send SMS data to serial out upon receipt 
  SIM900.print("AT+CNMI=2,2,0,0,0\r");
  delay(100);
}

void loop() {
  // Display any text that the GSM shield sends out on the serial monitor
  if(SIM900.available() >0) {
    //Get the character from the cellular serial port
    incoming_char=SIM900.read(); 
    //Print the incoming character to the terminal
    Serial.print(incoming_char); 
  }
}
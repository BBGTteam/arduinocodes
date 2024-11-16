/************************************************************************************
 *  Created By: Tauseef Ahmad
 *  Created On: December 26, 2021
 *  
 *  Tutorial Video: https://youtu.be/-lFoWN3dNTY
 *  My Channel: https://www.youtube.com/channel/UCOXYfOHgu-C-UfGyDcu5sYw/
 *  
 *  *********************************************************************************/
 
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#include<SoftwareSerial.h>
//GSM Module RX pin to Arduino 3
//GSM Module TX pin to Arduino 2
#define rxPin 3
#define txPin 2
SoftwareSerial sim800(rxPin,txPin);
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
int gombPin = 8; 
String smsStatus,senderNumber,receivedDate,msg,pass;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
void setup() {
  //delay(7000);
  pinMode(gombPin, OUTPUT);
  //---------------------------------------------------------
  Serial.begin(9600);
  Serial.println("Arduino serial initialize");
  //---------------------------------------------------------
  sim800.begin(9600);
  Serial.println("SIM800L software serial initialize");
  //---------------------------------------------------------
  smsStatus = "";
  senderNumber="";
  receivedDate="";
  msg="";
  pass= "";
  //---------------------------------------------------------
  sim800.println("ATE1"); //SMS text mode
  delay(1000);
  sim800.println("AT+CMGF=1"); //SMS text mode
  delay(1000);
  sim800.println("AT+IPR=9600"); // Set baud rate for communication
  delay(1000);
  sim800.println("AT+CNMI=2,1,0,0,0"); // Set baud rate for communication
  delay(1000);
  //---------------------------------------------------------
}
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN





//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
void loop() {
//---------------------------------------------------------
while(sim800.available()){
  parseData(sim800.readString());
}
//---------------------------------------------------------
while(Serial.available())  {
  sim800.println(Serial.readString());
}
//---------------------------------------------------------
} //main loop ends
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN





//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
void parseData(String buff){
  Serial.println(buff);

  unsigned int len, index;
  //---------------------------------------------------------
  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index+2);
  buff.trim();
  //---------------------------------------------------------
  
  //---------------------------------------------------------
  if(buff != "OK"){
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();
    
    buff.remove(0, index+2);
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    if(cmd == "+CMTI"){
      //get newly arrived memory location and store it in temp
      index = buff.indexOf(",");
      String temp = buff.substring(index+1, buff.length()); 
      temp = "AT+CMGR=" + temp + "\r"; 
      //get the message stored at memory location "temp"
      sim800.println(temp); 
    }
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    else if(cmd == "+CMGR"){
      extractSms(buff);
      
      //if received sms have at command
      if(msg.startsWith("AT+") || msg.startsWith("AT")){
        sendRseponseATcommand(msg);
        smsStatus = "";
        senderNumber="";
        receivedDate="";
        msg=""; 
      }
    }
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  }
  //---------------------------------------------------------
  else{
  //The result of AT Command is "OK"
  }
  //---------------------------------------------------------
}
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN



//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
void sendRseponseATcommand(String ATcommand){
  //---------------------------------------------------------
  if (ATcommand.equals("")){
    Serial.println("No AT Command is found.");
    return;
  }
  //---------------------------------------------------------
  uint8_t answer=0;
  unsigned int timeout = 3000;
  String response;
  unsigned long previous;
  delay(100);
  //---------------------------------------------------------
  //Clean the input buffer
  while(sim800.available() > 0) sim800.read();
  //---------------------------------------------------------
  //Send the AT command 
  sim800.println(ATcommand);
  //---------------------------------------------------------
  previous = millis();
  //---------------------------------------------------------
  //this loop waits for the answer with time out
  do{
    //if there are data in the UART input buffer, reads it.
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    if(sim800.available() != 0){
      response = sim800.readString();
      
      if(response != "")
        answer == 1;
    }
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  }while((answer == 0) && ((millis() - previous) < timeout));
  //---------------------------------------------------------
  //Remove sent "AT Command" from the response string.
  int index = response.indexOf("\r");
  response.remove(0, index+2);
  response.trim();
  Serial.println("---------------------");
  Serial.println(response);
  Serial.println("---------------------");
  //---------------------------------------------------------
  Reply(response);
  //---------------------------------------------------------
}
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN




//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
void extractSms(String buff){
  unsigned int index;
  //---------------------------------------------------------
  index = buff.indexOf(",");
  smsStatus = buff.substring(1, index-1); 
  buff.remove(0, index+2);
  //---------------------------------------------------------
  senderNumber = buff.substring(0, 12);
  Serial.println("senderNumber: " + senderNumber);
  buff.remove(0,18);
  //---------------------------------------------------------
  receivedDate = buff.substring(0, 19);
  Serial.println("receivedDate:" + receivedDate);
  buff.remove(0,buff.indexOf("\r"));
  buff.trim();
  //---------------------------------------------------------
  index =buff.indexOf("\n\r");
  buff = buff.substring(0, index);
  buff.trim();
  msg = buff;
  buff = "";
  Serial.println("message:" + msg);
  //msg.toLowerCase();
  //---------------------------------------------------------
  if (msg.substring(0,3) == "pass")
  {
    pass = msg.substring(5);
    msg.remove(0,4);
  }

  if (msg == pass)
  {
    Serial.println("Button pushed");
    digitalWrite(gombPin, HIGH); // Gomb aktiválása
    delay(2000); // 1 másodpercig tartva

    digitalWrite(gombPin, LOW); // Gomb kikapcsolása
    delay(1000); // 1 másodperc szünet
    msg = "";           // SMS törlése, hogy új üzenetre kész legyen
  }
}
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN




//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
void Reply(String text)
{
    sim800.print("AT+CMGF=1\r");
    delay(1000);
    //sim800.print("AT+CMGS=\""+senderNumber+"\"\r");
    sim800.print("AT+CMGS=\""+senderNumber+"\"\r");
    delay(1000);
    sim800.print(text);
    delay(100);
    //ascii code for ctrl-26 
    //sim800.println((char)26); //ascii code for ctrl-26
    sim800.write(0x1A); 
    delay(1000);
    Serial.println("SMS Sent Successfully.");
}
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
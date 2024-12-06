/*********
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <SoftwareSerial.h>

// Configure software serial port
SoftwareSerial SIM900(7, 8);

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
  pass= "Asdf";

  // AT command to set SIM900 to SMS mode
  SIM900.println("ATE1"); 
  delay(1000);

  SIM900.println("AT+CMGF=1"); //SMS text mode
  delay(1000);
  // Set module to send SMS data to serial out upon receipt 
  SIM900.println("AT+CNMI=2,2,0,0,0");
  delay(1000);
}

void loop() {

  while(SIM900.available()){
    parseData(SIM900.readString());
  }
  //---------------------------------------------------------
  while(Serial.available())  {
    SIM900.println(Serial.readString());
  }
  }
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
void parseData(String buff){
  Serial.println(buff);

  unsigned int len, index;
  //---------------------------------------------------------
  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index+2);
  Serial.println(buff);
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
      SIM900.println(temp); 
    }
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    else if(cmd == "+CMT"){
      Serial.println("extract SMS");
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
  while(SIM900.available() > 0) SIM900.read();
  //---------------------------------------------------------
  //Send the AT command 
  SIM900.println(ATcommand);
  //---------------------------------------------------------
  previous = millis();
  //---------------------------------------------------------
  //this loop waits for the answer with time out
  do{
    //if there are data in the UART input buffer, reads it.
    //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    if(SIM900.available() != 0){
      response = SIM900.readString();
      
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
  //index = buff.indexOf(",");
  //smsStatus = buff.substring(1, index-1); 
  buff.remove(0, 1);
  //---------------------------------------------------------
  senderNumber = buff.substring(0, 12);
  Serial.println("senderNumber: " + senderNumber);
  buff.remove(0,18);
  //---------------------------------------------------------
  receivedDate = buff.substring(0, 20);
  Serial.println("receivedDate:" + receivedDate);
  buff.remove(0,buff.indexOf("\r"));
  buff.trim();
  //---------------------------------------------------------
  index =buff.indexOf("\n\r");
  buff = buff.substring(0, index);
  buff.trim();
  msg = buff;
  buff = "";
  Serial.println("message: " + msg);
  msg.trim();
  //msg.toLowerCase();
  //---------------------------------------------------------
  if (msg.startsWith("pass"))
  {
    pass = msg.substring(5);
    Serial.println("Password change to: " + pass);
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
    SIM900.print("AT+CMGF=1\r");
    delay(1000);
    //SIM900.print("AT+CMGS=\""+senderNumber+"\"\r");
    SIM900.print("AT+CMGS=\""+senderNumber+"\"\r");
    delay(1000);
    SIM900.print(text);
    delay(100);
    //ascii code for ctrl-26 
    //SIM900.println((char)26); //ascii code for ctrl-26
    SIM900.write(0x1A); 
    delay(1000);
    Serial.println("SMS Sent Successfully.");
}
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

#include <SoftwareSerial.h>



String incomingSMS = "";       // Az SMS üzenet tárolására
String buffer = "";            // A beérkező adatokat ebben gyűjtjük
bool smsStart = false;         // Jelzi, ha SMS érkezett és az SMS szövegét várjuk
int gombPin = 8; 

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(3, 2); //SIM800L Tx & Rx is connected to Arduino #3 & #2

void setup()
{
  pinMode(gombPin, OUTPUT);
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  
  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);

  Serial.println("Initializing..."); 
  delay(1000);

  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  delay(500);
  mySerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();
  delay(500);
  mySerial.println("AT+IPR=9600"); // Set baud rate for communication
  updateSerial();
}

void loop()
{
  updateSerial();

  if (incomingSMS == "A")
  {
    Serial.println("SMS received: " + incomingSMS);
    digitalWrite(gombPin, HIGH); // Gomb aktiválása
    delay(2000); // 1 másodpercig tartva

    digitalWrite(gombPin, LOW); // Gomb kikapcsolása
    delay(1000); // 1 másodperc szünet
    incomingSMS = "";           // SMS törlése, hogy új üzenetre kész legyen
  }
}

void updateSerial()
{
  delay(500);
  
  // Először az Arduino-tól olvasott adatokat továbbítjuk a SIM800L-nek
  while (Serial.available()) 
  {
    char dataFromSerial = Serial.read(); // Az Arduino-tól kapott adat tárolása
    mySerial.write(dataFromSerial);      // Továbbítjuk a SIM800L-nek
  }

  // Most a SIM800L-től kapott adatokat olvassuk
  while (mySerial.available()) 
  {
    char dataFromSIM800L = mySerial.read(); // Az SIM800L-től kapott adat tárolása
    Serial.write(dataFromSIM800L);          // Kiírjuk a soros monitorba

    // Karakterenként építjük fel a buffer változóban az adatokat
    buffer += dataFromSIM800L;
    Serial.println("buffer= " + buffer);

    // Ellenőrizzük, hogy a "+CMT:" benne van-e a bufferben
    if (buffer.indexOf("+CMT:") != -1) {
      buffer = "";          // Töröljük a buffer tartalmát, hogy a következő sorra kész legyen
      smsStart = true;      // Az SMS szövegének beolvasása kezdődik a következő sorban
    }

    // Ha már az SMS beérkezését jelezte a SIM800L, beolvassuk az SMS szövegét
    if (smsStart && dataFromSIM800L == '\n') {
      smsStart = false;     // Megkaptuk az SMS végét
      incomingSMS = buffer; // Az SMS szövegét eltároljuk az incomingSMS változóban
      buffer = "";          // Töröljük a buffer tartalmát
    }
  }
}


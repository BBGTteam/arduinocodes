#include <ArduinoJson.h>
//#include <MQ135.h>
#include <DHT.h>
#include "pitches.h"
#include "ACS712.h"

//FREE PINS
/*
Tomi: 
28-kék
22-fehér mot
Bazsi:
23-fehér mot
CP:
36-zöld
32-fehér
Gábor:
35-szüke
39-fehér mot
*/

#define PIN_MQ135_TOMI A0 //piros
#define PIN_MQ135_BAZSI A1 //piros
#define PIN_MQ135_02 A2 //piros

#define DHT_TOMI 26 //zöld
#define DHTPIN_BAZSI 27 //zöld
#define DHTPIN_CP 38 //kék
#define DHTPIN_GABI 37 //zöld
#define DHTPIN_PAD 50 //kék
#define DHTPIN_OUT 48 //zöld
#define DHTPIN_BATH 52 //zöldCsíkos

#define DHTTYPE DHT11   // DHT 11

#define MOTION_CP 40 //sárga

/*
//MQ135
MQ135 mq135_sensor_Tomi(PIN_MQ135_TOMI);
MQ135 mq135_sensor_Bazsi(PIN_MQ135_BAZSI);
MQ135 mq135_sensor_02(PIN_MQ135_02);
*/

DHT dht_Tomi(DHT_TOMI, DHTTYPE); 
DHT dht_Bazsi(DHTPIN_BAZSI, DHTTYPE);
DHT dht_Cp(DHTPIN_CP, DHTTYPE);
DHT dht_Gabi(DHTPIN_GABI, DHTTYPE);
DHT dht_Pad(DHTPIN_PAD, DHTTYPE);
DHT dht_Out(DHTPIN_OUT, DHTTYPE);
DHT dht_Bath(DHTPIN_BATH, DHTTYPE);

//all relays plug in to NO not NC!
#define relay_Tomi_Heat 46 //fehér
#define relay_Tomi_Led 30 //sárga
boolean relay_State_Tomi_Heat = true;
boolean relay_State_Tomi_Led = true;

#define relay_Bazsi_Heat 44 //fehér
boolean relay_State_Bazsi_Heat = true;
#define relay_Bazsi_Led 31 //sárga
boolean relay_State_Bazsi_Led = true;


#define relay_Gabi_Heat 42 //kék
boolean relay_State_Gabi_Heat = true;
boolean relay_State_Gabi_Led = true;
#define GABI_RED_PIN 43 //piros
#define GABI_GREEN_PIN 41 //sárga
#define GABI_BLUE_PIN 33 //kék
int GabiRedState = 0;
int GabiGreenState = 0;
int GabiBlueState = 0;

#define relay_Cp_Heat 29 //kék
boolean relay_State_Cp_Heat = true;
#define relay_Cp_Led 34 //szürke
boolean relay_State_Cp_Led = true;
boolean motion_State_Cp = false;

//DHT 11
float temperature_Tomi, humidity_Tomi;
float temperature_Bazsi, humidity_Bazsi;
float temperature_Gabi, humidity_Gabi;
float temperature_Pad, humidity_Pad;
float temperature_Out, humidity_Out;
float temperature_Bath, humidity_Bath;


//piezos
#define piezo_Tomi 24 //szürke
#define piezo_Bazsi 25 //szürke

String message = "";

boolean serverState = true;
int messageMissNumber = 0;

ACS712  ACS_BAZSI(A4, 5.0, 1023, 61);
ACS712  ACS_TOMI(A5, 5.0, 1023, 61);
ACS712  ACS_GABI(A3, 5.0, 1023, 61);

int corrBazsi = -550;
int corrTomi = -390;
int corrGabi = -440;

unsigned long currentMillis = millis();
unsigned long previousMillisSendMessage = 0;
unsigned long previousMillisMessageMiss= 0;

const long interval = 1000;
boolean sendMessage = true;

int amperTomi = 0;
int amperBazsi = 0;
int amperGabi = 0;
float humidity_Cp = 0;
float temperature_Cp = 0;

void setup() {
   
  Serial.begin(115200);
  Serial.setTimeout(10);

  dht_Tomi.begin();
  dht_Bazsi.begin();
  dht_Cp.begin();
  dht_Gabi.begin();
  dht_Pad.begin();
  dht_Out.begin();
  dht_Bath.begin();

  pinMode(relay_Tomi_Heat , OUTPUT);
  digitalWrite(relay_Tomi_Heat , relay_State_Tomi_Heat);
  pinMode(relay_Tomi_Led , OUTPUT);
  digitalWrite(relay_Tomi_Led , relay_State_Tomi_Led);

  pinMode(relay_Bazsi_Heat , OUTPUT);
  digitalWrite(relay_Bazsi_Heat , relay_State_Bazsi_Heat);
  pinMode(relay_Bazsi_Led , OUTPUT);
  digitalWrite(relay_Bazsi_Led , relay_State_Bazsi_Led);

  pinMode(relay_Gabi_Heat , OUTPUT);
  digitalWrite(relay_Gabi_Heat , relay_State_Gabi_Heat);
  
  pinMode(relay_Cp_Heat , OUTPUT);
  digitalWrite(relay_Cp_Heat , relay_State_Cp_Heat);
  pinMode(relay_Cp_Led , OUTPUT);
  digitalWrite(relay_Cp_Led , relay_State_Cp_Led);
  pinMode(MOTION_CP, INPUT);

  pinMode(GABI_RED_PIN, OUTPUT);
  pinMode(GABI_GREEN_PIN, OUTPUT);
  pinMode(GABI_BLUE_PIN, OUTPUT);

}

void loop() {
  //turn off relays if server off
  if (messageMissNumber > 60){
    serverState = false;
  
  relay_State_Tomi_Heat = true;
  digitalWrite(relay_Tomi_Heat , relay_State_Tomi_Heat);
  relay_State_Tomi_Led = true;
  digitalWrite(relay_Tomi_Led , relay_State_Tomi_Led);

  relay_State_Bazsi_Heat = true;
  digitalWrite(relay_Bazsi_Heat , relay_State_Bazsi_Heat);
  relay_State_Bazsi_Led = true;
  digitalWrite(relay_Bazsi_Led , relay_State_Bazsi_Led);

  relay_State_Cp_Heat = true;
  digitalWrite(relay_Cp_Heat , relay_State_Cp_Heat);
  relay_State_Cp_Led = true;
  digitalWrite(relay_Cp_Led , relay_State_Cp_Led);

  relay_State_Gabi_Heat = true;
  digitalWrite(relay_Gabi_Heat , relay_State_Gabi_Heat);
  }
  
  //send JSON
  while ((Serial.available() == 0 && sendMessage) || messageMissNumber > 5) {
    
    currentMillis = millis();
    
    if (currentMillis - previousMillisSendMessage >= 900) {
      previousMillisSendMessage = currentMillis;

      StaticJsonDocument<350> doc;
      
    //Tomi Room
    humidity_Tomi = dht_Tomi.readHumidity();
    temperature_Tomi = dht_Tomi.readTemperature();
    /*
    float rzero_Tomi = mq135_sensor_Tomi.getRZero();
    float correctedRZero_Tomi = mq135_sensor_Tomi.getCorrectedRZero(temperature_Tomi, humidity_Tomi);
    float resistance_Tomi = mq135_sensor_Tomi.getResistance();
    float ppm_Tomi = mq135_sensor_Tomi.getPPM();
    float correctedPPM_Tomi = mq135_sensor_Tomi.getCorrectedPPM(temperature_Tomi, humidity_Tomi);
    */
    amperTomi = getAmper(ACS_TOMI, corrTomi);
    JsonObject Tomi = doc.createNestedObject("Tomi");
      Tomi["temperature_Tomi"] = temperature_Tomi;
      Tomi["humidity_Tomi"] = humidity_Tomi;
      //Tomi["correctedPPM_Tomi"] = correctedPPM_Tomi;
      Tomi["relay_State_Tomi_Heat"] = relay_State_Tomi_Heat;
      Tomi["relay_State_Tomi_Led"] = relay_State_Tomi_Led;
      Tomi["amperTomi"] = amperTomi;
      
    //Bazsi Room
    humidity_Bazsi = dht_Bazsi.readHumidity();
    temperature_Bazsi = dht_Bazsi.readTemperature();
    amperBazsi = getAmper(ACS_BAZSI, corrBazsi);
    
    JsonObject Bazsi = doc.createNestedObject("Bazsi");
      Bazsi["temperature_Bazsi"] = temperature_Bazsi;
      Bazsi["humidity_Bazsi"] = humidity_Bazsi;
      Bazsi["relay_State_Bazsi_Heat"] = relay_State_Bazsi_Heat;
      Bazsi["relay_State_Bazsi_Led"] = relay_State_Bazsi_Led;
      Bazsi["amperBazsi"] = amperBazsi;
      
    //Common place
    humidity_Cp = dht_Cp.readHumidity();
    temperature_Cp = dht_Cp.readTemperature();
    motion_State_Cp = digitalRead(MOTION_CP);

    JsonObject Cp = doc.createNestedObject("Cp");
      Cp["temperature_Cp"] = temperature_Cp;
      Cp["humidity_Cp"] = humidity_Cp;
      Cp["relay_State_Cp_Heat"] = relay_State_Cp_Heat;
      Cp["relay_State_Cp_Led"] = relay_State_Cp_Led;
      Cp["motion_State_Cp"] = motion_State_Cp;
  
    //Gabi Room
    humidity_Gabi = dht_Gabi.readHumidity();
    temperature_Gabi = dht_Gabi.readTemperature();
    /*
    float rzero_02 = mq135_sensor_02.getRZero();
    float correctedRZero_02 = mq135_sensor_02.getCorrectedRZero(temperature_Gabi, humidity_Gabi);
    float resistance_02 = mq135_sensor_02.getResistance();
    float ppm_02 = mq135_sensor_02.getPPM();
    float correctedPPM_02 = mq135_sensor_02.getCorrectedPPM(temperature_Gabi, humidity_Gabi);
    */
    amperGabi = getAmper(ACS_GABI, corrGabi);
 
    JsonObject Gabi = doc.createNestedObject("Gabi");
      Gabi["temperature_Gabi"] = temperature_Gabi;
      Gabi["humidity_Gabi"] = humidity_Gabi;
      Gabi["relay_State_Gabi_Heat"] = relay_State_Gabi_Heat;
      Gabi["amperGabi"] = amperGabi;
      Gabi["GabiRedState"] = GabiRedState;
      Gabi["GabiGreenState"] = GabiGreenState;
      Gabi["GabiBlueState"] = GabiBlueState;
      
    //Pad
    humidity_Pad = dht_Pad.readHumidity();
    temperature_Pad = dht_Pad.readTemperature();
 
    JsonObject Pad = doc.createNestedObject("Pad");
      Pad["temperature_Pad"] = temperature_Pad;
      Pad["humidity_Pad"] = humidity_Pad;
    
    //Out
    humidity_Out = dht_Out.readHumidity();
    temperature_Out = dht_Out.readTemperature();
    
    JsonObject Out = doc.createNestedObject("Out");
      Out["temperature_Out"] = temperature_Out;
      Out["humidity_Out"] = humidity_Out;
    
    //Bath Room
    humidity_Bath = dht_Bath.readHumidity();
    temperature_Bath = dht_Bath.readTemperature();
    
    JsonObject Bath = doc.createNestedObject("Bath");
      Bath["temperature_Bath"] = temperature_Bath;
      Bath["humidity_Bath"] = humidity_Bath;
      
    if (sendMessage){
      serializeJson(doc, Serial);
      Serial.println();
    }
    //Serial.print("memorySendMessage: ");
    //Serial.println(doc.memoryUsage());
    doc.clear();
    Serial.flush();
    sendMessage = false;
    }
  }
  
  //get message 
  while (Serial.available() > 0 && !sendMessage) {  

    message  = Serial.readStringUntil('>');
    //Serial.print("Message: ");
    //Serial.println(message);
  
    if(message.startsWith("{") && message.endsWith("}")){
      messageMissNumber = 0;
      serverState = true;
      
      StaticJsonDocument<400> orders;
      DeserializationError error = deserializeJson(orders, message);

      // Test if parsing succeeds.
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      
      //Serial.print("memoryGetMessage: ");
      //Serial.println(orders.memoryUsage());

      if (relay_State_Tomi_Heat != orders["TomiHeat"]){
        relay_State_Tomi_Heat = orders["TomiHeat"];
        digitalWrite(relay_Tomi_Heat, relay_State_Tomi_Heat);
      }

      if (relay_State_Tomi_Led != orders["TomiLed"]){
        relay_State_Tomi_Led = orders["TomiLed"];
        digitalWrite(relay_Tomi_Led, relay_State_Tomi_Led);
      }

      if (relay_State_Bazsi_Heat != orders["BazsiHeat"]){
        relay_State_Bazsi_Heat = orders["BazsiHeat"];
        digitalWrite(relay_Bazsi_Heat, relay_State_Bazsi_Heat);
      }
  
      if (relay_State_Gabi_Heat != orders["GabiHeat"]){
        relay_State_Gabi_Heat = orders["GabiHeat"];
        digitalWrite(relay_Gabi_Heat, relay_State_Gabi_Heat);
      }
      
      GabiRedState = orders["GabiRedState"];
      analogWrite(GABI_RED_PIN, GabiRedState);

      GabiGreenState = orders["GabiGreenState"];
      analogWrite(GABI_GREEN_PIN, GabiGreenState);
      
      GabiBlueState = orders["GabiBlueState"];
      analogWrite(GABI_BLUE_PIN, GabiBlueState);
/*
      if (GabiRedState != orders["GabiRedState"]){
      }
      if (GabiGreenState != orders["GabiGreenState"]){
      }
      if (GabiBlueState != orders["GabiBlueState"]){
      }
*/

      if (relay_State_Cp_Led != orders["CpLed"]){
        relay_State_Cp_Led = orders["CpLed"];
        digitalWrite(relay_Cp_Led, relay_State_Cp_Led);
      }

      orders.clear();
      sendMessage = true;
    }

/*

  if (message.endsWith("alertTomi")){
 //   playAlert(piezo_Tomi);
  }

  if (message.endsWith("ledBazsi")){
    message.replace("ledBazsi", "");
    relay_State_Bazsi_Led = message.toInt();
    digitalWrite(relay_Bazsi_Led, relay_State_Bazsi_Led);
  }
  
  if (message.endsWith("alertBazsi")){
 //   playAlert(piezo_Bazsi);
  }

  if (message.endsWith("RGB")){
      message.replace("RGB", "");
      int fadespeed = message.toInt();
      rgb(fadespeed);
  }
*/
    else {
      Serial.print("Message: ");
      Serial.println(message);
    }
  }

  //message missing number
  currentMillis = millis();
  if (currentMillis - previousMillisMessageMiss>= interval) {
    previousMillisMessageMiss= currentMillis;
    messageMissNumber++;
  }  
  
}
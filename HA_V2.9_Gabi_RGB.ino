#include <MQ135.h>
#include <DHT.h>
#include "pitches.h"
#include "ACS712.h"

#define PIN_MQ135_TOMI A0
#define PIN_MQ135_BAZSI A1
#define PIN_MQ135_02 A2

#define DHT_TOMI 26
#define DHTPIN_BAZSI 27
#define DHTPIN_CP 38
#define DHTPIN_GABI 37
#define DHTPIN_PAD 50
#define DHTPIN_OUT 48
#define DHTPIN_BATH 52

#define DHTTYPE DHT11   // DHT 11

#define MOTION_CP 40

//MQ135
MQ135 mq135_sensor_Tomi(PIN_MQ135_TOMI);
MQ135 mq135_sensor_Bazsi(PIN_MQ135_BAZSI);
MQ135 mq135_sensor_02(PIN_MQ135_02);

DHT dht_Tomi(DHT_TOMI, DHTTYPE);
DHT dht_Bazsi(DHTPIN_BAZSI, DHTTYPE);
DHT dht_Cp(DHTPIN_CP, DHTTYPE);
DHT dht_Gabi(DHTPIN_GABI, DHTTYPE);
DHT dht_Pad(DHTPIN_PAD, DHTTYPE);
DHT dht_Out(DHTPIN_OUT, DHTTYPE);
DHT dht_Bath(DHTPIN_BATH, DHTTYPE);

//all relays plug in to NO not NC!
int relay_Tomi_Heat = 46;
int relay_Tomi_Led = 30;
int relay_State_Tomi_Heat = HIGH;
int relay_State_Tomi_Led = HIGH;

int relay_Bazsi_Heat = 44;
int relay_State_Bazsi_Heat = HIGH;
int relay_Bazsi_Led = 31;
int relay_State_Bazsi_Led = HIGH;


int relay_Gabi_Heat = 42;
int relay_State_Gabi_Heat = HIGH;
//int relay_Gabi_Led = 43;
int relay_State_Gabi_Led = HIGH;
int REDPIN = 43;
int GREENPIN = 41;
int BLUEPIN = 33;

int relay_Cp_Heat = 29;
int relay_State_Cp_Heat = HIGH;
int relay_Cp_Led = 34;
int relay_State_Cp_Led = HIGH;
int motion_State_Cp = LOW;

//DHT 11
float temperature_Tomi, humidity_Tomi;
float temperature_Bazsi, humidity_Bazsi;
//float temperature_Cp, humidity_Cp;
float temperature_Gabi, humidity_Gabi;
float temperature_Pad, humidity_Pad;
float temperature_Out, humidity_Out;
float temperature_Bath, humidity_Bath;


//piezos
int piezo_Tomi = 24;
int piezo_Bazsi = 25;

String message = "";

boolean serverState = true;
int messageMissNumber = 0;

ACS712  ACS_BAZSI(A4, 5.0, 1023, 61);
ACS712  ACS_TOMI(A5, 5.0, 1023, 61);
ACS712  ACS_GABI(A3, 5.0, 1023, 61);

int corrBazsi = -550;
int corrTomi = -390;
int corrGabi = -440;

unsigned long previousMillis = 0;

const long interval = 1000;

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
  //pinMode(relay_Gabi_Led , OUTPUT);
  //digitalWrite(relay_Gabi_Led , relay_State_Gabi_Led);
  
  pinMode(relay_Cp_Heat , OUTPUT);
  digitalWrite(relay_Cp_Heat , relay_State_Cp_Heat);
  pinMode(relay_Cp_Led , OUTPUT);
  digitalWrite(relay_Cp_Led , relay_State_Cp_Led);
  pinMode(MOTION_CP, INPUT);

  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);

  
}

void loop() {
  
  while (Serial.available() == 0) {
    
    unsigned long currentMillis = millis();
  
  
    //turn off relays if server off
      if (messageMissNumber > 60){
        serverState = false;
      
      relay_State_Tomi_Heat = HIGH;
      digitalWrite(relay_Tomi_Heat , relay_State_Tomi_Heat);
      relay_State_Tomi_Led = HIGH;
      digitalWrite(relay_Tomi_Led , relay_State_Tomi_Led);
  
      relay_State_Bazsi_Heat = HIGH;
      digitalWrite(relay_Bazsi_Heat , relay_State_Bazsi_Heat);
      relay_State_Bazsi_Led = HIGH;
      digitalWrite(relay_Bazsi_Led , relay_State_Bazsi_Led);
  
      relay_State_Cp_Heat = HIGH;
      digitalWrite(relay_Cp_Heat , relay_State_Cp_Heat);
      relay_State_Cp_Led = HIGH;
      digitalWrite(relay_Cp_Led , relay_State_Cp_Led);
  
      relay_State_Gabi_Heat = HIGH;
      digitalWrite(relay_Gabi_Heat , relay_State_Gabi_Heat);
      //relay_State_Gabi_Led = HIGH;
      //digitalWrite(relay_Gabi_Led , relay_State_Gabi_Led);
    }
    
    //Tomi Room
  if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      
    humidity_Tomi = dht_Tomi.readHumidity();
    temperature_Tomi = dht_Tomi.readTemperature();
    
    float rzero_Tomi = mq135_sensor_Tomi.getRZero();
    float correctedRZero_Tomi = mq135_sensor_Tomi.getCorrectedRZero(temperature_Tomi, humidity_Tomi);
    float resistance_Tomi = mq135_sensor_Tomi.getResistance();
    float ppm_Tomi = mq135_sensor_Tomi.getPPM();
    float correctedPPM_Tomi = mq135_sensor_Tomi.getCorrectedPPM(temperature_Tomi, humidity_Tomi);
  
    Serial.print("Tomi");
    Serial.print(",");
    Serial.print(temperature_Tomi);
    Serial.print(",");
    Serial.print(humidity_Tomi);
    Serial.print(",");
    Serial.print(correctedPPM_Tomi);
    Serial.print(",");
    Serial.print(relay_State_Tomi_Heat);
    Serial.print(",");
    Serial.print(relay_State_Tomi_Led);
    Serial.print(",");
    getAmper(ACS_TOMI, corrTomi);
    Serial.print(",");
    Serial.println(serverState);
    
    //Bazsi Room
        
    humidity_Bazsi = dht_Bazsi.readHumidity();
    temperature_Bazsi = dht_Bazsi.readTemperature();
    
    Serial.print("Bazsi");
    Serial.print(",");
    Serial.print(temperature_Bazsi);
    Serial.print(",");
    Serial.print(humidity_Bazsi);
    Serial.print(",");
    Serial.print("N.A");
    Serial.print(",");
    Serial.print(relay_State_Bazsi_Heat);
    Serial.print(",");
    Serial.print(relay_State_Bazsi_Led);
    Serial.print(",");
    getAmper(ACS_BAZSI, corrBazsi);
    Serial.print(",");
    Serial.println(serverState);

    //Common place
      
    float humidity_Cp = dht_Cp.readHumidity();
    float temperature_Cp = dht_Cp.readTemperature();
  
    motion_State_Cp = digitalRead(MOTION_CP);
    
    Serial.print("Cp");
    Serial.print(",");
    Serial.print(temperature_Cp);
    Serial.print(",");
    Serial.print(humidity_Cp);
    Serial.print(",");
    Serial.print("N.A");
    Serial.print(",");
    Serial.print(relay_State_Cp_Heat);
    Serial.print(",");
    Serial.print(relay_State_Cp_Led);
    Serial.print(",");
    Serial.print(motion_State_Cp);
    Serial.print(",");
    Serial.println(serverState);
  
    //Gabi Room
    humidity_Gabi = dht_Gabi.readHumidity();
    temperature_Gabi = dht_Gabi.readTemperature();
    
    float rzero_02 = mq135_sensor_02.getRZero();
    float correctedRZero_02 = mq135_sensor_02.getCorrectedRZero(temperature_Gabi, humidity_Gabi);
    float resistance_02 = mq135_sensor_02.getResistance();
    float ppm_02 = mq135_sensor_02.getPPM();
    float correctedPPM_02 = mq135_sensor_02.getCorrectedPPM(temperature_Gabi, humidity_Gabi);
    
    Serial.print("Gabi");
    Serial.print(",");
    Serial.print(temperature_Gabi);
    Serial.print(",");
    Serial.print(humidity_Gabi);
    Serial.print(",");
    Serial.print("N.A");
    Serial.print(",");
    Serial.print(relay_State_Gabi_Heat);
    Serial.print(",");
    Serial.print(relay_State_Gabi_Led);
    Serial.print(",");
    getAmper(ACS_GABI, corrGabi);
    Serial.print(",");
    Serial.println(serverState);

    //Pad
    humidity_Pad = dht_Pad.readHumidity();
    temperature_Pad = dht_Pad.readTemperature();
    
    Serial.print("Pad");
    Serial.print(",");
    Serial.print(temperature_Pad);
    Serial.print(",");
    Serial.println(humidity_Pad);
    
    //Out
    humidity_Out = dht_Out.readHumidity();
    temperature_Out = dht_Out.readTemperature();
    
    Serial.print("Out");
    Serial.print(",");
    Serial.print(temperature_Out);
    Serial.print(",");
    Serial.println(humidity_Out);
    
    //Bath Room
    humidity_Bath = dht_Bath.readHumidity();
    temperature_Bath = dht_Bath.readTemperature();
    
    Serial.print("Bath");
    Serial.print(",");
    Serial.print(temperature_Bath);
    Serial.print(",");
    Serial.println(humidity_Bath);
    
  
    messageMissNumber++;
      
  }
  }
  
  //get message 
  message  = Serial.readString();
  serverState = true;
  messageMissNumber = 0;
  
    if (message.endsWith("heatTomi")){
    message.replace("heatTomi", "");
    relay_State_Tomi_Heat = message.toInt();
    digitalWrite(relay_Tomi_Heat, relay_State_Tomi_Heat);
  }

      if (message.endsWith("ledTomi")){
    message.replace("ledTomi", "");
    relay_State_Tomi_Led = message.toInt();
    digitalWrite(relay_Tomi_Led, relay_State_Tomi_Led);
  }

  if (message.endsWith("alertTomi")){
 //   playAlert(piezo_Tomi);
  }

  if (message.endsWith("ledBazsi")){
    message.replace("ledBazsi", "");
    relay_State_Bazsi_Led = message.toInt();
    digitalWrite(relay_Bazsi_Led, relay_State_Bazsi_Led);
  }

    if (message.endsWith("heatBazsi")){
    message.replace("heatBazsi", "");
    relay_State_Bazsi_Heat = message.toInt();
    digitalWrite(relay_Bazsi_Heat, relay_State_Bazsi_Heat);
  }
  
  if (message.endsWith("alertBazsi")){
 //   playAlert(piezo_Bazsi);
  }

  if (message.endsWith("heatGabi")){
    message.replace("heatGabi", "");
    relay_State_Gabi_Heat = message.toInt();
    digitalWrite(relay_Gabi_Heat, relay_State_Gabi_Heat);
  }

    if (message.endsWith("ledCp")){
    message.replace("ledCp", "");
    relay_State_Cp_Led = message.toInt();
    digitalWrite(relay_Cp_Led, relay_State_Cp_Led);
  }

  if (message.endsWith("RGB")){
      message.replace("RGB", "");
      int fadespeed = message.toInt();
      rgb(fadespeed);
  }
  
  if (message.endsWith("RGB_R")){
      message.replace("RGB_R", "");
      analogWrite(REDPIN, message.toInt());
  }
  
   if (message.endsWith("RGB_G")){
      message.replace("RGB_G", "");
      analogWrite(GREENPIN, message.toInt());
  }
  
   if (message.endsWith("RGB_B")){
      message.replace("RGB_B", "");
      analogWrite(BLUEPIN, message.toInt());
  }
}

#include <Arduino.h>
#include <ESP8266WiFi.h> // Importa a Biblioteca ESP8266WiFi
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient


#define LAMP_PIN   14


//Change to SSID network
const char* SSID = ""; 
//Change to PASSWORD
const char* PASSWORD = ""; 
  

const char* BROKER_MQTT = ""; 
const char* USER_MQTT = "";
const char* PASS_MQTT = "";

int BROKER_PORT = 1883; 
 
WiFiClient espClient; 
PubSubClient MQTT(espClient); 

char bufferConvertIntToChar[32];

int currentValueLamp = 500;
  
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{

  String msg;
  for (int i=0; i< length; i++) {
    char c = (char) payload[i];
    msg += c;
  }
  
 if (msg.equals("ON")) {
    analogWrite(LAMP_PIN, currentValueLamp);
 } else if(msg.equals("OFF")) {
    analogWrite(LAMP_PIN, 0);
 } else {
    int v = msg.toInt();
    currentValueLamp = v * 100;
    if(v > 1000)  {
      analogWrite(LAMP_PIN, 1023); 
    } else {
      analogWrite(LAMP_PIN, currentValueLamp);
    }  
 }

  Serial.println(msg);
}

char* getChipId() 
{
    uint32_t chipId = ESP.getChipId();
    itoa(chipId, bufferConvertIntToChar, 10);
    return bufferConvertIntToChar;
}


void reconnectMQTT() 
{
    while (!MQTT.connected()) 
    {
        if (MQTT.connect(getChipId())) 
        {
            MQTT.publish("status", "{\"online\": \"true\"}");
        } 
        else
        {
          Serial.print( "....");
            delay(2000);
        }
    }

     MQTT.subscribe("lamp/COMMAND/");
}

void initMQTT() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   
    MQTT.setCallback(mqtt_callback);        
}

void reconectWiFi() 
{
    if (WiFi.status() == WL_CONNECTED) return;
         
    WiFi.begin(SSID, PASSWORD); 
     
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.println("Connect");
    }
}

void initWiFi() 
{
    delay(10);
    reconectWiFi();
}

void setup() {
  Serial.begin(9600);
  analogWrite(LAMP_PIN, 512);  
  initWiFi();
  initMQTT();
}

void loop() 
{   
    if (!MQTT.connected()) reconnectMQTT();     
    else reconectWiFi();

    MQTT.loop();
}

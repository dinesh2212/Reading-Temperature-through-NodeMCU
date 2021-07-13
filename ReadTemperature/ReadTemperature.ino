//Setting up the libraries
#include<ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
//#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

//Setting up the DHT11 sensor
#define DHTTYPE DHT11
#define DHTPIN D4

//Setting up the WiFi credentials
const char* ssid = "";
const char* password = "";

#define CHAT_ID ""

// Initialize Telegram BOT
#define BOTtoken ""  // your Bot Token (Get from Botfather)

const uint8_t fingerprint[20] = {0xF2, 0xAD, 0x29, 0x9C, 0x34, 0x48, 0xDD, 0x8D, 0xF4, 0xCF, 0x52, 0x32, 0xF6, 0x57, 0x33, 0x68, 0x2E, 0x81, 0xC1, 0x90};
ESP8266WiFiMulti WiFiMulti;
//UniversalTelegramBot bot(BOTtoken, client);

DHT dht(DHTPIN,DHTTYPE);

void setup() {
    Serial.begin(115200);
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP(ssid, password);
    dht.begin();
}

//Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan = 0;


bool resetOffset(String offset){
    if ((WiFiMulti.run() == WL_CONNECTED)){
        std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
        client->setFingerprint(fingerprint);
        HTTPClient https;
        String url = "Enter_API_URL" + offset; //Enter the URL for API request
        if (https.begin(*client, url)){
            //Serial.println(url);
            int httpCode = https.GET();
            String data = https.getString();
            DynamicJsonDocument doc(5000);
            deserializeJson(doc, data);
            bool response = doc["ok"];
            return response;
        }
        Serial.printf("[HTTPS] Unable to connect\n");
        return false;
    }
}

String *getUpdates(){
    static String update[10]; 
    for(int i = 0; i < 10; i++){
      update[i] = "";
    }
    if ((WiFiMulti.run() == WL_CONNECTED)){
        Serial.println("Entered");
        std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
        client->setFingerprint(fingerprint);
        HTTPClient https;
        String url = "Enter_API_URL"; //Enter the URL for API request
        if (https.begin(*client, url)){
            int httpCode = https.GET();
            String data = https.getString();
            DynamicJsonDocument doc(5000);
            deserializeJson(doc, data);
            Serial.println(data);
            if(doc["result"].size()) {
                int offset = doc["result"][doc["result"].size() - 1]["update_id"];
                //String output;
                //serializeJson(doc["result"][doc["result"].size() - 1]["message"]["chat"]["id"], output);
                //Serial.println(output);
                String message = doc["result"][doc["result"].size() - 1]["message"]["text"];
                String first_name = doc["result"][doc["result"].size() - 1]["message"]["from"]["first_name"];
                update[0] = String(1);
                update[1] = String(offset + 1);
                //Serial.println(chat_id);
                update[2] = "noice";
                update[3] = message;
                update[4] = first_name;
            }

        } else {
            Serial.printf("[HTTPS] Unable to connect\n");
        }
        return update;
    }
}

bool sendMessage(String message, String chat_id){
    if ((WiFiMulti.run() == WL_CONNECTED)){
        std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
        client->setFingerprint(fingerprint);
        HTTPClient https;
        String url = "Enter_API_URL";  //Enter the URL for API request
        if (https.begin(*client, url)){
            //Serial.println(url);
            https.addHeader("Content-Type", "application/json");
            String body = "{\"text\": \"" + message + "\", \"chat_id\": \"" + chat_id +"\"}";
            int httpCode = https.POST(body);
            String data = https.getString();
            DynamicJsonDocument doc(5000);
            Serial.println(data);
            deserializeJson(doc, data);
            bool response = doc["ok"];
            return response;
        }
        Serial.printf("[HTTPS] Unable to connect\n");
        return false;
    }
}

String getTemp() {              //function which return temp string
  String message;
  float temp;
  temp = dht.readTemperature();
  message = "Temperature: " + String(temp) + "°C";
  return message;
}

void handleNewMessages(String text, String chat_id, String from){         
  Serial.println("Entering HandleNewMessages ... ");
    if(text == "/start") {
      String greet = "Hello, " + from + ":)" + "\n";
      greet += "Use the following commands and get started ... \n\n";
      greet += "/temp    ===>  To instantly get the temperature \n";
      sendMessage(chat_id, greet);
    }
    else if (text == "/temp") {
      String temp = getTemp();
      sendMessage(chat_id, temp);
    }
    else {
      sendMessage(chat_id, "kindly send a valid message.", );
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() > lastTimeBotRan + botRequestDelay) {
    String *latestUpdate = getUpdates();
    Serial.println(latestUpdate[0]); // success    (bool)
    if(latestUpdate[0] != "") {
      Serial.println(latestUpdate[1]); // offset     (int)
      Serial.println(latestUpdate[2]); // chat_id    (string)
      Serial.println(latestUpdate[3]); // message    (string)
      Serial.println(latestUpdate[4]); // first_name (string)
      Serial.println();
      Serial.println();
      if(latestUpdate[1]){
        bool resetStatus = resetOffset(latestUpdate[1]); 
        if(resetStatus){ 
            Serial.println("Succesfully reset !! "); 
        }else{ 
            Serial.println("Unsuccessful reset !! "); 
        }
      }
    }
    lastTimeBotRan = millis();
  }
}

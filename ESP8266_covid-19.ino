#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include<Wire.h>

ESP8266WiFiMulti WiFiMulti;
HTTPClient https;
std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
String stringa;
char str[1500];
char str_last_send[32];
char charBuf[90];
String ape;
#define ARRAYSIZE 5
String links[ARRAYSIZE] = { "https://coronavirus-19-api.herokuapp.com/countries/italy",
                            "https://coronavirus-19-api.herokuapp.com/countries/usa",
                            "https://coronavirus-19-api.herokuapp.com/countries/spain",
                            "https://coronavirus-19-api.herokuapp.com/countries/uk",
                            "https://coronavirus-19-api.herokuapp.com/countries/france",
                          };
String results[ARRAYSIZE];
unsigned long previousTime = 2000000;
int httpCode;
int p = 0;
long interval = 1800000; //30 minutes

void setup() {


  Serial.begin(9600);
  Wire.begin();
  //Serial.setDebugOutput(true);

  //for (uint8_t t = 4; t > 0; t--) {
  //Serial.printf("[SETUP] WAIT %d...\n", t);
  //Serial.flush();
  //delay(1000);
  // }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("aaaa", "www");

  client->setInsecure();
  Wire.begin();
}

void loop() {

  unsigned long currentTime = millis();

  if (WiFiMulti.run() == WL_CONNECTED) {


    if (abs(currentTime - previousTime) > interval)
    {
      previousTime = currentTime;

      for (int i = 0; i < ARRAYSIZE ; i++)
      {
        https.begin(*client, links[i]);
        httpCode = https.GET();

        if (httpCode > 0) {
          results[i] = https.getString();
          delay(10);
        }

        https.end();
      }
    }

    for (int i = 0; i < ARRAYSIZE ; i++)
    {
      Serial.println(results[i]);


    }

    if (p == 0)
    {
      stringa = 'æ' + results[0] + ',' + results[1] + ',' + results[2] + ',' + results[3] + ',' + results[4] + ']' + '$';
      delay(1000);
      stringa.toCharArray(str, stringa.length() + 1);

      int beginIndex = 0;
      char str_send[32];
      int cy = stringa.length() / 32;
      int rest = stringa.length() % 32;

      
      for (int i = 0; i < cy; i++)
      {
        Wire.beginTransmission(4); // transmit to device #4
        strncpy(str_send, str + beginIndex, 32);

        Wire.write(str_send, 32);
        Wire.endTransmission();    // stop transmitting
        beginIndex += 32;
        Wire.flush();
        delay(1000);
      }

      Wire.beginTransmission(4); // transmit to device #4
      strncpy(str_last_send, str + beginIndex, rest);

      Wire.write(str_last_send, 32);
      Wire.endTransmission();    // stop transmitting
//
//
    delay(100000);
    }

    
  }

    //Serial.println("Wait 10s before next round...");
    delay(10000);
  }

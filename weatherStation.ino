#include "DHTesp.h"
//#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti WiFiMulti;
DHTesp dht;
float humidity;
float temperature;
float heatIndex;
char url[500];

void setup() 
{
    Serial.begin(115200);
    //Serial.setDebugOutput(true);

    Serial.println();
    Serial.println();
    Serial.println();

    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP("Name", "Pass");

    dht.setup(5); //DC01 = PIN 5
}

void loop() 
{
    delay(dht.getMinimumSamplingPeriod());

    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
    heatIndex = dht.computeHeatIndex(temperature, humidity, false);
  
    if((WiFiMulti.run() == WL_CONNECTED)) 
    {
        HTTPClient http;

        Serial.print("[HTTP] begin...\n");
        sprintf(url, "http://domain.com/loadData.php?temperature=%.2f&humidity=%.2f&heatIndex=%.2f", temperature,humidity,heatIndex);
        Serial.print(url);
        
        http.begin(url);

        Serial.print("[HTTP] GET...\n");
        int httpCode = http.GET();

        if(httpCode > 0) 
        {
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            if(httpCode == HTTP_CODE_OK) 
            {
                String payload = http.getString();
                Serial.println(payload);
            }
        } 
        else 
        {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(60000-dht.getMinimumSamplingPeriod());
}

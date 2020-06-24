#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>

#include "wifi_creds.h"
#define PORT 8266

ESP8266WiFiMulti wifiMulti;
WiFiServer wifiServer(PORT);
WiFiClient client;

void setup()
{
    Serial.begin(9600);
    delay(3000);

    wifiMulti.addAP(SSID, PSK);

    WiFi.softAPdisconnect(true);

    while (wifiMulti.run() != WL_CONNECTED)
        delay(0);

    MDNS.begin("groundstation");
    MDNS.addService("_groundstation", "_tcp", PORT);

    wifiServer.begin();

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
    if (client && client.connected())
    {
        while (client.available())
            Serial.write((char)client.read());

        while (Serial.available())
            client.write((char)Serial.read());
    }
    else
    {
        client = wifiServer.available();
        while(Serial.available()) Serial.read();
    }

    MDNS.update();
}

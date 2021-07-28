#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char *host = "https://www.reddit.com/user/Multihacker007/about.json";

void (*resetFunc)(void) = 0;

int extractKarma(const char *json)
{
    StaticJsonDocument<4096> doc;
    StaticJsonDocument<200> filter;
    filter["data"]["total_karma"] = true;
    deserializeJson(doc, json, DeserializationOption::Filter(filter));
    int karma = doc["data"]["total_karma"];
    return karma;
}

String HTTPSGETRequest(const char *host)
{
    WiFiClientSecure wifi;
    HTTPClient http;
    wifi.setInsecure();
    wifi.connect(host, 443);
    http.begin(wifi, host);
    switch (http.GET())
    {
    case HTTP_CODE_TOO_MANY_REQUESTS:
        delay(5000);
        resetFunc();
        break;
    case HTTP_CODE_OK:
    {
        String payload = http.getString();
        return payload;
        break;
    }
    default:
        resetFunc();
        break;
    }
    http.end();
    String error = "e";
    return error;
}

int getKarma(const char *user)
{
    return extractKarma(HTTPSGETRequest(host).c_str());
}

void setup()
{
    Serial.begin(115200);
    const char *ssid = "YOUR_SSID";
    const char *password = "YOUR_PASSWORD";
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
        delay(200);
}

void loop()
{
    int karma = getKarma(host);
    Serial.println(karma);
    delay(30000);
}
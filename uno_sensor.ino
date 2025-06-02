#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#include <ArduinoJson.h>

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;

void setup() {
  Serial.begin(115200);
  dht.begin();
  Wire.begin();
  lightMeter.begin();
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  int gas = analogRead(A0);
  uint16_t lux = lightMeter.readLightLevel();

  StaticJsonDocument<128> doc;
  doc["temp"]  = t;
  doc["hum"]   = h;
  doc["gas"]   = gas;
  doc["light"] = lux;
  serializeJson(doc, Serial);
  Serial.println();

  delay(5000);
}

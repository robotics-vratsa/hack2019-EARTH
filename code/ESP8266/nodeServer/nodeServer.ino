//include some useful libraries
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//calibrate SEA_LEVEL
#define SEALEVELPRESSURE_HPA (1013.25)

const char* ssid = "wifi_ssif";
const char* password = "wifi_pswd";

String page = "";
float temp = 0;
float pressure = 0;
float alt = 0;
float humi = 0;
float val1 = 0;
float val2 = 0;
float val3 = 0;
float val4 = 0;
float val5 = 0;

//start services
ESP8266WebServer server(1517);
SoftwareSerial s(D6, D5);
Adafruit_BME280 bme;
StaticJsonBuffer<500> jsonBuffer;

void setup() {
  pinMode(D4, OUTPUT);
  digitalWrite(D4, HIGH);
  //setting up communication interfaces
  Serial.begin(115200);
  s.begin(115200);
  WiFi.begin(ssid, password); //begin WiFi connection
  //Serial.println("");
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  //  Serial.println("Web server started!");

  //-----WEB PAGE START-----
  server.on("/", []() {
    digitalWrite(D4, LOW);
    //page = "Temperature: " + String(temp) + "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Pressure: " + String(pressure) + "<br>Aprox. Altitude: " + String(alt) + "&nbsp; Humidity: " + String(humi) + "<br>Dust Density: " + String(val1) + "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;LPG: " + String(val2) + "<br>" + "CO: " + String(val3) + "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Smoke: " + String(val4) + "<br>Voltage: " + String(val5) + "" ;
    page = "{\n  \"temperature\": " + String(temp) + ",\n  \"pressure\": " + String(pressure) + ",\n  \"altitude\": " + String(alt) + ",\n  \"humidity\": " + String(humi) + ",\n  \"dustDensity\": " + String(val1) + ",\n  \"lpg\": " + String(val2) + ",\n  \"co\": " + String(val3) + ",\n  \"smoke\": " + String(val4) + ",\n  \"voltage\": " + String(val5) + "\n}";
    server.send(200, "application/json", page);
    digitalWrite(D4, HIGH);
  });


  //-----WEB PAGE END------

}

void loop() {
  JsonObject& root = jsonBuffer.parseObject(s);
  if (root.success())
  {
    root.printTo(Serial);
    Serial.println("");
    //---extract values-----
    val1 = root["dust"];
    val2 = root["lpg"];
    val3 = root["co"];
    val4 = root["smoke"];
    val5 = root["voltage"];

    //--extract values END---
  }
  jsonBuffer.clear();


  //---extract values-----
  //  val1 = root["dust"];
  //  val2 = root["lpg"];
  //  val3 = root["co"];
  //  val4 = root["smoke"];
  //  val5 = root["voltage"];

  //--extract values END---

  //-----bme280_READ------
  temp = bme.readTemperature();
  pressure = bme.readPressure() / 100.0F;
  alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
  humi = bme.readHumidity();
  //-----bme280_END------
  server.handleClient();
}

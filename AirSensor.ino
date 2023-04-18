#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "DHT.h"
#include "config.h"

//Constants
#define DHTPIN 4 // The pin that the sensor connects to
#define DHTTYPE DHT22 // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor for normal 16mhz Arduino

WiFiUDP udp;
IPAddress broadcast;
uint16_t port;

void setup() {
  Serial.begin(115200);
  dht.begin();
  setupWifi();
}

void loop() {
  //sendHelloMessage();
  //delay(500);

  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));

  sendHelloMessage(f);

  delay(500);
}

void sendHelloMessage(float f) {
  Serial.println("Sending hello message...");
  String message = "Hello";
  char rawMessage[6];

  message.toCharArray(rawMessage, 6);

  udp.beginPacketMulticast(broadcast, port, WiFi.localIP());
  udp.write(rawMessage);
  udp.write(reinterpret_cast <uint8_t *>(&f), sizeof(f));
  udp.endPacket();
  yield();
}

void setupWifi() {
  const char* ssid = WIFI_SSID;
  const char* password = WIFI_PASSWORD;

  IPAddress local_IP;
  IPAddress gateway;
  IPAddress subnet;
  IPAddress primaryDNS;

  local_IP.fromString(IP_ADDRESS);
  gateway.fromString(IP_ADDRESS);
  subnet.fromString(SUBNET);
  primaryDNS.fromString(DNS);

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS)) {
    Serial.println("Wifi failed to configure.");
  }

  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  /* Retry Connection if failed. */
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting to Wifi network failed. Retrying connection...");
  }

  broadcast.fromString(HUB_ADDRESS);
  port = HUB_PORT;

  Serial.println("WiFi successfully connected.");
}

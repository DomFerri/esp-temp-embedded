#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "config.h"

WiFiUDP udp;
IPAddress broadcast;
uint16_t port;

void setup() {
  Serial.begin(115200);
  setupWifi();
}

void loop() {
  sendHelloMessage();
  delay(500);
}

void sendHelloMessage() {
  Serial.println("Sending hello message...");
  String message = "Hello!";
  char rawMessage[6];

  message.toCharArray(rawMessage, 6);
  udp.beginPacketMulticast(broadcast, port, WiFi.localIP());
  udp.write(rawMessage);
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

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include "DHT.h"

#define ANALOGPIN A0
#define DHTPIN 0     // what digital pin we're connected to (D3)
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const char *ssid =  "Test";
const char *pass =  "haslo1234";

const uint16_t port = 4321;
const char *host = "192.168.43.159"; // local server's IP

WiFiUDP client;

void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t))
  {
    Serial.println("\n\nFailed to read from DHT sensor!");
    return;
  }

  Serial.println("");
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %     ");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" °C ");
  Serial.print("    ");

  //Sending data to local server
  //if(!client.read())   //if client is not connected to local server
  //{
      //Serial.println("\n\nConnecting to local server..."); 
      //if(!client.connect(host, port))
      //{
        //Serial.println("\nConnection to local server failed!");
        //Serial.println("Wait for 2 sec...");
        //delay(1000);
      //}
      //else
        //Serial.println("Connetion to local server is successful.");
  //}
  //else  //client is connected
  //{
    char buff1[16];
    char buff2[16];
    memset(buff1, 0, 16);
    dtostrf(h, 15, 2, buff1);
    memset(buff2, 0, 16);
    dtostrf(t, 15, 2, buff2);
    client.beginPacket(host, port);
    client.write(buff1);
    client.write(buff2);
    //client.endPacket();

    //client.beginPacket(host, port);
    //memset(buff, 17, 32);
    //dtostrf(t, 31, 2, buff);
    //client.write(buff, 32);
    client.endPacket();
  //}
}

void setup() 
{
  Serial.begin(9600);
  delay(100);          
  Serial.print("\nConnecting to: ");
  Serial.println(ssid); 

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Connecting to local server..."); 
  client.begin(port);
  //Serial.println(host);
  //if(!client.connect(host, port))
  //{
    //Serial.println("Connection to local server failed!");
    //Serial.println("Wait for 2 sec...");
    //delay(1000);
  //}
  //else
    //Serial.println("Connetion to loacl server is successful.");

  Serial.println("\nDHT11");

  dht.begin();
}
 
void loop() 
{
  sendSensor();
  delay(1000);
}

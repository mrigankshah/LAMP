#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <BH1750.h>
#include <Wire.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino 
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <WiFiClientSecure.h>

/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
static const int RXPin = 0, TXPin = 2;
static const uint32_t GPSBaud = 9600;

//Light sensor
BH1750 lightMeter(0x23);
float latitude , longitude, lux;
String latlong, lat_str, lng_str, speedkmph;
int sats;

//Button
int button = 12;
int button2 = 14;
int buttonpress, buttonpress2 = 0;
volatile bool potholeFlag, streetlightFlag = 0;

//Device ID
String DevID = "RS002";
String tim  = "";
String url = "";
String issue;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

//Google Server Setup Section
const char* host = "script.google.com";

const int httpsPort = 443;
WiFiClientSecure client;
const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";
String GAS_ID = "AKfycbysWuYi2TWR59n8338W5HUXsEWDuRHKY-_cB3P0LKby6blFZ-nImt6TG3BWP9XzKcQ";  // Replace by your Google App S service id
//Set up to send to GSheets
String readString;

void ICACHE_RAM_ATTR potholeButtonClick() {
  potholeFlag = true;
//  buttonpress = digitalRead (button);
  Serial.println("pothole interrupt was triggered");
}

void ICACHE_RAM_ATTR streetlightButtonClick() {
  streetlightFlag = true;
//  buttonpress2 = digitalRead (button2);
    Serial.println("streetlight interrupt was triggered");

}

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);

  pinMode(button, INPUT);
  pinMode(button2, INPUT);

  attachInterrupt(digitalPinToInterrupt(button), potholeButtonClick, RISING);
  attachInterrupt(digitalPinToInterrupt(button2), streetlightButtonClick, RISING);

  WiFiManager wifiManager;
  wifiManager.setTimeout(180);

  //Light sensor bit
  Wire.begin();
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE_2)) {
    Serial.println(F("BH1750 Advanced begin"));
  }
  else {
    Serial.println(F("Error initialising BH1750"));
  }
  if (!wifiManager.autoConnect("SetupWIFIDataLogger")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  delay(3000);
}

void loop()
{
  buttonpress = digitalRead (button);
  if (buttonpress != 0) {
          Serial.println("button pressed");
//          displayInfo();
        }

  buttonpress2 = digitalRead (button2);
  if (buttonpress2 != 0) {
          Serial.println("button2 pressed");
//          displayInfo();
        }

  // This sketch displays information every time a new sentence is correctly encoded
  //while changed to if because the whole function is a loop
  if (ss.available() > 0){
    if (gps.encode(ss.read())) {
      if (gps.location.isValid()) //check whether gps location is valid
      {
        
        
        latitude = gps.location.lat();
        lat_str = String(latitude , 6); // latitude location is stored in a string
        longitude = gps.location.lng();
        lng_str = String(longitude , 6); //longitude location is stored in a string
        latlong = lat_str + "," + lng_str;
        Serial.println(latlong);
        sats = gps.satellites.value();
        speedkmph = gps.speed.kmph();
        if (lightMeter.measurementReady()) {
          lux = lightMeter.readLightLevel();
          Serial.print("Light: ");
          Serial.print(lux);
          Serial.println(" lx");
        }
        
        //String string_y     =  String(sendvalue);
        //String url = "/macros/s/" + GAS_ID + "/exec?A=" + latlong + "&B=" + lux;
        issue = "";
        
        if(potholeFlag)
        { 
          Serial.print("came into pothole part");
          issue = "pothole";

          potholeFlag=false;
          streetlightFlag=false;
        }
        if(streetlightFlag)
        { 
          issue="streetlight";
          Serial.print("came into streetlight part");

          streetlightFlag=false;
          potholeFlag=false;
        }
        String url = "/macros/s/" + GAS_ID + "/exec?A=" + latlong + "&B=" + sats + "&C=" + speedkmph + "&D=" + lux + "&device=" + DevID + "&issue=" + issue;
        senddata(url);

      }
    }
}
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    
  }
}

void displayInfo()
{
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

void senddata(String url) {
  client.setInsecure();
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }

  Serial.print("requesting URL: ");
  Serial.println(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}

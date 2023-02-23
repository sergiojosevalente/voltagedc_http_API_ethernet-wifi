#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <EthernetClient.h> 
#include <Adafruit_ADS1015.h>
#include <LiquidCrystal_I2C.h> 
#include <Wire.h>
#define ETH_CS 5

Adafruit_ADS1115 ads(0x48);

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

char server[] = "portal.telkomcel.tl";    

const int kNetworkDelay = 1000;
const int kNetworkTimeout = 30*1000;

int calib = 7; 
float voltage =0; 
float Radjust = 0.042058261; 
float vbat =0;
// int lcdColumns = 16;
// int lcdRows = 2; 
int baud = 115200;

//disesuaikan alamat lcdnya
// LiquidCrystal_I2C lcd(0x38, lcdColumns, lcdRows);

void setup()
{
  // lcd.begin();         // initialize the lcd
  // lcd.backlight();    // open the backlight
  Serial.begin(baud);
  Wire.begin(22, 21);
  ads.begin();
  Serial.println("ESP32 W5500 Start");
  Ethernet.init(ETH_CS);
  Serial.println("Starting ETH connection...");
    
  if (!Ethernet.begin(mac)) {
    Serial.println("Failed to configure Eth using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Eth shield was not found.");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Eth cable is not connected.");
    }
    while (true) {
      delay(1);
    }
  }

  Serial.print("Ethernet IP is: ");
  Serial.println(Ethernet.localIP());

  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.print("connecting to ");
  Serial.print(server);
  Serial.println("...");
}

void loop()
{
  // lcd.clear();
  // Rumus
  int16_t adc0; // 16 bits ADC read of input A0
  adc0 = ads.readADC_SingleEnded(0);
  voltage = ((adc0 + calib) * 0.1875)/1000;

  vbat = voltage/Radjust;

  if (vbat < 0.1)
  {
    vbat = 0.01;
  }    
    
  // SENDING TO API
  int err =0;
  String path = "/voltmeter/fn_setdata?l=NOC+Aimutin&v="+String(vbat);

  EthernetClient c;
  HttpClient http(c);
  err = http.get(server, path.c_str());

  // lcd.setCursor(0, 0);  
  // lcd.print("Volt: ");
  // lcd.print(vbat);
  // lcd.print(" V");
  Serial.println(" Volt: ");
  Serial.println(vbat);

  if (err == 0)
  {
    Serial.println("startedRequest ok");
    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);
      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
        unsigned long timeoutStart = millis();
        char c;
        while ( (http.connected() || http.available()) && ((millis() - timeoutStart) < kNetworkTimeout) )
        {
          if (http.available())
          {
            c = http.read();
            // lcd.setCursor(0, 1);
            // lcd.print("success");
            Serial.print(c);
            
            bodyLen--;
            timeoutStart = millis();
          } else {
            delay(kNetworkDelay);
          }
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop(); 

  delay(3000);
}
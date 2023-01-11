#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <EthernetClient.h>
#include <LiquidCrystal_I2C.h> 
#define ETH_CS 5

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

char server[] = "server.tl";    // name address for Google (using DNS)

const int kNetworkDelay = 1000;
const int kNetworkTimeout = 30*1000;

float vPow = 1;
float r1 = 205000;
float r2 = 10000;
float volt;
int pin = 35;
int lcdColumns = 16;
int lcdRows = 2;
int baud = 115200;

//disesuaikan alamat lcdnya
LiquidCrystal_I2C lcd(0x38, lcdColumns, lcdRows);

void setup()
{
  lcd.begin();         // initialize the lcd
  lcd.backlight();    // open the backlight
  Serial.begin(baud);
  Serial.println("ESP32 W5500 Start");
  // ethernetReset(ETH_RST);
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
  lcd.clear();
  // Rumus
  volt=analogRead(pin);
  float v = (analogRead(pin) * vPow) / 1024.0;
  float v2 = v / (r2 / (r1 + r2));
  // SENDING TO API
  int err =0;
  String path = "linkapi="+String(v2);

  EthernetClient c;
  HttpClient http(c);
  err = http.get(server, path.c_str());

  lcd.setCursor(0, 0);  
  lcd.print("Volt: ");
  Serial.print("Volt: ");
  Serial.println(v2);
  lcd.print(v2);
  lcd.print(" V");
  Serial.print("  Status: ");
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
            lcd.setCursor(0, 1);
            lcd.print(c);
            Serial.println(c);
            
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
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_ADS1015.h>
#include <Wire.h>

Adafruit_ADS1115 ads(0x48);

//Network SSID
const char* ssid = "Telkomcel";
const char* password = "telkomcel1212";

int calib = 7; 
float voltage =0; 
float Radjust = 0.043058261; 
float vbat =0;
int baud = 115200;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(baud);
  Wire.begin(22, 21);
  ads.begin();
  //wifi koneksi
  WiFi.hostname("Check Voltage");
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi Connected");
  Serial.println("IP Address :");
  Serial.println(WiFi.localIP());
  
}

void loop() {
  int16_t adc0; // 16 bits ADC read of input A0
  adc0 = ads.readADC_SingleEnded(0);
  voltage = ((adc0 + calib) * 0.1875)/1000;

  vbat = voltage/Radjust;

  if (vbat < 0.1)
  {
    vbat = 0.01;
  }    
Serial.print("Volt: ");
Serial.println(vbat);
Serial.print("  Status: ");
if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    String Link = "http://portal.telkomcel.tl/voltmeter/fn_setdata?l=NOC+Aimutin&v=" + String(vbat);
    http.begin(Link.c_str());

    int httpCode = http.GET();
    String payload = http.getString();
    Serial.println(payload);
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
  delay(3000);
}
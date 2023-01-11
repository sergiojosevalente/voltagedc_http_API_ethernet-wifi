#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Ping.h>

//Network SSID
const char* ssid = "ssid";
const char* password = "password";

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

void setup() {
  lcd.begin();         // initialize the lcd
  lcd.backlight();    // open the backlight
  // put your setup code here, to run once:
  Serial.begin(baud);
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
  
  bool success = Ping.ping("www.google.com", 3);
  if(!success)
  {
    Serial.println("Ping failed");
    return;
  }
    Serial.println("Ping succesful.");
    return;
}

void loop() {
// put your main code here, to run repeatedly:
lcd.clear();
volt=analogRead(pin);
float v = (analogRead(pin) * vPow) / 1024.0;
float v2 = v / (r2 / (r1 + r2));

// float v3 = (analogRead(pin) / 4096) * 24 * (r2/r2);
// float v2 = v / (r2 / (r1 + r2));

lcd.setCursor(0, 0);  // display position
lcd.print("Volt: ");
Serial.print("Volt: ");
Serial.println(v2);
lcd.print(v2);
lcd.print(" V");
Serial.print("  Status: ");
if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    String Link = "http://api" + String(volt);
    http.begin(Link.c_str());

    int httpCode = http.GET();
    String payload = http.getString();
    lcd.setCursor(0, 1);
    lcd.print(payload);
    Serial.println(payload);
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
  delay(2000);
}

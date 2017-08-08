#include <Adafruit_NeoPixel.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 14
#define NUM_LEDS 24
#define BRIGHTNESS 255

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);

const char* ssid = "galactica";
const char* password = "FrackingToast3r";


ESP8266WebServer server(80);

const int led = 0;

void setColor(uint32_t color) {
//  Serial.print("Color ");
//  Serial.print(color);
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
//  Serial.println("Rendered");
}

void handleMsg() {
  digitalWrite(led, HIGH);
  int r = server.arg("r").toInt();
  int g = server.arg("g").toInt();
  int b = server.arg("b").toInt();
  int w = server.arg("w").toInt();
  int l = server.arg("l").toInt();
  Serial.print("Red ");
  Serial.println(String(r));
  setColor(strip.Color(r, g, b, w));
  strip.setBrightness(l);
  strip.show();
  String resp = "Received params: ";
  for (int i = 0; i < server.args(); i++) {
    resp += " ";
    resp += server.arg(i);
    resp += "\n";
  }
  server.send(200, "text/plain", resp);
  digitalWrite(led, LOW);
}

void loadingLights(int i) {
  for (int i = 0; i < 100; i++) {
//    setColor(strip.Color(210, 180, 100, 50));
    strip.setPixelColor(i % (NUM_LEDS - 1), 210, 280, 100, 50);
    strip.setPixelColor(i % NUM_LEDS, 0, 0, 0, 255);
    strip.show();
//    delay(50);
  }
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  delay(100);
  digitalWrite(led, HIGH);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");
  setColor(strip.Color(0, 0, 0, 255));
  // Wait for connection
  int ctr = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
    loadingLights(ctr);
    ctr++;
     
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/msg", handleMsg);

  server.on("/status", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  // Let us know that http server has started
  setColor(strip.Color(0, 240, 0, 0));
  delay(1000);
  // Set white while waiting for instruction
  setColor(strip.Color(0, 0, 0, 255));
  Serial.println("Lights are on white");
//  delay(500);
//  digitalWrite(led, LOW);
//  delay(500);
//  digitalWrite(led, HIGH);
//  delay(500);
//  digitalWrite(led, LOW);
//  delay(200);
//  digitalWrite(led, HIGH);
//  delay(1100);
//  digitalWrite(led, LOW);
//  delay(100);
}

void loop(void){
  server.handleClient();
}


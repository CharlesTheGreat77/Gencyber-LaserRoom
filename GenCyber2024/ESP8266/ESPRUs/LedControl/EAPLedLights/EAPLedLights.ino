#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define LED_PIN D4
#define NUM_LEDS 947 // 6 led strips (150 each) + 47 leds

CRGB leds[NUM_LEDS];

//existing AP
const char* ssid_name = "ssid";
const char* ssid_password = "ssid_password";

ESP8266WebServer server(80);

void handle_root_endpoint() {
  server.send(200, "text/plain", "LED Control");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
}

void white_leds() {
  for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::White;
  }
}

void red_leds() {
  for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Red;
  }
}


void turn_on_led() {
  String color = server.arg("color");
    if (color == "red") {
      red_leds();
    } else {
      white_leds();
    }
  FastLED.show();
  server.send(200, "text/plain", "LED is ON\n");
}

void turn_off_led() {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
  }
  FastLED.show();
  server.send(200, "text/plain", "LED is OFF\n");
}

void set_brightness() {
  int brightness = server.arg("brightness").toInt();
  if (brightness > 0 && brightness < 255) {
    FastLED.setBrightness(brightness);
  }
  FastLED.show();
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid_name, ssid_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("[*] Successfully connected to AP..");
  Serial.printf(" -> IP Address: %s\n", WiFi.localIP().toString().c_str());

  pinMode(LED_BUILTIN, OUTPUT);

  // same shit as the laser
  server.on("/", handle_root_endpoint);
  server.on("/led/on", turn_on_led);
  server.on("/led/off", turn_off_led);
  server.on("/led/brightness", set_brightness);

  server.begin();
  Serial.println("[*] HTTP Server Started..");
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(45); //default brightness based on power issue
  turn_off_led(); // turn leds off on startup..
  FastLED.show();  
}

void loop() {
  server.handleClient();
  digitalWrite(LED_BUILTIN, LOW);
}

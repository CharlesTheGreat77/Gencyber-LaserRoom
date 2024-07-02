#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Connect to existing AP
const char* ssid_name = "ssid";
const char* ssid_password = "ssid_password";

const int relay_pins[] = {D1, D2, D3, D5, D6, D7}; // one can control 6 lasers (hooked up to relays)
const int num_relays = sizeof(relay_pins) / sizeof(relay_pins[0]);

// Webserver port
ESP8266WebServer server(80);

void handle_root_endpoint() {
  server.send(200, "text/plain", "Laser Control");
}

void turn_laser_on() {
  int laser_number = server.arg("num").toInt();  
  if (laser_number >= 1 && laser_number <= num_relays) {
    digitalWrite(relay_pins[laser_number - 1], HIGH);
    delay(200);
    server.send(200, "text/plain", "Laser " + String(laser_number) + " is ON");
    Serial.println("[*] Laser " + String(laser_number) + " is on..");
  } else {
    for (int i = 0; i < num_relays; i++) {
      if (relay_pins[i] == D1) {
        digitalWrite(relay_pins[i], LOW); // don't know WTF is going on and its pissing me off..
      } else if (relay_pins[i] == D2) {
        digitalWrite(relay_pins[i], LOW); // same here.. pin 1 and 2.. might be power problem idefk
      } else {
        digitalWrite(relay_pins[i], HIGH);
      }
      delay(100);
    }
    server.send(200, "text/plain", "All lasers are ON");
  }
}

void turn_laser_off() {
  int laser_number = server.arg("num").toInt();
  Serial.printf("[*] Received request to turn off laser %d\n", laser_number);

  if (laser_number >= 1 && laser_number <= num_relays) {
    if (laser_number == 1) {
      digitalWrite(D1, HIGH); // HIGH TURNS THIS BITCH OFF LIKE WTF!!!!!
    } else if (laser_number == 2) {
      digitalWrite(D2, HIGH); // same here.. 
    }
    for (int i = 0; i < num_relays; i++) {
      if (relay_pins[i] == laser_number-1) {
        digitalWrite(relay_pins[laser_number-1], LOW);
        delay(200);
      }
    Serial.printf("[*] Turning off laser %d\n", i + 1);
    server.send(200, "text/plain", "Laser " + String(laser_number) + " is OFF");
    Serial.println("[*] Laser " + String(laser_number) + " is off..");
    }
  } else {
    for (int i = 0; i < num_relays; i++) {
      digitalWrite(relay_pins[i], LOW);
      delay(100);
      Serial.printf("[*] Turning off laser %d\n", i + 1);
    }
    server.send(200, "text/plain", "All lasers are OFF");
  }
}

void setup() {
  Serial.begin(9600);
  
  WiFi.begin(ssid_name, ssid_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);

  Serial.println("[*] Successfully connected to AP..");
  Serial.printf(" -> IP Address: %s\n", WiFi.localIP().toString().c_str());

  for (int i = 0; i < num_relays; i++) {
    pinMode(relay_pins[i], OUTPUT);
    digitalWrite(relay_pins[i], LOW); // all relays are off at startup
    Serial.printf("[*] Initialized relay pin %d (D%d)\n", relay_pins[i], i + 1);
  }


  // URL handlers
  server.on("/", handle_root_endpoint);
  server.on("/laser/on", turn_laser_on);
  server.on("/laser/off", turn_laser_off);

  // Start the server
  server.begin();
  Serial.println("[*] HTTP Server Started..");
}

void loop() {
  server.handleClient();
}
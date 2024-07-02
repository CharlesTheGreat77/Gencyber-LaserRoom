// made by Bit_Lost, DoobTheGoober, and choi.sauce
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>

const char* ssid_name = "ssid";
const char* ssid_password = "ssid_password";

float highestSensorInput = 0.0;
int cal_Count = 0;
bool armState = false;

const int THIS_UNIT_NUMBER = 6;
const int CALIBRATION_REQUIREMENT = 100;
const float PERCENTAGE_THRESHOLD  = .7;
const int buttonPin = D1;

int buttonState = 0;

WiFiClient client;
HTTPClient http;

void sendRequest(const char* url) {
  http.begin(client, url);
  int httpCode = http.GET();
  if (httpCode > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void alarm() {
  Serial.print("[*] Turning LEDS red..\n");
  sendRequest("http://192.168.0.102/led/on?color=red");

  Serial.print("[*] Turning off Lasers..\n");
  sendRequest("http://192.168.0.101/laser/off");
  delay(2000);

  Serial.print("[*] Turning LEDS white..\n");
  sendRequest("http://192.168.0.102/led/off");

  Serial.print("[*] Turning on lasers...\n");
  sendRequest("http://192.168.0.101/laser/on"); 
}

void laser_Off() {
  Serial.print("[*] Turning off laser 5..\n");
  sendRequest("http://192.168.0.115/laser/off?num=5");
}

void calibration() {
  if (cal_Count >= CALIBRATION_REQUIREMENT) {
    armState = true;
  }
  cal_Count++;
}

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  WiFi.mode(WIFI_STA);  //was tweakin tf out.. might've been a channel issue tbh
  WiFi.begin(ssid_name, ssid_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[*] Successfully connected to AP..");
  Serial.printf(" -> IP Address: %s\n", WiFi.localIP().toString().c_str());

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  delay(750);
}

void loop() {
  int currentSensorValue = analogRead(A0);

  //converts pin input to voltage value
  float voltage = currentSensorValue * (5.0 / 1023.0);
  //prints voltage to serial
  Serial.println(voltage);

  /* looks for sharp drop in input, if current value drops a 
  * percentage below the highest recorded value while armed, 
  * the alarm will sound
  */
  if ((currentSensorValue < (highestSensorInput * (PERCENTAGE_THRESHOLD - .2))) && armState) {
    alarm();
  }
  else if (armState) {
    Serial.print("[ARMED]\n");
  }
  //starts calibrating if there is a high consistent voltage signal
  else if (currentSensorValue >= (highestSensorInput * PERCENTAGE_THRESHOLD)) {
    highestSensorInput = currentSensorValue;
    calibration();
  }
  else if (currentSensorValue < (highestSensorInput * PERCENTAGE_THRESHOLD)) {
    cal_Count = 0;
  } 
  //starts tweaking otherwise.. idk
  delay(1);
}

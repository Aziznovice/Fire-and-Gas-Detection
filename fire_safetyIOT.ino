//this projected is made by student of ZPPSU College of Information and Computing Sciences of 2023 -  2024
#define BLYNK_TEMPLATE_ID "//create a bynk account"
#define BLYNK_TEMPLATE_NAME "//create a bynk account"
#define BLYNK_AUTH_TOKEN "//create a bynk account"
#define BLYNK_PRINT Serial
#define BLYNK_DEBUG
#include <Blynk.h>
#include "BlynkSimpleEsp32.h"
#include <Arduino_ESP32_OTA.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "arduino_secrets.h"
#include "thingProperties.h"

#define AOUT_PIN_MQ2 34    // Example GPIO34 connected to AOUT pin of the MQ2 sensor (ADC1)
#define AOUT_PIN_FLAME 35  // Example GPIO35 connected to AOUT pin of the flame sensor (ADC1)
#define BUZZER_PIN 5        // ESP32's pin GPIO5 connected to the buzzer
#define LED_RED_PIN 22       // ESP32's pin GPIO22 connected to the red LED
#define LED_GREEN_PIN 23     // ESP32's pin GPIO23 connected to the green LED

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(SECRET_SSID, SECRET_OPTIONAL_PASS);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
  } else {
    Serial.println("\nFailed to connect to WiFi");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(AOUT_PIN_MQ2, INPUT);
  pinMode(AOUT_PIN_FLAME, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);

  ledcSetup(0, 5000, 8);
  ledcAttachPin(BUZZER_PIN, 0);

  digitalWrite(LED_RED_PIN, HIGH);

  // Connect to WiFi
  connectToWiFi();

  // Check if WiFi, Blynk, and Arduino Cloud are connected
  if (WiFi.status() == WL_CONNECTED && Blynk.connected() && ArduinoCloud.connected()) {
    digitalWrite(LED_RED_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, HIGH);
    Serial.println("Connected, Sensors are ready!");
  } else {
    Serial.println("WiFi, Blynk, or IoT Cloud connection failed. Sensors may not work correctly.");
  }

  Blynk.begin(BLYNK_AUTH_TOKEN, SSID, PASS);
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  delay(1500);
  setDebugMessageLevel(4);
  ArduinoCloud.printDebugInfo();
}


void loop() {
  ArduinoCloud.update();
  Blynk.run();
  int smokeValue = analogRead(AOUT_PIN_MQ2);
  gas = smokeValue;  // Update IoT Cloud property
  Serial.print("Smoke Value: ");
  Serial.println(smokeValue);

  // Read flame sensor from A0
  int flameValue = analogRead(AOUT_PIN_FLAME);
  fire = flameValue;  // Update IoT Cloud property
  Serial.print("Flame Value: ");
  Serial.println(flameValue);

  // Check for smoke and activate buzzer with different tones
  if (smokeValue > 1000) {
    // Blynk.email("websitesake1122@gmail.com", "Alert", "Gas detected!");
    Blynk.logEvent("gas_leakage_alert"," Gas is Detected");
    tone(BUZZER_PIN, 1000);  // Adjust frequency for smoke detection
    Serial.println("Smoke detected! Buzzer sounding (Smoke tone).");
    digitalWrite(LED_RED_PIN, HIGH);
    digitalWrite(LED_GREEN_PIN, LOW); // Alternate red and green LEDs
    delay(3000);  // Adjust duration for the tone to be audible
    noTone(BUZZER_PIN);  // Turn off the buzzer
    Blynk.virtualWrite(V0, smokeValue); 
  }

  // Check for flame and activate buzzer with different tones
  if (flameValue < 2500) {
    // Blynk.email("websitesake1122@gmail.com", "Alert", "Flame Detected!");
    Blynk.logEvent("fire_alert"," fire is Detected");
    tone(BUZZER_PIN, 900);  // Adjust frequency for flame detection
    digitalWrite(LED_RED_PIN, HIGH);
    digitalWrite(LED_GREEN_PIN, LOW); // Alternate red and green LEDs
    Serial.println("Flame detected! Buzzer sounding (Flame tone).");
    Blynk.virtualWrite(V1, flameValue); 
  } else {
    noTone(BUZZER_PIN);
    digitalWrite(LED_RED_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, HIGH);
  }

  delay(1000);  // You can adjust the delay based on your application requirements
}

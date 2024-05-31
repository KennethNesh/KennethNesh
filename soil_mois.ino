#include <BlynkSimpleStream.h>
#include <BlynkWidgets.h>

// You should get Auth Token in the Blynk App.
char auth[] = "YourAuthToken";

// Set the pin numbers
const int soilMoistureSensorPin = A0; // Soil moisture sensor
const int waterPumpRelayPin = 7;   // Water pump relay
const int ultrasonicTriggerPin = 8; // Ultrasonic sensor trigger
const int ultrasonicEchoPin = 9;     // Ultrasonic sensor echo

// Thresholds
const float moistureThreshold = 50.0; // Moisture threshold percentage
const int waterTankMaxDistance = 200; // Maximum distance (in cm) when the tank is empty
const int distanceThreshold = 50; // Distance threshold (in cm) to refill the tank

// Variables to store readings
float soilMoisturePercentage;
long ultrasonicDistance;

// Blynk widgets
WidgetLED soilMoistureLED(V1);
WidgetLED waterLevelLED(V2);
WidgetNotifications notifications;

BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncAll();
}

void setup() {
  // Debug console
  Serial.begin(9600);

  // Initialize Blynk
  Blynk.begin(Serial, auth);

  // Set pin modes
  pinMode(soilMoistureSensorPin, INPUT);
  pinMode(waterPumpRelayPin, OUTPUT);
  pinMode(ultrasonicTriggerPin, OUTPUT);
  pinMode(ultrasonicEchoPin, INPUT);

  // Initialize the water pump to OFF
  digitalWrite(waterPumpRelayPin, HIGH); // Assuming a normally closed relay
}

void loop() {
  Blynk.run();

  // Read soil moisture
  int soilMoistureValue = analogRead(soilMoistureSensorPin);
  soilMoisturePercentage = map(soilMoistureValue, 0, 1023, 100, 0);

  // Control the water pump based on soil moisture
  if (soilMoisturePercentage < moistureThreshold) {
    digitalWrite(waterPumpRelayPin, LOW); // Turn on the water pump
    soilMoistureLED.on();
  } else {
    digitalWrite(waterPumpRelayPin, HIGH); // Turn off the water pump
    soilMoistureLED.off();
  }

  // Update soil moisture percentage on Blynk
  Blynk.virtualWrite(V3, soilMoisturePercentage);

  // Read distance from ultrasonic sensor
  digitalWrite(ultrasonicTriggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonicTriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonicTriggerPin, LOW);
  long duration = pulseIn(ultrasonicEchoPin, HIGH);
  ultrasonicDistance = duration * 0.034 / 2;

  // Control the water tank refill based on distance
  if (ultrasonicDistance > waterTankMaxDistance - distanceThreshold) {
    // Refill the tank (e.g., turn on a valve or pump)
    waterLevelLED.on();
  } else {
    // Stop refilling the tank
    waterLevelLED.off();
  }

  // Update water level on Blynk
  Blynk.virtualWrite(V4, ultrasonicDistance);

  // Check for notifications
  if (soilMoisturePercentage < moistureThreshold || ultrasonicDistance > waterTankMaxDistance - distanceThreshold) {
    notifications.notify("Soil moisture or water level alert!");
  }

  delay(1000); // Update every 1 second
}

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Define LED pins
#define RED_LED D3
#define YELLOW_LED D4
#define GREEN_LED D5
#define LED_hw038_high D6
#define LED_hw038_low D7

// Define actuator pin
#define PUMP_PIN D8

// Define sensor pin
#define HW_038_PIN A0

// Threshold for HW-038 sensor
const int HW038_THRESHOLD = 400; // Adjust as needed for your application

// Network credentials and MQTT server details
const char* ssid = "A54";
const char* password = "0987654321";
const char* mqtt_server = "87.236.166.178"; // Replace with your actual server IP

// WiFi and MQTT client objects
WiFiClient espClient;
PubSubClient client(espClient);

// Function declarations
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);//for recive pumpstate from cloud
void reconnect();
void reciver(float value);
void hw038_high();
void hw038_low();
int readHW038Sensor();
void checkHW038Sensor();

void setup() {
  // Initialize Serial communication for debugging
  Serial.begin(9600);

  // Initialize LED pins
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(LED_hw038_high, OUTPUT);
  pinMode(LED_hw038_low, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);

  // Set initial LED states
  digitalWrite(RED_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  // Connect to WiFi and set up MQTT
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  // Reconnect if disconnected
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Check the sensor and handle the readings
  checkHW038Sensor();

  // Delay to avoid flooding with requests
  delay(3000);
}

void setup_wifi() {
  delay(100);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
/////////////dastaaan!!!!!!!!!!!!!!start
void callback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming MQTT messages
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // Convert payload to string
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Check if the message is for the pump state
  if (String(topic) == "PumpState") {
    if (message == "1") {
      digitalWrite(PUMP_PIN, LOW); // Turn the pump ON
      Serial.println("Pump turned ON");
    } else if (message == "2") {
      digitalWrite(PUMP_PIN, HIGH); // Turn the pump OFF
      Serial.println("Pump turned OFF");
    }
  }
}
////////dastan endُ
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("NodeMCUClient", "admin", "123")) {
      Serial.println("connected");
      client.subscribe("PumpState"); // Subscribe to the pump state topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }سسسُُُ
  }
}

void reciver(float value) {
  String height = String(value);
  client.publish("WaterHeight", height.c_str());
}

void hw038_high() {
  digitalWrite(LED_hw038_low, LOW);
  digitalWrite(LED_hw038_high, HIGH);
}

void hw038_low() {
  digitalWrite(LED_hw038_high, LOW);
  digitalWrite(LED_hw038_low, HIGH);
}

int readHW038Sensor() {
  int HW038_Value = analogRead(HW_038_PIN);
  Serial.print("HW_038_PIN: "); // Debug
  Serial.println(HW038_Value);
  return HW038_Value;
}

void checkHW038Sensor() {
  int HW038_Value = readHW038Sensor();
  reciver(HW038_Value);

  if (HW038_Value > HW038_THRESHOLD) {
    hw038_low();
  } else {
    hw038_high();
  }
}

#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define RX_PIN D2  // RX pin on NodeMCU (connects to TX3 on Arduino Mega)
#define TX_PIN D1  // TX pin on NodeMCU (not used in this setup but connect to RX3)

// Define LED pins
#define RED_LED D5
#define YELLOW_LED D6
#define GREEN_LED D7
#define CLOUD_LED D4

SoftwareSerial megaSerial(RX_PIN, TX_PIN);

// Network SSID and Password
const char* ssid = "A54";
const char* password = "0987654321";
const char* mqtt_server = "87.236.166.178"; // MQTT server IP

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(100);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(YELLOW_LED, !digitalRead(YELLOW_LED)); // Blink yellow LED while connecting
    digitalWrite(RED_LED, HIGH); // Red LED on during connection attempt
  }
  
  digitalWrite(YELLOW_LED, LOW); // Turn off yellow LED when connected
  digitalWrite(RED_LED, LOW); // Turn off red LED when connected

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Handle message arrived
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    digitalWrite(YELLOW_LED, HIGH); // Yellow LED on during connection attempt
    // Attempt to connect with username and password
    if (client.connect("NodeMCUClient", "admin", "123")) {
      Serial.println("connected");
      digitalWrite(GREEN_LED, HIGH); // Turn on green LED when connected
      digitalWrite(RED_LED, LOW); // Turn off red LED when connected
      digitalWrite(YELLOW_LED, LOW); // Turn off yellow LED when connected
      client.subscribe("yourTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
      digitalWrite(YELLOW_LED, LOW); // Turn off yellow LED between attempts
      digitalWrite(RED_LED, HIGH); // Turn on red LED if connection failed
    }
  }
}

void receiver() {
  Serial.println("Receive Function!!");
  if (megaSerial.available()) {
    String receivedData = megaSerial.readStringUntil('\n');

    Serial.print("Received data: ");
    Serial.println(receivedData);

    int commaIndex1 = receivedData.indexOf(',');
    int commaIndex2 = receivedData.indexOf(',', commaIndex1 + 1);
    int commaIndex3 = receivedData.indexOf(',', commaIndex2 + 1);
    int commaIndex4 = receivedData.indexOf(',', commaIndex3 + 1);
    int commaIndex5 = receivedData.indexOf(',', commaIndex4 + 1);
    int commaIndex6 = receivedData.indexOf(',', commaIndex5 + 1);
    int commaIndex7 = receivedData.indexOf(',', commaIndex6 + 1);

    float temperature = receivedData.substring(0, commaIndex1).toFloat();
    float humidity = receivedData.substring(commaIndex1 + 1, commaIndex2).toFloat();
    int moisture = receivedData.substring(commaIndex2 + 1, commaIndex3).toInt();
    int gasValue = receivedData.substring(commaIndex3 + 1, commaIndex4).toInt();
    bool pumpState = receivedData.substring(commaIndex4 + 1).toInt();
    bool fan1State = receivedData.substring(commaIndex5 + 1, commaIndex6).toInt();
    bool fan2State = receivedData.substring(commaIndex6 + 1, commaIndex7).toInt();
    bool heaterState = receivedData.substring(commaIndex7 + 1).toInt();

    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.print("Moisture: ");
    Serial.println(moisture);
    Serial.print("Gas Value: ");
    Serial.println(gasValue);
    Serial.print("Pump State: ");
    Serial.println(pumpState);
    Serial.print("Fan 1 State: ");
    Serial.println(fan1State);
    Serial.print("Fan 2 State: ");
    Serial.println(fan2State);
    Serial.print("Heater State: ");
    Serial.println(heaterState);

    String temperatureStr = String(temperature);
    String humidityStr = String(humidity);
    String moistureStr = String(moisture);
    String gasValueStr = String(gasValue);
    String pumpStateStr = pumpState ? "1" : "0";
    String fan1StateStr = fan1State ? "1" : "0";
    String fan2StateStr = fan2State ? "1" : "0";
    String heaterStateStr = heaterState ? "1" : "0";

    client.publish("Temperature", temperatureStr.c_str());
    client.publish("Humidity", humidityStr.c_str());
    client.publish("Moisture", moistureStr.c_str());
    client.publish("GasValue", gasValueStr.c_str());
    client.publish("PumpState", pumpStateStr.c_str());
    client.publish("FanState-1", fan1StateStr.c_str());
    client.publish("FanState-2", fan2StateStr.c_str());
    client.publish("HeaterState", heaterStateStr.c_str());

    digitalWrite(CLOUD_LED, LOW); // Blink green LED to indicate data sent
    delay(100);
    digitalWrite(CLOUD_LED, HIGH);
  }
}

void setup() {
  Serial.begin(9600);
  megaSerial.begin(9600);

  pinMode(RED_LED, OUTPUT);
  pinMode(CLOUD_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  digitalWrite(RED_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(CLOUD_LED, LOW);

  Serial.println("NodeMCU ready to receive data from Arduino Mega");

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  receiver();
  delay(3000);
}



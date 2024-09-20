//library
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// Define communication pin
#define RX_PIN D2  // RX pin on NodeMCU (connects to TX3 on Arduino Mega)
#define TX_PIN D1  // TX pin on NodeMCU (not used in this setup but connect to RX3) becouse the comunicate is one way not two way
// Define LED pins
#define RED_LED D5 //
#define YELLOW_LED D6
#define GREEN_LED D7
//FOR COMUNICATION
SoftwareSerial megaSerial(RX_PIN, TX_PIN);
//network SSID AND PASSSWORD AND SERVER IP
const char* ssid = "A54";
const char* password = "0987654321";
const char* mqtt_server = "87.236.166.178";//"192.168.63.248" for amir local pc and "192.168.150.152" for ali local pc
//object  for wifi communication
WiFiClient espClient;
PubSubClient client(espClient);
//--------------------------------------------------------------------------------------------------------------------------------------
//***************************function************************************
//wifi
void setup_wifi() {
  delay(100);
  // Connect to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");

  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Handle message arrived  *?
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect with username and password
    if (client.connect("NodeMCUClient", "admin", "123")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("yourTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//------------------------------------------------------------------------------------------------------------------------------------
// reciver from mega
void reciver() {
  Serial.println("Receive Function!!");/deb
  if (true) { // Check if data is available to read from SoftwareSerial

    String receivedData = megaSerial.readStringUntil('\n'); // Read data until a newline character is encountered

    // Print the received data to the Serial Monitor
    Serial.print("Received data: ");
    Serial.println(receivedData);

    // Parse the received data (assuming the format: temperature,humidity,moisture,gasValue,pump_state)
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

    // Print parsed values for debugging
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

    // Convert numerical values to strings
    String temperatureStr = String(temperature);
    String humidityStr = String(humidity);
    String moistureStr = String(moisture);
    String gasValueStr = String(gasValue);
    String pumpStateStr = pumpState ? "1" : "0"; 
    String fan1StateStr = fan1State ? "1" : "0";
    String fan2StateStr = pumpState ? "1" : "0";
    String heaterStateStr = pumpState ? "1" : "0";

    // Publish the strings
    client.publish("Temperature", temperatureStr.c_str());
    client.publish("Humidity", humidityStr.c_str());
    client.publish("Moisture", moistureStr.c_str());
    client.publish("GasValue", gasValueStr.c_str());
    client.publish("PumpState", pumpStateStr.c_str());
    client.publish("FanState-1",  fan1StateStr.c_str());
    client.publish("FanState-2",  fan2StateStr.c_str());
    client.publish("HeaterState",  heaterStateStr.c_str());
  }
}


void setup() {
  // Initialize Serial communication for debugging
  Serial.begin(9600);
  megaSerial.begin(9600);

  // Initialize LED pins
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Initial LED state
  digitalWrite(RED_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

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
  reciver();
  delay(3000);//It sends to vm cloud every 3 seconds
}


//**************************library******************************
#include <Arduino.h>
#include <DHT.h>
#include <LiquidCrystal.h>
#include <Wire.h> 
//-------------------------------------------------------------------------------------------------------
//*************************CONSTANT*********************************
//STATE LED (UI)
#define LED_MQ4_NORMAL 22
#define LED_MQ4_CAUTION 23
#define LED_MQ4_DANGER 24
#define LED_DH11HU_NORMAL 25
#define LED_DH11HU_COUTION 26
#define LED_DH11T_COLD 27
#define LED_DH11T_WARM 28
#define LED_DH11T_HOT 29
#define LED_YL69_WET 30
#define LED_YL69_DRY 31
// INPUT SENSOR PIN
#define YL69_PIN A15
#define MQ4_PIN A8
#define DHT11_PIN 21
// OBJECT NAME DEFINE 
#define DHTTYPE DHT11
//CONTROLLER PIN
#define BUZZER_PIN 20//3
#define FAN_1_PIN 19
#define FAN_2_PIN 18
#define HITTER_PIN 17
//------------------------------------------------------------------------------------------------------
//THRESHOLDS
const int YL69_THRESHOLD = 700;
const int MQ4_THRESHOLD = 300;
const float DHT11_HU_THRESHOLD_HIGH = 60.0;
const float DHT11_HU_THRESHOLD_LOW = 38.0;
const float DHT11_TEMP_THRESHOLD_HOT = 37.0;
const float DHT11_TEMP_THRESHOLD_WARM = 15.0;
// Button pins
const int btnRIGHT = 0;
const int btnUP = 1;
const int btnDOWN = 2;
const int btnLEFT = 3;
const int btnSELECT = 4;
const int btnNONE = 5;
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
//-------------------------------------------------------------------------------------------------------
//********CLASS AND OBJECT*************
//LCD AND DHT OBJECT
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
DHT dht(DHT11_PIN, DHTTYPE);
//--------------------------------------------------------------------------------------------------------
//******************VARIABLE**********************
//PUMP STATE
bool pump_state = false;
// Current display mode
enum DisplayMode { TEMPERATURE_HUMIDITY, TEMPERATURE_F, YL69_MQ4, TIME_STATUS, FAN_STATUS };
DisplayMode currentMode = TEMPERATURE_HUMIDITY;
//DEFULT GLOBAL VALUE
float temperature = 0;
float humidity = 0;
int YL69_Value = 0;
int MQ4_Value = 0;
//----------------------------------------------------------------------------------------------------------------------------
//**********prototypes*******************
// Function prototypes
int readButtons();
void updateDisplay();
void showTemperatureHumidity();
void showTemperatureF();
void showYL69MQ4();
void showTimeStatus();
void showFanStatus();
//------------------------------------------------------------------------------------------------------------------------------
//************************FUNCTION***********************
//YL69
void yl69_dry() {
  digitalWrite(LED_YL69_WET, LOW);
  digitalWrite(LED_YL69_DRY, HIGH);
  pump_state = true;
  Serial.println(" dry Pump state: ON"); 
}
void yl69_wet() {
  digitalWrite(LED_YL69_DRY, LOW);
  digitalWrite(LED_YL69_WET, HIGH);
  pump_state = false;
  Serial.println(" yl69_wet Pump state: OFF"); 
}
int readYL69Sensor() {
  YL69_Value = analogRead(YL69_PIN);
  Serial.print("YL69_PIN: ");
  Serial.println(YL69_Value);
  return YL69_Value;
}
void checkYL69Sensor() {
  int YL69_Value = readYL69Sensor();
  if (YL69_Value > YL69_THRESHOLD) {
    yl69_wet();
  } else {
    yl69_dry();
  }
}
//--------------------------------------------------------------------------------------------------------------------------
//MQ4
void alarm_on() {
  digitalWrite(LED_MQ4_DANGER, HIGH);
  digitalWrite(LED_MQ4_CAUTION, LOW);
  digitalWrite(LED_MQ4_NORMAL, LOW);
  
  tone(BUZZER_PIN, 5000);
  delay(300);
  digitalWrite(FAN_1_PIN, LOW);
  Serial.println("gas danger");
  Serial.print("FAN_1_PIN: ");
  Serial.println(digitalRead(FAN_1_PIN));
}
void alarm_off() {
  digitalWrite(LED_MQ4_DANGER, LOW);
  digitalWrite(LED_MQ4_CAUTION, LOW);
  digitalWrite(LED_MQ4_NORMAL, HIGH);
  noTone(BUZZER_PIN);
  digitalWrite(FAN_1_PIN, HIGH);
  Serial.println("gas normal");
  Serial.println(digitalRead(FAN_1_PIN));
}
void MQ4_caution() {
  digitalWrite(LED_MQ4_DANGER, LOW);
  digitalWrite(LED_MQ4_CAUTION, HIGH);
  digitalWrite(LED_MQ4_NORMAL, LOW);
  noTone(BUZZER_PIN);
  digitalWrite(FAN_1_PIN, HIGH);
  Serial.println("gas Caution");
  Serial.println(digitalRead(FAN_1_PIN));
}
int readMQ4Sensor() {
  int MQ4_Value = analogRead(MQ4_PIN);
  Serial.print("MQ-4: ");
  Serial.println(MQ4_Value);
  return MQ4_Value;
}
void checkMQ4Sensor() {
  MQ4_Value = readMQ4Sensor();
  if (MQ4_Value > MQ4_THRESHOLD) {
    alarm_on();
  } else if (MQ4_Value > MQ4_THRESHOLD - 50) {
    MQ4_caution();
  } else {
    alarm_off();
  }
}
//----------------------------------------------------------------------------------------------------------------------------------------
//DH11
void dh11hu_normal() {
  Serial.println("Humidity is normal.");
  digitalWrite(LED_DH11HU_NORMAL, HIGH);
  digitalWrite(LED_DH11HU_COUTION, LOW);
  digitalWrite(FAN_2_PIN, HIGH);
}
void dh11hu_caution() {
  Serial.println("Humidity caution.");
  digitalWrite(LED_DH11HU_NORMAL, LOW);
  digitalWrite(LED_DH11HU_COUTION, HIGH);
  digitalWrite(FAN_2_PIN, LOW);
}
void dh11temp_warm() {
  Serial.println("Temperature cool.");
  digitalWrite(LED_DH11T_COLD, LOW);
  digitalWrite(LED_DH11T_WARM, HIGH);
  digitalWrite(LED_DH11T_HOT, LOW);
  digitalWrite(FAN_1_PIN, HIGH);
  digitalWrite(HITTER_PIN, HIGH);
}
void dh11temp_cold() {
  Serial.println("Temperature cold.");
  digitalWrite(LED_DH11T_WARM, LOW);
  digitalWrite(LED_DH11T_COLD, HIGH);
  digitalWrite(LED_DH11T_HOT, LOW);
  digitalWrite(FAN_1_PIN, HIGH);
  digitalWrite(HITTER_PIN, LOW);
}
void dh11temp_hot() {
  Serial.println("Temperature hot.");
  digitalWrite(LED_DH11T_COLD, LOW);
  digitalWrite(LED_DH11T_WARM, LOW);
  digitalWrite(LED_DH11T_HOT, HIGH);
  digitalWrite(FAN_1_PIN, LOW);
  digitalWrite(HITTER_PIN, HIGH);
}
void checkDHT11Sensor() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("Humidity (%): ");
  Serial.println(humidity, 2);
  Serial.print("Temperature (°C): ");
  Serial.println(temperature, 2);
  // Check humidity
  if (humidity > DHT11_HU_THRESHOLD_HIGH || humidity < DHT11_HU_THRESHOLD_LOW) {
    dh11hu_caution();
  } else {
    dh11hu_normal();
  }
  // Check temperature
  if (temperature > DHT11_TEMP_THRESHOLD_HOT) {
    dh11temp_hot();
  } else if (temperature > DHT11_TEMP_THRESHOLD_WARM) {
    dh11temp_warm();
  } else {
    dh11temp_cold();
  }
}
//---------------------------------------------------------------------------------------------------------------
//COMMUNICATION TO NODEMCU(GATEWAY)
void send_to_node() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();       
    int moisture = analogRead(YL69_PIN);      
    int gasValue = analogRead(MQ4_PIN);       
    // Format data as a string to send via serial
    String dataToSend = String(temperature) + "," + String(humidity) + "," + String(moisture) + "," + String(gasValue) + "," + String(pump_state) + "," String(humidity) + "," String(humidity) + ",";
    // Send data via Serial3 every 5 minute (300000 milliseconds) we can ise 60000 ms (1 min) for see
    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 3000) {//decrease to test and debyg
        Serial3.println("Sending data to NodeMCU...");
        Serial3.println(dataToSend);
        Serial.println("Sending data to NodeMCU...");
        Serial.println(dataToSend);
        previousMillis = currentMillis;
    }
}
//--------------------------------------------------------------------------------------------------------------------------------
//LCD DISPLY UI
void button_select() {
  int button = readButtons();
  switch (button) {
    case btnLEFT:
      currentMode =TEMPERATURE_HUMIDITY ;
      updateDisplay();
      break;
    case btnRIGHT:
      currentMode = YL69_MQ4;
      updateDisplay();
      break;
    case btnUP:
      currentMode = TIME_STATUS ;
      updateDisplay();
      break;
    case btnDOWN:
      currentMode = FAN_STATUS;
      updateDisplay();
      break;
    case btnSELECT:
      currentMode = TEMPERATURE_F ;
      updateDisplay();
      break;
  }
}
int readButtons() {
  int adc_key_in = analogRead(0);
  if (adc_key_in > 1000) return btnNONE;
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 195)  return btnUP;
  if (adc_key_in < 380)  return btnDOWN;
  if (adc_key_in < 555)  return btnLEFT;
  if (adc_key_in < 790)  return btnSELECT;
  return btnNONE;
}
void updateDisplay() {
  lcd.clear();
  switch (currentMode) {
    case TEMPERATURE_HUMIDITY:
      showTemperatureHumidity();
      break;
    case TEMPERATURE_F:
      showTemperatureF();
      break;
    case YL69_MQ4:
      showYL69MQ4();
      break;
    case TIME_STATUS:
      showTimeStatus();
      break;
    case FAN_STATUS:
      showFanStatus();
      break;
  }
}
void showTemperatureHumidity() {
  float temperature = dht.readTemperature();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature, 1);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity, 1);
  lcd.print(" %");
}
void showTemperatureF() {
  float tempF = temperature * 9.0 / 5.0 + 32.0;
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(tempF, 1);
  lcd.print(" F");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity, 1);
  lcd.print(" %");
}
void showYL69MQ4() {
  lcd.setCursor(0, 0);
  lcd.print("Moisture: ");
  if (YL69_Value < YL69_THRESHOLD) {
    lcd.print("Wet");
  } else {
    lcd.print("Dry");
  }
  lcd.setCursor(0, 1);
  lcd.print("Gas: ");
  lcd.print(MQ4_Value);
}
void showTimeStatus() {
  lcd.setCursor(0, 0);
  lcd.print("pump status: ");
  lcd.print(pump_state); 

  lcd.setCursor(0, 1);
  lcd.print("heater status:");
  lcd.print(digitalRead(HITTER_PIN) ? "off" : "on");
}
void showFanStatus() {
  lcd.setCursor(0, 0);
  lcd.print("Fan 1 status:");
  lcd.print(digitalRead(FAN_1_PIN) ? "off" : "on");

  lcd.setCursor(0, 1);
  lcd.print("Fan 2 status:");
  lcd.print(digitalRead(FAN_2_PIN) ? "off" : "on");
}
//----------------------------------------------------------------------------------------------
//**********SETTING UP*********
void setup() {
  Serial.begin(9600); 
  Serial3.begin(9600);  // FOR SENDING TO NODE
  //LET PIN
  pinMode(LED_MQ4_NORMAL, OUTPUT);
  pinMode(LED_MQ4_CAUTION, OUTPUT);
  pinMode(LED_MQ4_DANGER, OUTPUT);
  pinMode(LED_YL69_WET, OUTPUT);
  pinMode(LED_YL69_DRY, OUTPUT);
  pinMode(LED_DH11T_COLD, OUTPUT);
  pinMode(LED_DH11T_WARM, OUTPUT);
  pinMode(LED_DH11T_HOT, OUTPUT);
  pinMode(LED_DH11HU_COUTION, OUTPUT);
  pinMode(LED_DH11HU_NORMAL, OUTPUT);
  //SENSOR PIN
  pinMode(YL69_PIN, INPUT);
  pinMode(MQ4_PIN, INPUT);
  pinMode(DHT11_PIN, INPUT);
  //CONTROL PIN
  pinMode(FAN_1_PIN, OUTPUT);
  pinMode(FAN_2_PIN, OUTPUT);
  pinMode(HITTER_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  //DEFAULT LED WRITE
  digitalWrite(LED_YL69_WET, LOW);
  digitalWrite(LED_YL69_DRY, LOW);
  digitalWrite(LED_DH11HU_NORMAL, LOW);
  digitalWrite(LED_DH11HU_COUTION, LOW);
  digitalWrite(LED_DH11T_COLD, LOW);
  digitalWrite(LED_DH11T_WARM, LOW);
  digitalWrite(LED_DH11T_HOT, LOW);
  digitalWrite(LED_MQ4_NORMAL, LOW);
  digitalWrite(LED_MQ4_DANGER, LOW);
  digitalWrite(LED_MQ4_CAUTION, LOW);
  //DEFAULT RELLAY WRITE (RELAY IS LOW ENABLE)
  digitalWrite(FAN_1_PIN, HIGH);
  digitalWrite(FAN_2_PIN, HIGH);
  digitalWrite(HITTER_PIN, HIGH);
  digitalWrite(BUZZER_PIN, LOW);
  //Start DHT sensor AND LCD
  dht.begin(); // 
  lcd.begin(16, 2);
  // Initial display
  updateDisplay();
  //READY TO GO
  Serial.println("System initializing...");
  delay(1000);
  Serial.println("Setup complete. Starting to read sensors...");
}  
//MAIN LOOP
void loop() {
  static unsigned long lastSensorReadTime = 0;
  static unsigned long lastButtonCheckTime = 0;
  const unsigned long sensorInterval = 5000; // 5 seconds for sensor checks decrease to test and debug
  const unsigned long buttonCheckInterval = 100; // 100 milliseconds for button checks

  unsigned long currentMillis = millis();

  // Check sensors at a fixed interval
  if (currentMillis - lastSensorReadTime >= sensorInterval) {
    checkYL69Sensor();
    checkMQ4Sensor();
    checkDHT11Sensor();
    send_to_node();
    lastSensorReadTime = currentMillis;
  }
  // Check buttons at a fixed interval
  if (currentMillis - lastButtonCheckTime >= buttonCheckInterval) {
    button_select();
    lastButtonCheckTime = currentMillis;
  }
}
#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// ========== USER CONFIG ==========
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";
const char* THINGSPEAK_API_KEY = "2BAW65SUPO5853ZJ";
// ==================================

// Pins (match wiring in Wokwi / your hardware)
const int PIN_MQ2_ADC = 34; // ADC1_CH6 (MQ-2 AOUT)
const int PIN_DS18B20 = 4; // 1-Wire data
const int PIN_BUZZER = 25; // active buzzer +
const int PIN_LED = 2; // LED anode via 220R

// Timing
const uint32_t SENSOR_SAMPLE_INTERVAL_MS = 2000; // sensor sample cadence
const uint32_t THINGSPEAK_UPDATE_INTERVAL_MS = 20000; // ThingSpeak >=15s recommended

// Alert thresholds (tune in real world)
const int MQ2_ALERT_ADC_THRESHOLD = 3000; // 0-4095
const float TEMP_ALERT_C = 50.0; // degrees Celsius

// ThingSpeak endpoint
const char* THINGSPEAK_HOST = "http://api.thingspeak.com/update";

// OneWire / DS18B20
OneWire oneWire(PIN_DS18B20);
DallasTemperature sensors(&oneWire);

// Shared sensor snapshot
typedef struct {
  float tempC;
  int mq2_adc;
  bool alert;
  unsigned long lastSensorMillis;
} SensorData;
SensorData sharedData;
SemaphoreHandle_t dataMutex = NULL;

// Forward task prototypes
void sensorTask(void* pvParameters);
void networkTask(void* pvParameters);

// Helper: read MQ-2 averaged
int readMQ2Raw() {
  const int SAMPLES = 8;
  long sum = 0;
  for (int i = 0; i < SAMPLES; ++i) {
    sum += analogRead(PIN_MQ2_ADC);
    vTaskDelay(pdMS_TO_TICKS(5)); // Non-blocking delay
  }
  int avg = (int)(sum / SAMPLES);
  return avg; // 0 - 4095
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== Smart Air Quality & Temp Monitor (Arduino, ESP32) ===");

  // pin modes
  pinMode(PIN_MQ2_ADC, INPUT);
  pinMode(PIN_DS18B20, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_LED, LOW);

  // initialize DS18B20
  sensors.begin();
  if (!sensors.getDeviceCount()) {
    Serial.println("[setup] WARN: No DS18B20 devices found!");
  }

  // create mutex
  dataMutex = xSemaphoreCreateMutex();
  if (dataMutex == NULL) {
    Serial.println("[setup] ERROR: failed to create mutex");
    while (1) vTaskDelay(pdMS_TO_TICKS(1000));
  }

  // init sharedData
  if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
    sharedData.tempC = NAN;
    sharedData.mq2_adc = 0;
    sharedData.alert = false;
    sharedData.lastSensorMillis = 0;
    xSemaphoreGive(dataMutex);
  }

  // Create tasks:
  // sensorTask pinned to core 0 (low-latency I/O)
  BaseType_t r1 = xTaskCreatePinnedToCore(
    sensorTask, "SensorTask", 4096, NULL, 2, NULL, 0);
  // networkTask pinned to core 1 (networking, blocking I/O)
  BaseType_t r2 = xTaskCreatePinnedToCore(
    networkTask, "NetworkTask", 8192, NULL, 1, NULL, 1);
  if (r1 != pdPASS || r2 != pdPASS) {
    Serial.println("[setup] ERROR creating tasks");
    while (1) vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void loop() {
  // All work done in tasks
  vTaskDelay(pdMS_TO_TICKS(1000));
}

// ---------- Sensor Task (Core 0) ----------
void sensorTask(void* pvParameters) {
  (void) pvParameters;
  Serial.printf("[sensorTask] started on core %d\n", xPortGetCoreID());
  for (;;) {
    unsigned long now = millis();

    // Request and read temp (DS18B20 conversion ~750ms, but we poll immediately)
    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0); // single sensor
    if (tempC == DEVICE_DISCONNECTED_C) {
      tempC = NAN; // Handle sensor error
      Serial.println("[sensorTask] WARN: DS18B20 read error");
    }

    // Read MQ-2 ADC (averaged)
    int mq_adc = readMQ2Raw();

    // Determine alert
    bool alert = false;
    if (mq_adc >= MQ2_ALERT_ADC_THRESHOLD) alert = true;
    if (!isnan(tempC) && tempC >= TEMP_ALERT_C) alert = true;

    // store snapshot under mutex
    if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      sharedData.tempC = tempC;
      sharedData.mq2_adc = mq_adc;
      sharedData.alert = alert;
      sharedData.lastSensorMillis = now;
      xSemaphoreGive(dataMutex);
    } else {
      Serial.println("[sensorTask] WARN: unable to take mutex");
    }

    // local alert hardware feedback
    if (alert) {
      digitalWrite(PIN_LED, HIGH);
      digitalWrite(PIN_BUZZER, HIGH);
      vTaskDelay(pdMS_TO_TICKS(150)); // Non-blocking beep
      digitalWrite(PIN_BUZZER, LOW);
      digitalWrite(PIN_LED, LOW); // Blink off after beep
    } else {
      digitalWrite(PIN_LED, LOW);
      digitalWrite(PIN_BUZZER, LOW);
    }

    // Log sensor data periodically (every 10 samples for debug)
    static int logCounter = 0;
    if (++logCounter % 10 == 0) {
      Serial.printf("[sensorTask] Temp: %.2f°C, MQ2: %d, Alert: %s\n",
                    isnan(tempC) ? 0.0 : tempC, mq_adc, alert ? "YES" : "NO");
    }

    vTaskDelay(pdMS_TO_TICKS(SENSOR_SAMPLE_INTERVAL_MS));
  }
}

// ---------- Network Task (Core 1) ----------
void networkTask(void* pvParameters) {
  (void) pvParameters;
  Serial.printf("[networkTask] started on core %d\n", xPortGetCoreID());

  // WiFi connect with retry
  WiFi.mode(WIFI_STA);
  unsigned long wifiStart = millis();
  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 10) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("[networkTask] Connecting to WiFi");
    wifiStart = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - wifiStart < 10000)) {
      vTaskDelay(pdMS_TO_TICKS(500));
      Serial.print(".");
    }
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("\n[networkTask] WiFi connect timeout, retrying...");
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
      vTaskDelay(pdMS_TO_TICKS(2000));
      retryCount++;
    }
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[networkTask] ERROR: WiFi failed after retries. Restarting...");
    ESP.restart();
  }
  Serial.println();
  Serial.print("[networkTask] Connected. IP: ");
  Serial.println(WiFi.localIP());

  HTTPClient http;
  unsigned long lastUpdate = 0;
  bool lastAlertState = false; // Track alert state to avoid spam

  for (;;) {
    // Check WiFi connection periodically
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[networkTask] WiFi disconnected, reconnecting...");
      WiFi.reconnect();
      vTaskDelay(pdMS_TO_TICKS(5000));
      continue;
    }

    unsigned long now = millis();

    // Take snapshot first for consistency
    SensorData snap;
    bool gotSnapshot = false;
    if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(300)) == pdTRUE) { // Increased timeout
      snap = sharedData;
      xSemaphoreGive(dataMutex);
      gotSnapshot = true;
    }

    if (!gotSnapshot) {
      Serial.println("[networkTask] WARN: couldn't take mutex for snapshot");
      vTaskDelay(pdMS_TO_TICKS(500));
      continue;
    }

    // Periodic ThingSpeak update (normal data)
    if (now - lastUpdate >= THINGSPEAK_UPDATE_INTERVAL_MS) {
      String url = String(THINGSPEAK_HOST) + "?api_key=" + THINGSPEAK_API_KEY;
      float tempField = isnan(snap.tempC) ? 0.0f : snap.tempC;
      url += "&field1=" + String(tempField, 2); // Suhu
      url += "&field2=" + String(snap.mq2_adc); // MQ2 raw
      url += "&field3=" + String(snap.alert ? 1 : 0); // Alert status
      Serial.println("[networkTask] Sending to ThingSpeak:");
      Serial.println(url);

      http.begin(url);
      int httpCode = http.GET();
      if (httpCode > 0) {
        String payload = http.getString();
        Serial.printf("[networkTask] ThingSpeak resp: %d payload:%s\n", httpCode, payload.c_str());
        lastAlertState = snap.alert; // Update state after successful send
      } else {
        Serial.printf("[networkTask] ERROR http.GET() code: %d\n", httpCode);
      }
      http.end();
      lastUpdate = now;
    }

    // Alert-only update: Only if alert changed to TRUE and enough time passed
    else if (snap.alert && !lastAlertState && (now - lastUpdate >= 15000)) { // Respect 15s min
      String url = String(THINGSPEAK_HOST) + "?api_key=" + THINGSPEAK_API_KEY;
      float tempField = isnan(snap.tempC) ? 0.0f : snap.tempC;
      url += "&field1=" + String(tempField, 2);
      url += "&field2=" + String(snap.mq2_adc);
      url += "&field3=1"; // Force alert flag
      Serial.println("[networkTask] Sending ALERT to ThingSpeak");
      http.begin(url);
      int httpCode = http.GET();
      if (httpCode > 0) {
        Serial.printf("[networkTask] Alert resp: %d\n", httpCode);
        lastAlertState = true;
      } else {
        Serial.printf("[networkTask] ERROR alert http.GET(): %d\n", httpCode);
      }
      http.end();
      lastUpdate = now;
    }

    vTaskDelay(pdMS_TO_TICKS(1000)); // Check every 1s
  }
}
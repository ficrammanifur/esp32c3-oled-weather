#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// --- Konfigurasi Hardware ---
#define DHTPIN 4
#define DHTTYPE DHT22
#define NTC_PIN 34
#define BUZZER_PIN 15

// Pin Motor Stepper (28BYJ-48 dengan ULN2003)
#define STEPPER_IN1 13
#define STEPPER_IN2 12
#define STEPPER_IN3 14
#define STEPPER_IN4 27

// Konfigurasi OLED SSD1306 I2C (128x64)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_SDA 21
#define OLED_SCL 22

// Inisialisasi objek DHT dan OLED
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Sumber Daya FreeRTOS ---
SemaphoreHandle_t dataMutex;
SemaphoreHandle_t oledMutex;

// Struktur data sensor (dilindungi mutex)
struct SensorData {
  float dht;
  float ntc;
  bool dhtValid;
  bool ntcValid;
  float filtered;
} sensorData;

// Parameter Kalman Filter 1D
struct KalmanFilter {
  float estimate;
  float error;
  float processNoise;
  float measurementNoise;
} kalman = {25.0, 1.0, 0.01, 0.5};

// Urutan langkah stepper (full-step untuk 28BYJ-48)
const uint8_t stepSequence[4][4] = {
  {1, 0, 1, 0},
  {0, 1, 1, 0},
  {0, 1, 0, 1},
  {1, 0, 0, 1}
};
volatile int currentStep = 0;

// Prototipe fungsi
void sensorTask(void* pvParameters);
void filterTask(void* pvParameters);
void displayTask(void* pvParameters);
void actuatorTask(void* pvParameters);
float readNTC_Celsius(int pin);
float kalmanUpdate(float measurement);
void stepperMove(bool forward);
void stepperStop();

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Konfigurasi pin output
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(STEPPER_IN1, OUTPUT);
  pinMode(STEPPER_IN2, OUTPUT);
  pinMode(STEPPER_IN3, OUTPUT);
  pinMode(STEPPER_IN4, OUTPUT);
  stepperStop();

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);  // Tambahan: set atenuasi ADC untuk range 0-3.3V

  // Inisialisasi DHT22
  dht.begin();

  // Inisialisasi I2C dan OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED init failed!"));
    while (1);
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Initializing..."));
  display.display();

  // Buat mutex
  dataMutex = xSemaphoreCreateMutex();
  oledMutex = xSemaphoreCreateMutex();
  
  if (dataMutex == NULL || oledMutex == NULL) {
    Serial.println(F("Mutex creation failed!"));
    while (1);
  }

  // Inisialisasi data sensor
  sensorData.dht = NAN;
  sensorData.ntc = NAN;
  sensorData.dhtValid = false;
  sensorData.ntcValid = false;
  sensorData.filtered = 25.0;

  // Buat task FreeRTOS
  xTaskCreatePinnedToCore(sensorTask, "Sensor", 4096, NULL, 3, NULL, 1);
  xTaskCreatePinnedToCore(filterTask, "Filter", 4096, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(displayTask, "Display", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(actuatorTask, "Actuator", 4096, NULL, 2, NULL, 1);

  Serial.println(F("System started - All tasks running"));
}

void loop() {
  // Loop kosong - semua dihandle oleh FreeRTOS tasks
  vTaskDelay(pdMS_TO_TICKS(10000));
}

// --- TASK IMPLEMENTATIONS ---

// Task 1: Pembacaan Sensor
void sensorTask(void* pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  const TickType_t frequency = pdMS_TO_TICKS(2000);

  for (;;) {
    // Baca DHT22
    float dhtTemp = dht.readTemperature();
    bool dhtValid = !isnan(dhtTemp) && (dhtTemp > -40 && dhtTemp < 80);

    // Baca NTC
    float ntcTemp = readNTC_Celsius(NTC_PIN);
    bool ntcValid = !isnan(ntcTemp) && (ntcTemp > -50 && ntcTemp < 150);

    // Update data dengan mutex protection
    if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      sensorData.dht = dhtTemp;
      sensorData.ntc = ntcTemp;
      sensorData.dhtValid = dhtValid;
      sensorData.ntcValid = ntcValid;
      xSemaphoreGive(dataMutex);

      // Debug output
      Serial.printf("DHT: %.2f°C [%s] | NTC: %.2f°C [%s]\n",
                    dhtTemp, dhtValid ? "OK" : "FAIL",
                    ntcTemp, ntcValid ? "OK" : "FAIL");
    }

    vTaskDelayUntil(&lastWakeTime, frequency);
  }
}

// Task 2: Filter Kalman
void filterTask(void* pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  const TickType_t frequency = pdMS_TO_TICKS(500);

  for (;;) {
    float dhtVal, ntcVal;
    bool dhtOk, ntcOk;

    // Baca data sensor
    if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      dhtVal = sensorData.dht;
      ntcVal = sensorData.ntc;
      dhtOk = sensorData.dhtValid;
      ntcOk = sensorData.ntcValid;
      xSemaphoreGive(dataMutex);

      // Gabungkan sensor valid
      float combined = 0.0;
      int count = 0;

      if (dhtOk) {
        combined += dhtVal;
        count++;
      }
      if (ntcOk) {
        combined += ntcVal;
        count++;
      }

      // Terapkan filter jika ada data valid
      if (count > 0) {
        combined /= count;
        float filtered = kalmanUpdate(combined);

        // Simpan hasil filter
        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
          sensorData.filtered = filtered;
          xSemaphoreGive(dataMutex);
          Serial.printf("Filtered: %.2f°C\n", filtered);
        }
      }
    }

    vTaskDelayUntil(&lastWakeTime, frequency);
  }
}

// Task 3: Update Display OLED
void displayTask(void* pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  const TickType_t frequency = pdMS_TO_TICKS(500);

  for (;;) {
    float temp = 0.0;

    // Baca suhu filtered
    if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      temp = sensorData.filtered;
      xSemaphoreGive(dataMutex);
    }

    // Update display
    if (xSemaphoreTake(oledMutex, pdMS_TO_TICKS(200)) == pdTRUE) {
      display.clearDisplay();
      
      // Title
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println(F("Temperature Monitor"));
      display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);

      // Temperature value (large)
      display.setTextSize(3);
      display.setCursor(10, 20);
      display.printf("%.1f", temp);
      
      display.setTextSize(2);
      display.setCursor(95, 25);
      display.print(F("C"));

      // Status
      display.setTextSize(1);
      display.setCursor(0, 55);
      if (temp > 25.0) {
        display.print(F("Status: COOLING ACTIVE"));
      } else {
        display.print(F("Status: NORMAL"));
      }

      display.display();
      xSemaphoreGive(oledMutex);
    }

    vTaskDelayUntil(&lastWakeTime, frequency);
  }
}

// Task 4: Kontrol Aktuator (Buzzer & Stepper)
void actuatorTask(void* pvParameters) {
  bool motorDirection = true;
  int stepCounter = 0;
  const int STEPS_PER_CYCLE = 50;
  bool wasHot = false;

  for (;;) {
    float temp = 0.0;

    // Baca suhu
    if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      temp = sensorData.filtered;
      xSemaphoreGive(dataMutex);
    }

    bool isHot = (temp > 25.0);

    if (isHot) {
      // Aktifkan buzzer
      if (!wasHot) {
        digitalWrite(BUZZER_PIN, HIGH);
        Serial.println(F("ALERT: Temperature HIGH - Cooling activated"));
      }

      // Gerakkan stepper
      stepperMove(motorDirection);
      stepCounter++;

      // Ganti arah setiap STEPS_PER_CYCLE
      if (stepCounter >= STEPS_PER_CYCLE) {
        stepCounter = 0;
        motorDirection = !motorDirection;
      }

      wasHot = true;
      vTaskDelay(pdMS_TO_TICKS(20));  // Kecepatan motor

    } else {
      // Matikan buzzer dan stepper
      if (wasHot) {
        digitalWrite(BUZZER_PIN, LOW);
        stepperStop();
        Serial.println(F("Temperature NORMAL - Cooling stopped"));
      }
      
      wasHot = false;
      stepCounter = 0;
      vTaskDelay(pdMS_TO_TICKS(200));
    }
  }
}

// --- UTILITY FUNCTIONS ---

// Baca NTC Thermistor
float readNTC_Celsius(int pin) {
  const int SAMPLES = 10;
  int sum = 0;
  
  // Ambil rata-rata beberapa pembacaan
  for (int i = 0; i < SAMPLES; i++) {
    sum += analogRead(pin);
    vTaskDelay(pdMS_TO_TICKS(5));
  }
  
  float raw = sum / (float)SAMPLES;
  float voltage = (raw / 4095.0) * 3.3;
  
  const float R_FIXED = 10000.0;
  const float BETA = 3950.0;
  const float T0 = 298.15;  // 25°C dalam Kelvin
  const float R0 = 10000.0;
  
  if (voltage >= 3.29 || voltage <= 0.01) {
    return NAN;
  }
  
  float rNTC = (voltage * R_FIXED) / (3.3 - voltage);
  
  if (rNTC <= 0) {
    return NAN;
  }
  
  float tempK = 1.0 / ((1.0 / T0) + (1.0 / BETA) * log(rNTC / R0));
  return tempK - 273.15;
}

// Kalman Filter Update
float kalmanUpdate(float measurement) {
  // Prediction
  kalman.error += kalman.processNoise;
  
  // Kalman Gain
  float gain = kalman.error / (kalman.error + kalman.measurementNoise);
  
  // Update
  kalman.estimate = kalman.estimate + gain * (measurement - kalman.estimate);
  kalman.error = (1.0 - gain) * kalman.error;
  
  return kalman.estimate;
}

// Gerakkan stepper satu langkah
void stepperMove(bool forward) {
  if (forward) {
    currentStep = (currentStep + 1) % 4;
  } else {
    currentStep = (currentStep - 1 + 4) % 4;
  }
  
  digitalWrite(STEPPER_IN1, stepSequence[currentStep][0]);
  digitalWrite(STEPPER_IN2, stepSequence[currentStep][1]);
  digitalWrite(STEPPER_IN3, stepSequence[currentStep][2]);
  digitalWrite(STEPPER_IN4, stepSequence[currentStep][3]);
}

// Hentikan stepper motor
void stepperStop() {
  digitalWrite(STEPPER_IN1, LOW);
  digitalWrite(STEPPER_IN2, LOW);
  digitalWrite(STEPPER_IN3, LOW);
  digitalWrite(STEPPER_IN4, LOW);
}

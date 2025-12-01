#include <Wire.h>
#include <Adafruit_TCS34725.h>

// Inisialisasi sensor (integration time & gain disesuaikan)
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_50MS,
  TCS34725_GAIN_4X
);

// Buffer untuk moving average
const int SAMPLES = 10;
uint32_t r_buffer[SAMPLES] = {0};
uint32_t g_buffer[SAMPLES] = {0};
uint32_t b_buffer[SAMPLES] = {0};
uint32_t c_buffer[SAMPLES] = {0};

int index = 0;
bool buffer_full = false;

void setup() {
  Serial.begin(115200);

  if (!tcs.begin()) {
    Serial.println("Sensor TCS34725 tidak ditemukan!");
    while (1);
  }

  Serial.println("TCS34725 OK — Mode FIXED Activated");
}

void loop() {
  uint16_t r, g, b, c;

  // Ambil raw data sensor
  tcs.getRawData(&r, &g, &b, &c);

  // Masukkan raw ke buffer
  r_buffer[index] = r;
  g_buffer[index] = g;
  b_buffer[index] = b;
  c_buffer[index] = c;

  index++;
  if (index >= SAMPLES) {
    index = 0;
    buffer_full = true;
  }

  // Hitung rata-rata
  uint32_t sum_r = 0, sum_g = 0, sum_b = 0, sum_c = 0;
  int count = buffer_full ? SAMPLES : index;

  for (int i = 0; i < count; i++) {
    sum_r += r_buffer[i];
    sum_g += g_buffer[i];
    sum_b += b_buffer[i];
    sum_c += c_buffer[i];
  }

  float avg_r = sum_r / count;
  float avg_g = sum_g / count;
  float avg_b = sum_b / count;
  float avg_c = sum_c / count;

  // -----------------------------
  // ⭐ FIX PALING PENTING:
  // Normalisasi dengan CLEAR channel
  // -----------------------------
  float r_norm = (avg_r / avg_c) * 255.0;
  float g_norm = (avg_g / avg_c) * 255.0;
  float b_norm = (avg_b / avg_c) * 255.0;

  // Batasi rentang nilai
  r_norm = constrain(r_norm, 0, 255);
  g_norm = constrain(g_norm, 0, 255);
  b_norm = constrain(b_norm, 0, 255);

  // Print hasil
  Serial.print("RGB FIX = ");
  Serial.print((int)r_norm); Serial.print(", ");
  Serial.print((int)g_norm); Serial.print(", ");
  Serial.println((int)b_norm);

  delay(100);
}

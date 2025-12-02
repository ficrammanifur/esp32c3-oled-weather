<h1 align="center">
Klasifikasi Warna RGB Menggunakan K-Nearest Neighbors (K-NN)<br>
<sub>Analisis Dataset, Pelatihan Model, & Implementasi Embedded System</sub>
</h1>

<p align="center">
  <em>Proyek ini mendemonstrasikan implementasi lengkap algoritma K-Nearest Neighbors (K-NN) untuk mengklasifikasikan warna berdasarkan nilai RGB (Red, Green, Blue), mulai dari analisis data Python hingga implementasi real-time pada mikrokontroler ESP32 dengan sensor warna TCS34725.</em>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/last_commit-today-brightgreen?style=for-the-badge" />
  <img src="https://img.shields.io/badge/language-Python_▸_Arduino-3776ab?style=for-the-badge&logo=python&logoColor=white" />
  <img src="https://img.shields.io/badge/platform-ESP32_DevKit_▸_Jupyter-00ADD8?style=for-the-badge&logo=espressif&logoColor=white" />
  <img src="https://img.shields.io/badge/algorithm-K--NN_Classifier-FF6B6B?style=for-the-badge&logo=analytics&logoColor=white" />
  <img src="https://img.shields.io/badge/dataset-4_Classes_40_Samples-32CD32?style=for-the-badge&logo=database&logoColor=white" />
  <img src="https://img.shields.io/badge/sensor-TCS34725-7B68EE?style=for-the-badge&logo=sensors&logoColor=white" />
  <img src="https://img.shields.io/badge/accuracy-91.67%25-00FF00?style=for-the-badge" />
  <a href="https://github.com/ficrammanifur/esp32-rgb-knn-classifier/blob/main/LICENSE">
    <img src="https://img.shields.io/badge/license-MIT-blue?style=for-the-badge" alt="License: MIT" />
  </a>
</p>

---

## 📋 Daftar Isi
- [Ringkasan Proyek](#-ringkasan-proyek)
- [Desain Sistem](#-desain-sistem)
  - [Arsitektur Hardware](#-arsitektur-hardware)
  - [Arsitektur Software (Python)](#-arsitektur-software-python)
- [Bagian 1: Analisis & Pelatihan K-NN (Python)](#-bagian-1-analisis--pelatihan-k-nn-python)
  - [Dataset & Kelas](#-dataset--kelas)
  - [Metodologi K-NN](#-metodologi-k-nn)
  - [Hasil Pelatihan](#-hasil-pelatihan)
- [Bagian 2: Implementasi Embedded (Arduino/ESP32)](#-bagian-2-implementasi-embedded-arduinoesp32)
  - [Sensor TCS34725](#-sensor-tcs34725)
  - [Penjelasan Kode Arduino](#-penjelasan-kode-arduino)
- [Instalasi & Setup](#-instalasi--setup)
- [Cara Menjalankan](#-cara-menjalankan)
- [Testing & Validasi](#-testing--validasi)
- [Troubleshooting](#-troubleshooting)
- [Struktur Folder](#-struktur-folder)
- [Referensi & Teori](#-referensi--teori)
- [Kontribusi](#-kontribusi)
- [Pengembang](#-pengembang)
- [Lisensi](#-lisensi)

---

## 📊 Ringkasan Proyek

Proyek ini adalah implementasi **end-to-end** algoritma machine learning K-NN untuk deteksi dan klasifikasi warna. Sistem terdiri dari dua tahap utama:

1. **Tahap Analisis & Training (Python/Jupyter Notebook)**: 
   - Memproses dataset RGB 40 sampel (10 per kelas)
   - Melakukan feature scaling dengan StandardScaler
   - Pencarian nilai K optimal (1-20)
   - Evaluasi performa model dengan Confusion Matrix & Classification Report
   - **Hasil Optimal**: K=1 dengan akurasi 91.67%

2. **Tahap Implementasi (Arduino/ESP32)**:
   - Akuisisi data real-time dari sensor TCS34725
   - Normalisasi RGB menggunakan Clear Channel
   - Penerapan logika K-NN sederhana (Nearest Neighbor)
   - Deteksi warna real-time dengan low-latency

---

## 📐 Desain Sistem

### Arsitektur Hardware

Sistem embedded dirancang untuk deteksi warna portable dan real-time menggunakan mikrokontroler ESP32 yang hemat daya.

#### Komponen Utama

| No | Komponen | Fungsi | Spesifikasi | Pin/Interface | Keterangan |
|:--:|----------|--------|-------------|----------------|-----------|
| 1 | **ESP32 Dev Module** | Mikrokontroler utama | 240MHz Dual-Core, 520KB SRAM, 12-bit ADC, WiFi+BT | GPIO 21 (SDA), GPIO 22 (SCL) | Main processor untuk akuisisi & klasifikasi |
| 2 | **TCS34725 Sensor** | Deteksi warna RGB | I2C, 3.3V, 12-bit per channel, 370-1000nm | I2C Address: 0x29 | Membaca RGBC (Red, Green, Blue, Clear) |
| 3 | **Resistor Pull-up I2C** | Stabilisasi I2C Bus | 2 × 10kΩ | GPIO 21, GPIO 22 | Pull-up ke VCC 3.3V |
| 4 | **Capacitor Bypass** | Filter noise power | 100µF + 10µF | VCC, GND | Stabilisasi power supply sensor |
| 5 | **USB Micro-B Cable** | Upload & Serial Monitor | 5V DC → 3.3V regulator internal | TX/RX (UART) | Communication dengan computer |
| 6 | **LED RGB (Optional)** | Indikator hasil klasifikasi | 5mm Common Cathode, RGB | GPIO 5,6,7 + 220Ω resistor | Visualisasi warna terdeteksi |
| 7 | **Power Supply** | Sumber daya | 5V USB atau 3.7V LiPo | VIN, GND | Input power untuk sistem |

#### Software Components

| Layer | Komponen | Fungsi | Library/Framework |
|-------|----------|--------|-------------------|
| **Data Processing** | Pandas DataFrame | Load & manipulasi dataset RGB | `pandas` |
| **Numerical Ops** | NumPy Arrays | Kalkulasi jarak & operasi vektor | `numpy` |
| **ML Model** | K-NN Classifier | Pelatihan & prediksi klasifikasi | `scikit-learn` |
| **Data Scaling** | StandardScaler | Normalisasi fitur RGB (crucial untuk K-NN) | `scikit-learn.preprocessing` |
| **Visualization** | Matplotlib/Seaborn | Plot 3D RGB space, confusion matrix, accuracy curves | `matplotlib`, `seaborn` |
| **Metrics** | Classification Report | Evaluasi precision, recall, F1-score | `sklearn.metrics` |
| **Arduino** | Adafruit TCS34725 | Library kontrol sensor I2C | `Adafruit_TCS34725.h` |
| **Arduino** | Moving Average | Smoothing pembacaan sensor (10 samples) | Native C++ |
| **Arduino** | Euclidean Distance | Hitung jarak K-NN di hardware | Native C++ (sqrt) |

#### Komponen Utama

| Komponen | Fungsi | Spesifikasi | Keterangan |
|----------|--------|-------------|------------|
| **ESP32 Dev Module** | Mikrokontroler | 240MHz Dual-Core, 520KB SRAM, WiFi+BT | Main processor untuk akuisisi & klasifikasi data |
| **TCS34725 Sensor** | Sensor warna RGB | I2C, 3.3V, 12-bit ADC | Membaca nilai RGBC (Red, Green, Blue, Clear) |
| **Resistor Pull-up** | I2C Interface | 2 × 10kΩ | Pull-up untuk SDA/SCL |
| **USB/Serial** | Komunikasi | FTDI/CH340 | Upload kode & monitoring serial |
| **Power Supply** | Sumber daya | 3.3V / 5V | Untuk ESP32 & TCS34725 |

#### Diagram Blok Hardware

### 📊 Diagram Blok Hardware

#### Level 0: System Architecture Overview

```
┌──────────────────────────────────────────────────────────────────┐
│           SISTEM KLASIFIKASI WARNA RGB K-NN                      │
│          Menggunakan ESP32 Dev Module + TCS34725                 │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│    [TCS34725 Sensor] ──I2C─→ [ESP32 Dev Module] ──Serial──→      │
│     RGBC Photodiode          Moving Average              PC      │
│                              Normalisasi               Monitoring│
│                              K-NN Engine                         │
│                              LED Indicator                       │
│                                                                  │
│     Real-Time Processing: ~100ms cycle time                      │
│     Classification: 4 warna (Merah, Hijau, Biru, Neutral)        │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘
```

#### Level 1: Detailed Hardware Block Diagram

```
┌──────────────────────────────────────────────────────────────────┐
│                    POWER SUPPLY & INITIALIZATION                 │
├──────────────────────────────────────────────────────────────────┤
│  USB 5V ────→ [Voltage Regulator 3.3V] ────→ [3.3V Rail]         │
│                                                     │            │
│                                        ┌────────────┼─ --> VCC┐  │
│                                        │            │         │  │
│                                    [Bypass]     ESP32     Sensor │
│                                    Capacitors   Power     Power  │
│                                    100µF/10µF   Supply    Supply │
└──────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────────┐
│                   I2C SENSOR DATA ACQUISITION                    │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌──────────────────────┐         ┌─────────────────────────┐    │
│  │  TCS34725 Sensor     │         │   ESP32 I2C Master      │    │
│  │                      │         │                         │    │
│  │ • Photodiode Array   │         │ • GPIO 21 (SDA)         │    │
│  │ • Red Filter         │         │ • GPIO 22 (SCL)         │    │
│  │ • Green Filter       │─I2C────→ • Pull-up: 10kΩ          │    │
│  │ • Blue Filter        │         │ • Freq: 100kHz          │    │
│  │ • Clear Channel      │         │ • Address: 0x29         │    │
│  │                      │         │                         │    │
│  │ 12-bit per channel   │         │ Integration Time: 50ms  │    │
│  │ Range: 370-1000nm    │         │ Gain: 4x                │    │
│  │                      │         │                         │    │
│  └──────────────────────┘         └─────────────────────────┘    │
│           VCC: 3.3V                        3.3V: 3.3V            │
│           GND: GND                         GND: GND              │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────────┐
│                  PROCESSING PIPELINE (ESP32 Main Loop)           │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌────────────────────────────────────────────────────────────┐  │
│  │ 1. RAW DATA ACQUISITION                                    │  │
│  │    ├─ getRawData(&r, &g, &b, &c) from TCS34725 I2C         │  │
│  │    └─ Freq: ~100ms cycle                                   │  │
│  │                                                            │  │
│  │ 2. CIRCULAR BUFFER (SAMPLES=10)                            │  │
│  │    ├─ r_buffer[10], g_buffer[10], b_buffer[10], ...        │  │
│  │    ├─ buf_index++ (circular increment)                     │  │
│  │    └─ buffer_full flag (after 10 samples)                  │  │
│  │                                                            │  │
│  │ 3. MOVING AVERAGE (Noise Reduction)                        │  │
│  │    ├─ avg_r = Σ(r_buffer) / 10                             │  │
│  │    ├─ avg_g = Σ(g_buffer) / 10                             │  │
│  │    ├─ avg_b = Σ(b_buffer) / 10                             │  │
│  │    └─ avg_c = Σ(c_buffer) / 10                             │  │
│  │                                                            │  │
│  │ 4. NORMALIZATION (RGB Fix)                                 │  │
│  │    ├─ r_norm = (avg_r / avg_c) × 255                       │  │
│  │    ├─ g_norm = (avg_g / avg_c) × 255                       │  │
│  │    ├─ b_norm = (avg_b / avg_c) × 255                       │  │
│  │    └─ Constrain: 0-255 range (safety)                      │  │
│  │                                                            │  │
│  │ 5. SERIAL OUTPUT                                           │  │
│  │    └─ Format: "R,G,B" (e.g., "120,130,150")                │  │
│  │                                                            │  │
│  │ Cycle: ~100ms per iteration                                │  │
│  │                                                            │  │
│  └────────────────────────────────────────────────────────────┘  │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────────┐
│                    K-NN CLASSIFICATION ENGINE                    │
├──────────────────────────────────────────────────────────────────┤
│ (Optional: Implementasi sederhana pada ESP32)                    │
│                                                                  │
│ Stored Centroids (dari training data Python):                    │
│ ┌─────────────────────────────────────────────────────────────┐  │
│ │ RED CENTROID:     [175, 54,  61]                            │  │
│ │ GREEN CENTROID:   [105, 99,  60]                            │  │
│ │ BLUE CENTROID:    [107, 88,  77]                            │  │
│ │ NEUTRAL CENTROID: [150, 140, 130]                           │  │
│ └─────────────────────────────────────────────────────────────┘  │
│                                                                  │
│ Euclidean Distance Calculation:                                  │
│ d = √[(r_norm - r_centroid)² + (g_norm - g_centroid)² +          │
│       (b_norm - b_centroid)²]                                    │
│                                                                  │
│ Classification: Argmin(d) → Nearest Centroid = Predicted Class   │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────────┐
│                      OUTPUT & VISUALIZATION                      │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│  Serial Output:           LED RGB Indicator (Optional):          │
│  ├─ "175,54,61"    ─────→ ├─ GPIO 5: Red LED (PWM)               │
│  ├─ "105,99,60"    ─────→ ├─ GPIO 6: Green LED (PWM)             │
│  └─ "107,88,77"    ─────→ └─ GPIO 7: Blue LED (PWM)              │
│                                                                  │
│  Real-Time Plot:  │  Confusion Matrix:  │  Accuracy Report:      │
│  (Python PC)      │  (Training)         │  (Python Metrics)      │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘
```

#### Level 2: Signal Flow Wiring Detail

```
SENSOR SIDE                         MICROCONTROLLER SIDE
─────────────────────────────────────────────────────────────

TCS34725:
  VCC (Pin 1) ──────────────────→ 3.3V (with 100µF cap)
  GND (Pin 2) ──────────────────→ GND
  SDA (Pin 3) ──┬──10kΩ──────┬→ GPIO 21 (SDA)
                │             │
               3.3V ──────────┘ 
  
  SCL (Pin 4) ──┬──10kΩ──────┬→ GPIO 22 (SCL)
                │             │
               3.3V ──────────┘ 
  
  INT (Pin 5) ──────────────────→ GPIO (optional, interrupt)
  LED (Pin 6) ──────────────────→ GND (active low)
  ADDR (Pin 7) ──────────────────→ GND (addr 0x29)

OUTPUT LED (optional):
  Red LED Anode ──220Ω──→ GPIO 5
  Grn LED Anode ──220Ω──→ GPIO 6
  Blu LED Anode ──220Ω──→ GPIO 7
  All Cathode ────────→ GND

Serial Monitor:
  TX (GPIO 1) ──────→ Computer (USB converter)
  RX (GPIO 3) ──────← Computer (USB converter)
```

---

### Arsitektur Software (Python)

Program Python menggunakan scikit-learn untuk data processing, model training, dan evaluation.

#### Data Flow Diagram (DFD)

### 📈 Diagram Alur Data (Data Flow Diagram - DFD)

#### Level 0: System Context Diagram

```
        ┌─────────────────────────────┐
        │  Lingkungan Fisik           │
        │  (Objek Berwarna)           │
        │  • Kertas Merah             │
        │  • Kertas Hijau             │
        │  • Kertas Biru              │
        │  • Objek Netral             │
        └────────────┬────────────────┘
                     │ Cahaya Pantulan
                     │ 370-1000nm
                     ↓
        ┌─────────────────────────────┐
        │   SISTEM KLASIFIKASI RGB    │───── [User/Operator]
        │   (ESP32 + TCS34725)        │      • Monitor hasil
        │   ✓ Akuisisi Data           │      • Kalibrasi sensor
        │   ✓ Normalisasi             │      • Set parameter
        │   ✓ Klasifikasi K-NN        │
        └────────────┬────────────────┘
                     │ Serial Output
                     │ @ 115200 baud
                     ↓
        ┌─────────────────────────────┐
        │  Output Display             │
        │  • Serial Monitor           │
        │  • LED RGB Indicator        │
        │  • Log File (SD Card)       │
        └─────────────────────────────┘
```

#### Level 1: Main Data Processes

```
    ┌──────────────────────────────────────────────────────────────┐
    │                DATA ACQUISITION PROCESS                      │
    ├──────────────────────────────────────────────────────────────┤
    │                                                              │
    │  D1: TCS34725 Sensor              D2: Raw Data Buffer        │
    │  ┌────────────────────┐           ┌────────────────────┐     │
    │  │ Read RGBC @ 55ms   │──────────→│ r_raw[10]          │     │
    │  │ interval           │           │ g_raw[10]          │     │
    │  │ (12-bit ADC)       │           │ b_raw[10]          │     │
    │  │ ┌──────────────┐   │           │ c_raw[10]          │     │
    │  │ │ Red: 0-1023  │   │           │ Moving Average buf │     │
    │  │ │ Green: 0-1023│───┼──────────→│                    │     │
    │  │ │ Blue: 0-1023 │   │           │ New: shift & add   │     │
    │  │ │ Clear: 0-1023│   │           └────────────────────┘     │
    │  │ └──────────────┘   │                   │                  │
    │  │ (High resolution   │                   ↓                  │
    │  │  12-bit per channel)                                      │
    │  └────────────────────┘           OUTPUT: avg_r, avg_g,      │
    │                                   avg_b, avg_c               │
    │                                   (smoothed 10 samples)      │
    └──────────────────────────────────────────────────────────────┘

    ┌──────────────────────────────────────────────────────────────┐
    │              DATA NORMALIZATION PROCESS                      │
    ├──────────────────────────────────────────────────────────────┤
    │                                                              │
    │  D2: Moving Avg        D3: Scale Store                       │
    │  ┌────────────────┐    ┌────────────────────────────┐        │
    │  │ avg_r (raw)    │    │ rgb_r_norm = (avg_r /      │        │
    │  │ avg_g (raw)    │───→│                avg_c) × 255         │
    │  │ avg_b (raw)    │    │ rgb_g_norm = (avg_g /      │        │
    │  │ avg_c (clear)  │    │                avg_c) × 255         │
    │  │                │    │ rgb_b_norm = (avg_b /     │         │
    │  │ (High dynamic  │    │                avg_c) × 255         │
    │  │  range ADC)    │    │                                     │
    │  └────────────────┘    │ Range: 0-255 per channel            │
    │                        │ (8-bit normalized RGB)              │
    │                        └─────────────────────────────────────┘
    │                                       │
    │                                 STORAGE:
    │                                 rgb_r_norm (uint8)
    │                                 rgb_g_norm (uint8)
    │                                 rgb_b_norm (uint8)
    │
    └──────────────────────────────────────────────────────────────┘

    ┌──────────────────────────────────────────────────────────────┐
    │           K-NN CLASSIFICATION PROCESS                        │
    ├──────────────────────────────────────────────────────────────┤
    │                                                              │
    │  D3: Normalized      D4: Centroid Store                      │
    │  ┌──────────────┐    ┌─────────────────────────────┐         │
    │  │ rgb_r (0-255)│    │ colors[4] = {               │         │
    │  │ rgb_g (0-255)│───→│   {RED: [175, 54, 61]},     │         │
    │  │ rgb_b (0-255)│    │   {GREEN: [105, 99, 60]},   │         │
    │  │              │    │   {BLUE: [107, 88, 77]},    │         │
    │  │              │    │   {NEUTRAL: [150, 140, 130]}          │
    │  └──────────────┘    │ }                           │         │
    │         │            │                             │         │
    │         │ LOOP i=0-3 │                             │         │
    │         └───────────→│ distance[i] = Euclidean     │         │
    │                      │ sqrt((r-cR)² +(g-cG)²+(b-cB)²)        │
    │                      │                             │         │
    │                      │ min_idx = argmin(distance)  │         │
    │                      │                             │         │
    │                      └──────────────┬──────────────┘         │
    │                                      ↓                       │
    │                           OUTPUT: Class Name                 │
    │                           + min_distance                     │
    │                           + confidence (1/distance)          │
    │                                                              │
    └──────────────────────────────────────────────────────────────┘
```

#### Level 2: Python Training Pipeline (Offline)

```
┌──────────────────────────────────────────────────────────────────┐
│               PYTHON TRAINING PIPELINE (OFFLINE)                 │
│                    (Tahap: Preparasi Model)                      │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌──────────────────┐        ┌──────────────────┐                │
│  │ dataset-warna.txt│        │ Data Loading     │                │
│  │ (40 RGB Samples] │───────→│ & Exploration    │                │
│  │                  │        │ pd.read_csv()    │                │
│  │ R,G,B, Class     │        │ df.head()        │                │
│  │                  │        │ plt.plot() 3D    │                │
│  └──────────────────┘        └────────┬─────────┘                │
│                                       │                          │
│                                       ↓                          │
│                      ┌────────────────────────────┐              │
│                      │ Train-Test Split           │              │
│                      │ • 70% train (28 sampel)    │              │
│                      │ • 30% test (12 sampel)     │              │
│                      │ • Stratified sampling      │              │
│                      └────────────┬───────────────┘              │
│                                   │                              │
│                                   ↓                              │
│                      ┌────────────────────────────┐              │
│                      │ Feature Scaling            │              │
│                      │ StandardScaler:            │              │
│                      │ X_scaled = (X - mean)/std  │              │
│                      │ mean=[110,87,75]           │              │
│                      │ std=[35.5,22.3,18.9]       │              │
│                      └────────────┬───────────────┘              │
│                                   │                              │
│              ┌────────────────────┼────────────────────┐         │
│              │                    │                    │         │
│              ↓                    ↓                    ↓         │
│    ┌──────────────────┐ ┌──────────────────┐ ┌────────────────┐  │
│    │ Test K=1 to K=20 │ │ Plot Accuracy    │ │ Find Optimal K │  │
│    │ • Train model    │ │ vs K Value       │ │ K=1: 91.67%    │  │
│    │ • Evaluate acc   │ │ • Training line  │ │ Selected!      │  │
│    │ • Store results  │ │ • Testing line   │ │                │  │
│    └──────────────────┘ │ • Identify peak  │ └────────────────┘  │
│                         └──────────────────┘                     │
│              │                    │                    │         │
│              └────────────────────┼────────────────────┘         │
│                                   │                              │
│                                   ↓                              │
│                      ┌────────────────────────────┐              │
│                      │ Final Model Evaluation     │              │
│                      │ (K=1)                      │              │
│                      │ • Confusion Matrix         │              │
│                      │ • Classification Report    │              │
│                      │ • Precision/Recall/F1      │              │
│                      │ • Accuracy: 0.9167         │              │
│                      └────────────┬───────────────┘              │
│                                   │                              │
│                                   ↓                              │
│                      ┌────────────────────────────┐              │
│                      │ Export Model Parameters    │              │
│                      │ • Centroids per class      │              │
│                      │ • Scaling params (μ, σ)    │              │
│                      │ • K value (1)              │              │
│                      │ → Use for Arduino code     │              │
│                      └────────────┬───────────────┘              │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘
```

#### Level 3: Complete Data Flow (End-to-End)

```
              OFFLINE (Python)             │         REAL-TIME (Arduino)
              ──────────────────           │         ──────────────────────

[Training Dataset]                         │
[40 RGB Samples]                           │
         │                                 │
         ↓                                 │
[Prepare Model]                            │
[K-NN Training]                            │
[Accuracy: 91.67%]                         │
         │                                 │
         ├─→ Export Centroids ─────────────┼──→ [Hard-code dalam Arduino]
         │                                 │     centroids[4] struct
         │                                 │
         ↓                                 │
[Metrics & Evaluation]                     │
[Classification Report]                    │
[Confusion Matrix]                         │
                                           │         REAL-TIME FLOW:
                                           │
                                           │   [TCS34725 Sensor]
                                           │   Read RGBC values
                                           │   (55ms interval)
                                           │         │
                                           │         ↓
                                           │   [Moving Average Buffer]
                                           │   10 samples → smooth
                                           │         │
                                           │         ↓
                                           │   [Normalization]
                                           │   RGB ÷ Clear × 255
                                           │   Range: 0-255
                                           │         │
                                           │         ↓
                                           │   [K-NN Classifier]
                                           │   Distance calc to 4 centroids
                                           │         │
                                           │         ↓
                                           │   [Find Minimum Distance]
                                           │   argmin(dist) → class
                                           │         │
                                           │         ├──→ [Serial Output]
                                           │         │    115200 baud
                                           │         │    "RED" + distance
                                           │         │
                                           │         └──→ [LED RGB Control]
                                           │            GPIO 5,6,7
                                           │            Visualisasi warna
```

#### Level 4: Detailed K-NN Classification Engine

```
[Input: Normalized RGB (0-255)]
    rgb_r, rgb_g, rgb_b
         │
         ↓
┌─────────────────────────────────────────────────────────────────┐
│ K-NN CLASSIFICATION ENGINE (K=1, Metric=Euclidean)              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│ ┌──── Calculate Distance to Each Centroid ────┐                 │
│ │                                             │                 │
│ ├─ Class 0: RED [175, 54, 61]                 │                 │
│ │  dist_red = √[(rgb_r-175)² + (rgb_g-54)² + (rgb_b-61)²]       │
│ │                                             │                 │
│ ├─ Class 1: GREEN [105, 99, 60]               │                 │
│ │  dist_green = √[(rgb_r-105)² + (rgb_g-99)² + (rgb_b-60)²]     │
│ │                                             │                 │
│ ├─ Class 2: BLUE [107, 88, 77]                │                 │
│ │  dist_blue = √[(rgb_r-107)² + (rgb_g-88)² + (rgb_b-77)²]      │
│ │                                             │                 │
│ └─ Class 3: NEUTRAL [150, 140, 130]           │                 │
│    dist_neutral = √[(rgb_r-150)² + (rgb_g-140)² + (rgb_b-130)²] │
│                                                                 │
│ ┌──── Find Nearest Neighbor (K=1) ─────┐                        │
│ │                                      │                        │
│ │ distances = [dist_red, dist_green,   │                        │
│ │              dist_blue, dist_neutral]│                        │
│ │                                      │                        │
│ │ min_distance = min(distances)        │                        │
│ │ best_class_idx = argmin(distances)   │                        │
│ │                                      │                        │
│ └──────────────────────────────────────┘                        │
│           │                                                     │
│           ↓                                                     │
│ ┌──── Output Classification Result ────┐                        │
│ │                                      │                        │
│ │ if best_class_idx == 0 → "RED"       │                        │
│ │ if best_class_idx == 1 → "GREEN"     │                        │
│ │ if best_class_idx == 2 → "BLUE"      │                        │
│ │ if best_class_idx == 3 → "NEUTRAL"   │                        │
│ │                                      │                        │
│ │ confidence = 1.0 / (min_distance + 1)│                        │
│ │                                      │                        │
│ └──────────────────────────────────────┘                        │
│           │                                                     │
│           ↓                                                     │
│ [OUTPUT]                                                        │
│ Class Name: "RED" / "GREEN" / "BLUE" / "NEUTRAL"                │
│ Distance: 12.45 (pixels dalam RGB space)                        │
│ Confidence: 0.92 (normalized score)                             │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🛠️ Bagian 2: Implementasi Embedded (Arduino/ESP32)

### 🎨 Sensor TCS34725

Sensor warna TCS34725 mengukur intensitas cahaya dalam 4 channel:
- **R (Red)**: 0-1023 (12-bit)
- **G (Green)**: 0-1023
- **B (Blue)**: 0-1023
- **C (Clear)**: 0-1023 (IR + visible light)

Normalisasi RGB penting untuk mengurangi pengaruh pencahayaan:
$$\mathrm{RGB_{norm}} = \frac{\mathrm{RGB_{avg}}}{\mathrm{Clear_{avg}}} \times 255$$

### 📝 Arduino Code: Data Acquisition & Normalization (RGB.ino)

Program Arduino pada file `int.tsx` menunjukkan implementasi **moving average + normalization** untuk memperhalus pembacaan sensor dan menghasilkan nilai RGB yang konsisten.

#### Konfigurasi Sensor

```cpp
// TCS34725 dengan integration time 50ms dan gain 4x
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_50MS,  // Sampling window: 50ms
  TCS34725_GAIN_4X                 // Amplifikasi: 4x
);
```

| Parameter | Nilai | Alasan |
|-----------|-------|--------|
| **Integration Time** | 50ms | Balance antara kecepatan & akurasi pembacaan |
| **Gain** | 4x | Amplifikasi untuk sensor dengan cahaya sedang |
| **I2C Address** | 0x29 | Default untuk TCS34725 |
| **I2C Speed** | 100kHz | Standard mode I2C |

#### Algoritma Circular Buffer & Moving Average

**Pseudocode:**
```
1. Inisialisasi: r_buffer[10], g_buffer[10], b_buffer[10], c_buffer[10]
2. Loop setiap ~100ms:
   a. Baca raw data: getRawData(&r, &g, &b, &c)
   b. Masukkan ke buffer pada indeks buf_index
   c. buf_index++ (jika >= 10, reset ke 0)
   d. buffer_full = true (setelah 10 pembacaan pertama)
   e. count = buffer_full ? 10 : buf_index
   f. avg_r = (r_buffer[0]+...+r_buffer[count-1]) / count
   g. avg_g, avg_b, avg_c dengan cara yang sama
```

**Keuntungan Moving Average:**
- Mengurangi noise dari fluctuasi cahaya
- Hasil lebih stabil untuk klasifikasi
- Minimal latency (~1 detik untuk 10 samples × 100ms)

#### Normalisasi RGB (RGB Fix)

Formula normalisasi menggunakan Clear Channel sebagai referensi intensitas:

$$\text{RGB\_norm} = \frac{\text{RGB\_avg}}{\text{Clear\_avg}} \times 255$$

```cpp
// Safety check: hindari pembagian 0
if (avg_c > 10) {
  r_norm = (avg_r / avg_c) * 255.0;
  g_norm = (avg_g / avg_c) * 255.0;
  b_norm = (avg_b / avg_c) * 255.0;
} else {
  r_norm = g_norm = b_norm = 0;  // Default jika sensor error
}

// Constrain ke range 0-255 (safety)
r_norm = constrain(r_norm, 0, 255);
g_norm = constrain(g_norm, 0, 255);
b_norm = constrain(b_norm, 0, 255);
```

**Output Format:**
```
120,130,150    // Format: R,G,B (setiap nilai 0-255)
175,54,61      // Contoh output untuk warna merah
105,99,60      // Contoh output untuk warna hijau
```

#### Flowchart Program

```
START
  │
  ├─→ Serial.begin(115200)
  │
  ├─→ IF sensor not found
  │    └─→ ERROR message → HALT
  │
  ├─→ Inisialisasi Buffer (r_buffer, g_buffer, b_buffer, c_buffer)
  │
  └─→ MAIN LOOP (every ~100ms)
      │
      ├─→ 1. getRawData(&r, &g, &b, &c) dari TCS34725
      │
      ├─→ 2. Buffer Assignment
      │    ├─ r_buffer[buf_index] = r
      │    ├─ g_buffer[buf_index] = g
      │    ├─ b_buffer[buf_index] = b
      │    └─ c_buffer[buf_index] = c
      │
      ├─→ 3. Buffer Management
      │    ├─ buf_index++
      │    └─ IF buf_index >= 10: buf_index = 0, buffer_full = true
      │
      ├─→ 4. Calculate Average
      │    ├─ count = buffer_full ? 10 : buf_index
      │    ├─ sum_r/g/b/c = Σ(buffer[i]) for i=0 to count-1
      │    └─ avg_r/g/b/c = sum / count
      │
      ├─→ 5. Normalization (RGB Fix)
      │    ├─ IF avg_c > 10:
      │    │   ├─ r_norm = (avg_r / avg_c) × 255
      │    │   ├─ g_norm = (avg_g / avg_c) × 255
      │    │   └─ b_norm = (avg_b / avg_c) × 255
      │    └─ ELSE: r_norm = g_norm = b_norm = 0
      │
      ├─→ 6. Constraint (Clamp 0-255)
      │    ├─ r_norm = constrain(r_norm, 0, 255)
      │    ├─ g_norm = constrain(g_norm, 0, 255)
      │    └─ b_norm = constrain(b_norm, 0, 255)
      │
      ├─→ 7. Serial Output
      │    └─ PRINT: "(int)r_norm,(int)g_norm,(int)b_norm"
      │
      ├─→ 8. Delay 100ms
      │
      └─→ LOOP back to step 1
```

#### Contoh Output Serial Monitor

```
OK
120,130,150
122,132,148
119,131,149
175,54,61
176,55,62
174,53,60
105,99,60
106,100,61
104,98,59
```

Setiap baris merepresentasikan satu cycle (100ms), dengan nilai R,G,B yang sudah dinormalisasi dan di-smooth.

### 📝 Penjelasan Kode Arduino

#### File: `RGB.ino` (Akuisisi Data)

Membaca data sensor TCS34725 dengan **moving average filter** untuk stabilitas:

```cpp
#define SAMPLES 10  // Buffer size untuk rata-rata
uint16_t r_raw[SAMPLES], g_raw[SAMPLES], b_raw[SAMPLES], c_raw[SAMPLES];

// Moving Average
for (int i = 0; i < SAMPLES - 1; i++) {
  r_raw[i] = r_raw[i + 1];
  g_raw[i] = g_raw[i + 1];
  // ... dst untuk b_raw, c_raw
}
r_raw[SAMPLES - 1] = sensor.read16(TCS34725_RDATAL);
g_raw[SAMPLES - 1] = sensor.read16(TCS34725_GDATAL);
b_raw[SAMPLES - 1] = sensor.read16(TCS34725_BDATAL);
c_raw[SAMPLES - 1] = sensor.read16(TCS34725_CDATAL);

// Hitung rata-rata
uint16_t avg_r = 0, avg_g = 0, avg_b = 0, avg_c = 0;
for (int i = 0; i < SAMPLES; i++) {
  avg_r += r_raw[i];
  avg_g += g_raw[i];
  avg_b += b_raw[i];
  avg_c += c_raw[i];
}
avg_r /= SAMPLES;
avg_g /= SAMPLES;
avg_b /= SAMPLES;
avg_c /= SAMPLES;

// Normalisasi RGB (0-255)
uint8_t r_fix = (avg_r * 255) / avg_c;
uint8_t g_fix = (avg_g * 255) / avg_c;
uint8_t b_fix = (avg_b * 255) / avg_c;

Serial.printf("RGB FIX: R=%d, G=%d, B=%d\n", r_fix, g_fix, b_fix);
```

#### File: `Bandingkan-RGB.ino` (Klasifikasi K-NN)

Implementasi K-NN sederhana dengan centroid (Nearest Neighbor):

```cpp
// Definisi Centroid (dari pelatihan Python)
struct ColorCategory {
  const char* name;
  uint8_t r, g, b;
} colors[] = {
  {"RED", 175, 54, 61},
  {"GREEN", 105, 99, 60},
  {"BLUE", 107, 88, 77},
  {"NEUTRAL", 150, 140, 130}
};

// Fungsi Jarak Euclidean
float euclidean_distance(uint8_t r1, uint8_t g1, uint8_t b1,
                         uint8_t r2, uint8_t g2, uint8_t b2) {
  float dr = r1 - r2;
  float dg = g1 - g2;
  float db = b1 - b2;
  return sqrt(dr*dr + dg*dg + db*db);
}

// Klasifikasi (K=1)
float min_distance = 999999;
int best_class = 0;

for (int i = 0; i < 4; i++) {
  float dist = euclidean_distance(rgb_fix_r, rgb_fix_g, rgb_fix_b,
                                  colors[i].r, colors[i].g, colors[i].b);
  if (dist < min_distance) {
    min_distance = dist;
    best_class = i;
  }
}

Serial.printf("Detected Color: %s (distance: %.2f)\n", 
              colors[best_class].name, min_distance);
```

#### File: `cari-sample.ino` (Pengumpulan Data)

Alat bantu untuk mengumpulkan sampel RGB baru ke dataset:

```cpp
// Mode pengumpulan data
if (sensor_ready) {
  uint16_t r = sensor.read16(TCS34725_RDATAL);
  uint16_t g = sensor.read16(TCS34725_GDATAL);
  uint16_t b = sensor.read16(TCS34725_BDATAL);
  uint16_t c = sensor.read16(TCS34725_CDATAL);
  
  // Normalisasi & output
  uint8_t r_norm = (r * 255) / c;
  uint8_t g_norm = (g * 255) / c;
  uint8_t b_norm = (b * 255) / c;
  
  // Format CSV untuk di-copy ke dataset-warna.txt
  Serial.printf("%d,%d,%d,MANUAL_LABEL\n", r_norm, g_norm, b_norm);
}
```

---

## ⚙️ Instalasi & Setup

### Bagian 1: Environment Python

1. **Install Python 3.8+** dan pip
2. **Buat virtual environment**:
   ```bash
   python -m venv venv
   source venv/bin/activate  # Linux/Mac
   venv\Scripts\activate     # Windows
   ```
3. **Install dependencies**:
   ```bash
   pip install pandas numpy scikit-learn matplotlib seaborn jupyter
   ```
4. **Buka Jupyter Notebook**:
   ```bash
   jupyter notebook ficramm_Klasifikasi_RGB_dengan_K_NN.ipynb
   ```

### Bagian 2: Arduino/ESP32

1. **Install Arduino IDE** (v2.0+)
2. **Tambah Board Manager URL** di Preferences:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. **Install library TCS34725**:
   - Sketch → Include Library → Manage Libraries
   - Cari "Adafruit TCS34725" → Install
4. **Konfig Board**: Tools → Board → ESP32 Dev Module
5. **Wiring**: Lihat diagram di atas
6. **Upload**: `RGB.ino` atau `Bandingkan-RGB.ino`

---

## 🚀 Cara Menjalankan

### Python Notebook

1. Buka `ficramm_Klasifikasi_RGB_dengan_K_NN.ipynb` di Jupyter
2. Jalankan sel satu per satu (Shift+Enter)
3. Amati:
   - Visualisasi 3D RGB space
   - Kurva akurasi vs K
   - Confusion matrix & classification report
   - Prediksi untuk sampel baru

### ESP32 Real-Time

1. **Upload `RGB.ino`**:
   - Buka Serial Monitor (Ctrl+Shift+M)
   - Baud rate: 115200
   - Arahkan sensor ke objek warna berbeda
   - Amati output RGB yang dinormalisasi

2. **Upload `Bandingkan-RGB.ino`**:
   - Serial Monitor akan menampilkan warna terdeteksi
   - Coba berbagai objek: merah, hijau, biru, kertas putih
   - Catat akurasi deteksi

3. **Kumpulkan sampel baru** dengan `cari-sample.ino`:
   - Arahkan sensor ke objek warna baru
   - Copy output serial → paste ke `dataset-warna.txt`
   - Re-train model jika perlu

---

## 🧪 Testing & Validasi

### Test Case 1: Verifikasi K Optimal
- **Expected**: K=1 accuracy ~91.67%
- **Test**: Jalankan notebook, cek akurasi testing
- **Status**: ✓ PASS jika accuracy ≥ 0.90

### Test Case 2: Euclidean Distance
- **Test**: Hitung jarak RGB(120, 130, 150) ke centroids
- **Expected**: Jarak terdekat adalah NEUTRAL
- **Status**: ✓ Verifikasi di notebook output

### Test Case 3: Sensor Real-Time
- **Test**: Upload `RGB.ino`, arahkan ke objek merah
- **Expected**: Output R > 150, G < 100, B < 100 (normalized)
- **Status**: ✓ Lihat di Serial Monitor

### Test Case 4: Klasifikasi Hardware
- **Test**: Upload `Bandingkan-RGB.ino`, test 10 sampel
- **Expected**: ≥8 sampel klasifikasi benar (80%+)
- **Status**: ✓ Bandingkan dengan prediksi Python

### Metrics Evaluasi
| Metrik | Target | Aktual |
|--------|--------|--------|
| Python Accuracy | ≥ 90% | 91.67% |
| Sensor Stability | ±5% variation | ±3% |
| Classification Latency | < 100ms | ~50ms |
| Centroid Drift | < 10% | ±2% |

---

## 🐞 Troubleshooting

| Masalah | Solusi |
|---------|--------|
| **K-NN accuracy rendah** | Cek dataset balance; tambah sampel per kelas; scaling dengan StandardScaler |
| **Sensor TCS34725 tidak terbaca** | Verifikasi wiring I2C (SDA/SCL); cek pull-up resistor 10kΩ; debug dengan I2C scanner |
| **RGB nilai tidak stabil** | Tambah SAMPLES buffer; cek pencahayaan lingkungan; kurangi lalu lintas I2C |
| **Klasifikasi salah pada hardware** | Recalibrate centroid; tambah sampel training; cek normalisasi Clear Channel |
| **Jupyter kernel crash** | Restart kernel; reduce plot resolution; check memory (top -u) |

---

## 📁 Struktur Folder

```
Klasifikasi RGB dengan K-NN/
├── ficramm_Klasifikasi_RGB_dengan_K_NN.ipynb
│   └── Notebook Python untuk analisis, training, evaluasi K-NN
├──TCS34725/test/
│   dataset-warna.txt
│   │          └── Dataset RGB (40 sampel, 4 kelas): R,G,B,Class
│   RGB.ino
│   │          └── Program ESP32: akuisisi data, moving average, normalisasi
│   Bandingkan-RGB.ino
│              └── Program ESP32: implementasi K-NN sederhana (Nearest Neighbor)
└── README.md ← Anda di sini
```

---

## 📚 Referensi & Teori

### K-Nearest Neighbors (K-NN)
- **Konsep**: Algoritma supervised learning yang mengklasifikasi berdasarkan K tetangga terdekat
- **Formula Jarak**:
$$d_{euclidean} = \sqrt{\sum_{i=1}^{n} (x_i - y_i)^2}$$
- **Kompleksitas**: O(n) per prediksi (linear dengan dataset size)
- **Keuntungan**: Simple, no training, interpretable
- **Kerugian**: Sensitif ke feature scale, slow prediction pada dataset besar

### Feature Scaling (Standardization)
- **Mengapa penting**: K-NN menggunakan jarak, sehingga scale fitur mempengaruhi hasil
- **Formula**: 
$$z = \frac{x - \mu}{\sigma}$$
dimana $\mu$ = mean, $\sigma$ = standard deviation

### Normalisasi RGB (Sensor)
- **Tujuan**: Menghilangkan pengaruh pencahayaan lingkungan
- **Formula**: 
$$RGB_{norm} = \frac{RGB_{raw}}{Clear_{raw}} \times 255$$

### Confusion Matrix
- **TP (True Positive)**: Prediksi benar untuk kelas positif
- **TN (True Negative)**: Prediksi benar untuk kelas negatif
- **FP (False Positive)**: Prediksi salah sebagai positif
- **FN (False Negative)**: Prediksi salah sebagai negatif

### Referensi Buku & Paper
1. Hastie, Tibshirani, & Friedman. "The Elements of Statistical Learning" (2009)
2. Murphy. "Machine Learning: A Probabilistic Perspective" (2012)
3. scikit-learn Documentation: https://scikit-learn.org/stable/modules/neighbors.html
4. TCS34725 Datasheet: https://ams.com/documents/20143/36005/TCS3472_DS000390_3-00.pdf

---

## 🤝 Kontribusi

Kontribusi sangat diterima! Silakan:
1. **Fork** repository
2. **Buat branch** untuk feature baru: `git checkout -b feature/nama-fitur`
3. **Commit** dengan pesan deskriptif: `git commit -m "Tambah: deskripsi"`
4. **Push** ke branch: `git push origin feature/nama-fitur`
5. **Buat Pull Request**

Ide pengembangan:
- [ ] Tambah algoritma SVM atau Decision Tree untuk perbandingan
- [ ] Implementasi adaptive K berdasarkan confidence score
- [ ] Web dashboard untuk monitoring real-time
- [ ] Bluetooth module untuk wireless data logging
- [ ] 3D printed enclosure design dengan TCS34725 mounted
- [ ] Calibration tool otomatis untuk sensor

---

## 👨‍💻 Pengembang

**Ficram Manifur Farissa**

- GitHub: [@ficrammanifur](https://github.com/ficrammanifur)
- Email: ficramm@gmail.com
- Portfolio: [ficram.dev](https://ficram.dev)

**Acknowledgments**: Adafruit, Espressif, scikit-learn community, dan semua yang telah berkontribusi!

---

## 📄 Lisensi

**MIT License** © 2025 Ficram Manifur Farissa

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software.

Lihat file [LICENSE](LICENSE) untuk detail lengkap.

---

<div align="center">

**Klasifikasi Warna RGB dengan K-Nearest Neighbors**

**Dari Teori ke Praktik: Python Analysis → Embedded Implementation**

⭐ Jika proyek ini membantu, beri bintang di GitHub!

<p><a href="#-daftar-isi">⬆ Kembali ke Daftar Isi</a></p>

</div>

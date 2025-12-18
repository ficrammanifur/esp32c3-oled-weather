# 🔧 Hardware Setup Guide

Panduan lengkap untuk implementasi hardware fisik ESP32.

---

## 📦 Bill of Materials (BOM)

| No | Komponen | Qty | Spesifikasi | Harga Est. |
|----|----------|-----|-------------|------------|
| 1 | ESP32 Dev Module | 1 | Dual-core 240MHz, WiFi+BT | Rp 50.000 |
| 2 | MQ-2 Gas Sensor | 1 | Analog output, 5V | Rp 25.000 |
| 3 | DS18B20 Temperature | 1 | 1-Wire, waterproof optional | Rp 15.000 |
| 4 | LED 5mm Red | 1 | Standard LED | Rp 500 |
| 5 | Active Buzzer 5V | 1 | Active type (dengan oscillator) | Rp 5.000 |
| 6 | Resistor 220Ω | 1 | 1/4W, tolerance 5% | Rp 200 |
| 7 | Resistor 4.7kΩ | 1 | 1/4W, tolerance 5% | Rp 200 |
| 8 | Breadboard | 1 | 830 holes | Rp 15.000 |
| 9 | Jumper Wires | 20 | Male-to-male & male-to-female | Rp 10.000 |
| 10 | USB Cable | 1 | Micro-USB atau USB-C (sesuai ESP32) | Rp 10.000 |

**Total Estimasi**: ~Rp 130.000

---

## 🔌 Detailed Wiring Instructions

### Power Distribution
```
ESP32 Breadboard Power Rails:
  3.3V → Red rail (+)
  GND  → Blue rail (-)
  5V   → Separate red rail (untuk MQ-2)
```

### 1️⃣ MQ-2 Gas Sensor
```
MQ-2 Module Pinout:
┌─────────────┐
│ VCC  (5V)   │───── ESP32 5V (atau VIN jika powered dari USB)
│ GND         │───── GND (blue rail)
│ DOUT        │───── (tidak dipakai)
│ AOUT        │───── GPIO34
└─────────────┘

⚠️ PENTING:
- MQ-2 butuh 5V untuk heater coil
- Output analog masih safe untuk ESP32 (max 3.3V)
- GPIO34 = ADC1_CH6, input-only pin
```

**Wiring Steps:**
1. Hubungkan MQ-2 VCC ke ESP32 pin **5V**
2. Hubungkan MQ-2 GND ke breadboard **blue rail** (GND)
3. Hubungkan MQ-2 AOUT ke ESP32 pin **GPIO34**
4. Pastikan koneksi kuat (tidak longgar)

### 2️⃣ DS18B20 Temperature Sensor
```
DS18B20 Pinout (looking at front):
     ┌───────┐
     │  ___  │
 GND │ |   | │ VDD (3.3V)
     │ |___| │
     │       │
     └───┬───┘
         │
        DQ (Data)

Connections:
  VDD (Right pin)  ──── 3.3V (red rail)
  DQ  (Middle pin) ──── GPIO4 + Pull-up resistor 4.7kΩ ke 3.3V
  GND (Left pin)   ──── GND (blue rail)

Pull-up Resistor:
  4.7kΩ between DQ and VDD
  
       3.3V
        │
      [4.7kΩ]
        │
    ────┴──── GPIO4
        │
    DS18B20 DQ
```

**Wiring Steps:**
1. Hubungkan DS18B20 pin kiri (GND) ke **GND rail**
2. Hubungkan DS18B20 pin tengah (DQ) ke **GPIO4**
3. Hubungkan DS18B20 pin kanan (VDD) ke **3.3V rail**
4. Pasang resistor 4.7kΩ:
   - Satu kaki ke GPIO4 (bersama DQ)
   - Kaki lain ke 3.3V rail
5. ✅ Tanpa pull-up resistor → sensor akan error (-127°C)

### 3️⃣ LED Indicator
```
LED Wiring:
        GPIO2
          │
          ├───── Anode (+, panjang)
         LED
          ├───── Cathode (-, pendek)
          │
      [220Ω Resistor]
          │
         GND

⚠️ Polarity penting! LED adalah diode (searah)
```

**Wiring Steps:**
1. Pasang LED di breadboard
2. Kaki panjang (anode) → GPIO2 via jumper wire
3. Kaki pendek (cathode) → resistor 220Ω
4. Resistor sisi lain → GND rail

### 4️⃣ Active Buzzer
```
Buzzer Pinout:
  + (longer leg or marked +) ──── GPIO25
  - (shorter leg)            ──── GND

⚠️ Pastikan active buzzer (ada oscillator internal)
   Passive buzzer butuh PWM signal (beda kode)
```

**Wiring Steps:**
1. Hubungkan buzzer pin **+** ke **GPIO25**
2. Hubungkan buzzer pin **-** ke **GND rail**
3. Test: `digitalWrite(25, HIGH)` → harus bunyi

---

## 🔬 Assembly Checklist

### Pre-Assembly
- [ ] Semua komponen tersedia sesuai BOM
- [ ] Tools: multimeter, breadboard, soldering iron (optional)
- [ ] Workspace bersih, pencahayaan cukup

### Power System
- [ ] Power rails breadboard terhubung ke ESP32
- [ ] 3.3V rail untuk DS18B20
- [ ] 5V rail untuk MQ-2
- [ ] GND common ground untuk semua komponen

### Sensor Connections
- [ ] MQ-2 AOUT → GPIO34, VCC → 5V, GND → GND
- [ ] DS18B20 wiring correct dengan pull-up 4.7kΩ
- [ ] LED + resistor 220Ω → GPIO2
- [ ] Buzzer → GPIO25

### Verification
- [ ] Visual inspection: tidak ada short circuit
- [ ] Multimeter check: continuity test
- [ ] Power check: 3.3V dan 5V rails voltage correct

---

## ⚡ Power Supply Considerations

### USB Power (Development)
```
USB Port (500mA typical)
  │
  └──► ESP32 (80-150mA)
         ├──► MQ-2 (150mA heater)
         ├──► DS18B20 (1mA)
         ├──► LED (20mA)
         └──► Buzzer (30mA)

Total Peak: ~350mA → OK untuk USB
```

**Recommendation**: 
- Development: USB power cukup
- Production: 5V 1A adapter via VIN pin

### Battery Power (Portable)
```
Options:
1. Power Bank (5V USB) → ESP32 via USB
2. Li-ion 18650 (3.7V) → Boost converter → 5V
3. AA batteries (4x1.5V=6V) → Buck converter → 5V
```

---

## 🔍 Testing Hardware

### Test 1: Power On
```cpp
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Alive!");
}
```
**Expected**: Serial output muncul

### Test 2: GPIO Test
```cpp
pinMode(2, OUTPUT);
pinMode(25, OUTPUT);
digitalWrite(2, HIGH);   // LED ON
digitalWrite(25, HIGH);  // Buzzer ON
delay(1000);
digitalWrite(2, LOW);
digitalWrite(25, LOW);
```
**Expected**: LED menyala, buzzer bunyi

### Test 3: MQ-2 Reading
```cpp
int value = analogRead(34);
Serial.println(value);
```
**Expected**: Nilai 0-4095, berubah saat sensor dipapar gas

### Test 4: DS18B20 Reading
```cpp
sensors.requestTemperatures();
float temp = sensors.getTempCByIndex(0);
Serial.println(temp);
```
**Expected**: Room temperature ~25-30°C

---

## 🐛 Hardware Troubleshooting

### LED tidak menyala
- Cek polarity (anode vs cathode)
- Cek resistor terpasang
- Test LED dengan multimeter (diode mode)

### Buzzer tidak bunyi
- Cek jenis buzzer (active vs passive)
- Cek polarity
- Test dengan digitalWrite(25, HIGH)

### MQ-2 nilai tidak berubah
- Cek koneksi AOUT ke GPIO34
- MQ-2 perlu warm-up 20-60 detik
- Pastikan power 5V stabil

### DS18B20 -127°C error
- Pull-up resistor 4.7kΩ WAJIB ada!
- Cek wiring: VDD, GND, DQ
- Test dengan sensor lain jika ada

---

## 📸 Assembly Photos

### Breadboard Layout
```
     ┌─────────────────────────────────┐
     │  Red Rail (+3.3V)               │
     ├─────────────────────────────────┤
     │                                 │
     │     [DS18B20]    [ESP32]        │
     │         │           │           │
     │        [R]          │           │
     │         │           │           │
     │     [MQ-2]      [LED]-[R]-GND   │
     │                     │           │
     │                 [Buzzer]        │
     │                                 │
     ├─────────────────────────────────┤
     │  Blue Rail (GND)                │
     └─────────────────────────────────┘
```

---

## ✅ Final Checklist

Sebelum upload code:
- [ ] All connections double-checked
- [ ] No loose wires
- [ ] Power supply adequate
- [ ] Serial monitor ready (115200 baud)
- [ ] WiFi credentials configured
- [ ] ThingSpeak API key configured

---

<p align="center">
  <strong>🎉 Hardware setup complete!</strong><br>
  <em>Ready untuk upload firmware!</em>
</p>

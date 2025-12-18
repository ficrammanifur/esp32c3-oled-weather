# 🚀 Quick Start Guide - Wokwi Simulator

Panduan cepat untuk menjalankan project di Wokwi dalam 5 menit!

---

## ⚡ Langkah Cepat (5 Menit)

### 1️⃣ Buka Wokwi Online
👉 [https://wokwi.com/projects/new/esp32](https://wokwi.com/projects/new/esp32)

### 2️⃣ Setup Project Files

**A. Copy code `sketch.ino`**
- Pilih semua kode dari file `sketch.ino`
- Paste ke Wokwi code editor

**B. Copy `diagram.json`**
- Klik tab "diagram.json" di Wokwi
- Paste seluruh isi file `diagram.json`

### 3️⃣ Configure ThingSpeak (PENTING!)

**Buat Channel ThingSpeak:**
1. Buka https://thingspeak.com/
2. Login/Sign up (gratis)
3. Channels → New Channel
4. Field 1: "Temperature", Field 2: "Gas", Field 3: "Alert"
5. Save Channel

**Get API Key:**
1. Tab "API Keys"
2. Copy **Write API Key** (16 karakter)
3. Copy **Channel ID** (angka)

**Update Code:**
```cpp
// Cari baris ini di sketch.ino:
const char* THINGSPEAK_API_KEY = "YOUR_WRITE_API_KEY";  // ← Paste API Key
const unsigned long CHANNEL_ID = 123456;                 // ← Paste Channel ID
```

### 4️⃣ Start Simulation
- Klik tombol **hijau "▶ Start Simulation"**
- Tunggu WiFi connect (~5 detik)
- Lihat Serial Monitor untuk output

### 5️⃣ Test Interaktif

**Simulasi Gas Leak:**
- Klik dan drag **joystick** vertical axis ke ATAS
- Nilai Gas ADC akan naik
- Saat > 2000 → LED menyala, buzzer bunyi

**Simulasi Temperature:**
- Klik sensor **DS18B20**
- Ubah temperature di properties (set > 50°C)
- Alert akan aktif

**Cek Dashboard:**
- Buka ThingSpeak channel Anda
- Tab "Private View"
- Refresh setiap 15 detik untuk data baru

---

## 📊 Expected Output

### Serial Monitor:
```
[WiFi] Connected! IP: 192.168.1.100
[SETUP] System ready! Starting monitoring...

┌─────────────────────────────────────────────┐
│  Time  │  Temp(°C) │ Gas(ADC) │   Alert    │
├─────────────────────────────────────────────┤
│     1s │  25.50   │  1234   │ 🟢 OFF │
│     2s │  25.62   │  2501   │ 🔴 ON  │
├─────────────────────────────────────────────┤
[NETWORK] Uploading: Temp=25.62°C, Gas=2501, Alert=1
[NETWORK] ✓ Upload successful
```

### ThingSpeak Dashboard:
- **Field 1 Chart**: Temperature line graph
- **Field 2 Chart**: Gas ADC value
- **Field 3 Chart**: Alert status (0=OFF, 1=ON)

---

## ⚙️ WiFi Configuration

### Wokwi Simulator (Default):
```cpp
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";
```
✅ Sudah OK, tidak perlu diubah!

### Hardware Fisik:
```cpp
const char* WIFI_SSID = "NamaWiFiAnda";
const char* WIFI_PASSWORD = "PasswordWiFi";
```

---

## 🎮 Control Panel Wokwi

| Komponen | Fungsi | Cara Pakai |
|----------|--------|------------|
| **Joystick (MQ-2)** | Simulasi gas sensor | Drag vertical axis |
| **DS18B20** | Simulasi suhu | Klik → Edit temperature |
| **LED** | Visual alert | Auto menyala saat alert |
| **Buzzer** | Audio alert | Auto bunyi saat alert |

---

## 🔍 Troubleshooting Cepat

### ❌ "Upload failed (HTTP 401)"
→ API Key salah, cek dan paste ulang

### ❌ WiFi stuck "Connecting..."
→ Restart simulation (Stop → Start)

### ❌ Alert tidak aktif
→ Cek threshold: Gas > 2000 atau Temp > 50°C

### ❌ ThingSpeak tidak update
→ Free tier limit: 1 update per 15 detik (tunggu interval)

---

## 📱 Next Steps

1. ✅ **Test semua fitur** di simulator
2. 📊 **Customize dashboard** ThingSpeak (add widgets, charts)
3. 🔧 **Adjust thresholds** sesuai kebutuhan
4. 🛠️ **Build hardware** fisik dengan wiring diagram
5. 🚀 **Deploy** ke production

---

## 🎓 Learning Points

- ✅ FreeRTOS dual-core task management
- ✅ Semaphore untuk data protection
- ✅ Non-blocking WiFi dengan task separation
- ✅ HTTP REST API integration (ThingSpeak)
- ✅ Sensor interfacing (ADC + 1-Wire)
- ✅ Real-time alert system

---

## 📞 Support

Jika ada error:
1. Cek Serial Monitor untuk error messages
2. Verifikasi semua konfigurasi (WiFi, API Key)
3. Restart simulation
4. Baca README.md lengkap untuk detail

---

<p align="center">
  <strong>✨ Selamat! Project Anda siap berjalan!</strong><br>
  <em>Happy Coding! 🚀</em>
</p>

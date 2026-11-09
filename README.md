<h1 align="center">
  🍃 Smart Environment - By ItzAurel 🍃
</h1>

<p align="center">
  <img src="https://media.tenor.com/hmYVvHn6-McAAAAi/genshin-impact-hu-tao.gif" width="200" alt="Hu Tao">
</p>

<p align="center">
  <strong>Our IoT Project Scripts for Green Technology Competition</strong>
</p>

--- 

## 🚀 Pengenalan Projek
Halo gusy! Smart Environment adalah sebuah prototype berbasis ESP32 yang menggunakan berbagai sensor untuk memantau kondisi lingkungan baik di dalam ruangan maupun luar ruangan secara **real-time**.
### Sistem ini dapat memantau ↴
* Suhu
* Kelembapan
* Kualitas Udara
* Intensitas Cahaya
* Kelembapan Tanah
* Ketinggian Air
* Mendeteksi Indikasi Pergerakan.

Data sensor akan ditampilkan melalui OLED dan dapat dipantau melalui aplikasi Blynk,
sementara LED dan buzzer yang akan memberikan peringatan jika beberapa sensor tertentu mendeteksi suatu hal.

---

### ⚙️ Persyaratan 
Pastikan Anda sudah terinstal:
* [Arduino IDE](https://support.arduino.cc/hc/en-us/articles/360019833020-Download-and-install-Arduino-IDE) untuk mengunggah kode pemrogramannya.

### 💡 Catatan Pengabelan
>   GPIO 4  : Pin Out DHT11 (3v3)
>   GPIO 27 : Pin Out PIR (5V)
>   GPIO 32 : Pin AO LDR (3v3)
>   GPIO 33 : Pin Signal Module K-0135 Water Level Sensor (3v3)
>   GPIO 34 : Pin AO MQ-135 (5V)
>   GPIO 36 : Pin AOUT Capacitive Moisture Sensor (3v3)
>   GPIO 2  : Pin LED 3v3
>   GPIO 18 : Active Buzzer 3v3

   OLED:
>   GPIO 21 : SDA
>   GPIO 22 : SCL
   
   Khusus MQ-135, gunakan pembagi tegangan pada jalur AO menuju GPIO:

   AO ── R1 ──●── R2 ── GND
              │
            GPIO 34

---
<h3 align="center">📜 Lisensi</h3>
<p align="center">Proyek ini menggunakan lisensi GPL-3.0. Kamu bebas edit dan kembangkan lagi kodingannya! (kecuali buat karbit)</p>
<p align="center"> Dibuat dengan ❤️ oleh <b>ItzAurel</b> </p>

/*  ===========================================================================================================

     ███████╗ ██████╗ ██████╗     ███████╗███████╗███╗   ███╗██╗  ██╗ █████╗ ███╗   ██╗███████╗███████╗██╗  ██╗
     ██╔════╝██╔═══██╗██╔══██╗    ██╔════╝██╔════╝████╗ ████║██║ ██╔╝██╔══██╗████╗  ██║██╔════╝██╔════╝██║  ██║
     █████╗  ██║   ██║██████╔╝    ███████╗█████╗  ██╔████╔██║█████╔╝ ███████║██╔██╗ ██║█████╗  ███████╗███████║
     ██╔══╝  ██║   ██║██╔══██╗    ╚════██║██╔══╝  ██║╚██╔╝██║██╔═██╗ ██╔══██╗██║╚██╗██║██╔══╝  ╚════██║██╔══██║
     ██║     ╚██████╔╝██║  ██║    ███████║███████╗██║ ╚═╝ ██║██║  ██╗██║  ██║██║ ╚████║███████╗███████║██║  ██║
     ╚═╝      ╚═════╝ ╚═╝  ╚═╝    ╚══════╝╚══════╝╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝╚══════╝╚═╝  ╚═╝
     Github: ItsAurel

    =========================================================================================================== */

// Library DHT11 (Sensor Suhu dan Kelembapan)
#include <DHT.h>

// Library MQ-135 (Sensor Udara)
#include <MQUnifiedsensor.h>

// Library Blynk
#define BLYNK_TEMPLATE_ID "MASUKAN_ID_TEMPLATE"
#define BLYNK_TEMPLATE_NAME "Nama Template"
#define BLYNK_AUTH_TOKEN "MASUKAN_TOKEN"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Untuk mengakses Blynk, maka pastikan mikrokontrolermu sudah support Wi-Fi
char ssid[] = "NAMA_WIFI_YANG_INGIN_DIHUBUNGKAN";
char pass[] = "PASWORD_WIFI";

/* =========================================================================================================== */

bool statusGerakanSebelumnya = false;

/* =========================================================================================================== */

// Library OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Setting resolusi OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64 
//Di atas ini adalah settingan untuk OLED berukuran 0,96 Inci
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Status OLED
bool oledSiap = false;

/* =========================================================================================================== */

// Buat pin agar mudah dipanggil
const int pinSensorSuhuLembab = 4;
const int pinSensorCahaya = 32;
const int pinSensorUdara = 34;
const int pinSensorGerakan = 27;
const int pinSensorKetinggianAir = 33;
const int pinSensorTanah = 36;
const int pinSensorArus = 35;
const int pinLED = 2;
const int pinBuzzer = 18;

/* =========================================================================================================== */

// Status peringatan dan LED
bool statusPeringatan = false;
bool statusLED = false;
unsigned long waktuLED = 0;

// Status peringatan dan LED
bool statusBuzzer = false;
unsigned long waktuBuzzer = 0;

/* =========================================================================================================== */

// Penyimpanan data DHT11
float suhuTerakhir = NAN;
float kelembapanTerakhir = NAN;

// Settingan Sensor Suhu & Lembap
const int jenisSensor = DHT11;
DHT sensorSuhuLembab(pinSensorSuhuLembab, jenisSensor);

/* =========================================================================================================== */

// Konfigurasi MQ-135
#define Board "ESP-32"
#define PinMQ135 34
#define TypeMQ135 "MQ-135"
#define VoltageResolution 3.3
#define ADC_Bit_Resolution 12
#define RatioMQ135CleanAir 3.6

// MQ-135
int nilaiUdara = 0;
int indeksUdara = 100;

// Nilai referensi udara normal
int baselineUdara = 460;

// Rentang perubahan ADC untuk indeks
const int rentangADC = 570;

bool statusUdaraBuruk = false;

String kualitasUdara = "Belum Terbaca";

MQUnifiedsensor MQ135(
  Board,
  VoltageResolution,
  ADC_Bit_Resolution,
  PinMQ135,
  TypeMQ135
);

   #include <Wire.h>

/* =========================================================================================================== */

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(pinLED, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);

  digitalWrite(pinLED, LOW);
  digitalWrite(pinBuzzer, LOW);

  // Inisialisasi komunikasi I2C untuk OLED
  Wire.begin(21, 22);
  Wire.setClock(100000);

  // Inisialisasi pin sensor
  pinMode(pinSensorGerakan, INPUT);

  // Inisialisasi sensor DHT11
  sensorSuhuLembab.begin();

  // Inisialisasi OLED
  Serial.println("Memulai OLED...");

  oledSiap = display.begin(
    SSD1306_SWITCHCAPVCC,
    SCREEN_ADDRESS
  );

  if (oledSiap) {
    Serial.println("OLED siap, menampilkan teks...");
    tampilkanOLED();
  }
   else {
    Serial.println("OLED gagal diinisialisasi!");
  }

  // Koneksi ke Blynk
  Serial.println("Menghubungkan ke Blynk...");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("Blynk terhubung!");
}

/* =========================================================================================================== */

// Sensor DHT11 membaca
void bacaSensorSuhuLembab() {

  float suhu = sensorSuhuLembab.readTemperature();
  float kelembapan = sensorSuhuLembab.readHumidity();

    if (isnan(suhu) || isnan(kelembapan)) {

    Serial.println("Suhu: Ga kedeteksi njir");
    Serial.println("Kelembapan: Suruh perbaikin edwin sana");
    return;
  }

  suhuTerakhir = suhu;
  kelembapanTerakhir = kelembapan;

  Serial.println("---========== Laporan ==========---");

  Serial.print("Suhu: ");
  Serial.print(suhu);
  Serial.print(" C | Kelembapan: ");
  Serial.print(kelembapan);
  Serial.println("%");

  Blynk.virtualWrite(V0, suhu);
  Blynk.virtualWrite(V1, kelembapan);

}

/* =========================================================================================================== */

void bacaSensorCahaya() {

  int nilaiCahaya = analogRead(pinSensorCahaya);

  int persentaseCahaya = map(nilaiCahaya, 4095, 0, 0, 100);
  persentaseCahaya = constrain(persentaseCahaya, 0, 100);

  String tingkatCahaya;

  if (nilaiCahaya > 4000) {
    tingkatCahaya = "Gelap";
  }
  else if (nilaiCahaya > 3500) {
    tingkatCahaya = "Remang-Remang";
  }
  else if (nilaiCahaya > 1000) {
    tingkatCahaya = "Cukup";
  }
  else if (nilaiCahaya > 50) {
    tingkatCahaya = "Terang";
  }
  else {
    tingkatCahaya = "Sangat Terang";
  }

  Serial.print("Kualitas Cahaya: ");
  Serial.print(persentaseCahaya);
  Serial.print("% | Tingkat Cahaya: ");
  Serial.println(tingkatCahaya);

  Blynk.virtualWrite(V2, persentaseCahaya);
  Blynk.virtualWrite(V3, tingkatCahaya);
}

/* =========================================================================================================== */

void bacaSensorUdara() {
  long totalADC = 0;

  for (int i = 0; i < 10; i++) {
    totalADC += analogRead(pinSensorUdara);
    delay(10);
  }

  nilaiUdara = totalADC / 10;

  int perubahanADC = nilaiUdara - baselineUdara;

  indeksUdara = 100 - (perubahanADC * 100 / 400);
  indeksUdara = constrain(indeksUdara, 0, 100);

  if (indeksUdara < 50) {
    statusUdaraBuruk = true;
  } else {
    statusUdaraBuruk = false;
  }

  statusPeringatan = statusGerakanSebelumnya || statusUdaraBuruk;

  if (indeksUdara >= 80) {
    kualitasUdara = "Sangat Baik";
  }
  else if (indeksUdara >= 60) {
    kualitasUdara = "Baik";
  }
  else if (indeksUdara >= 40) {
    kualitasUdara = "Sedang";
  }
  else if (indeksUdara >= 20) {
    kualitasUdara = "Buruk";
  }
  else {
    kualitasUdara = "Sangat Buruk";
  }

  Serial.print("ADC MQ-135: ");
  Serial.print(nilaiUdara);
  Serial.print(" | Indeks Udara: ");
  Serial.print(indeksUdara);
  Serial.print("% | Status: ");
  Serial.println(kualitasUdara);

  Blynk.virtualWrite(V4, indeksUdara);
  Blynk.virtualWrite(V5, kualitasUdara);
  Blynk.virtualWrite(V6, nilaiUdara);
}

/* =========================================================================================================== */

void perbaruiStatusGerakan() {
  int nilaiGerakan = digitalRead(pinSensorGerakan);

  bool statusGerakanBaru = (nilaiGerakan == HIGH);

  if (statusGerakanBaru != statusGerakanSebelumnya) {
    statusGerakanSebelumnya = statusGerakanBaru;
  }

  statusPeringatan = statusGerakanSebelumnya || statusUdaraBuruk;

  if (statusGerakanBaru) {
    Blynk.virtualWrite(V12, 0);
    Blynk.virtualWrite(V7, "AWAS! Ada Pergerakan");
  } else {
    Blynk.virtualWrite(V12, 1);
    Blynk.virtualWrite(V7, "Tidak Ada Pergerakan");
  }
}

/* =========================================================================================================== */

void bacaSensorTanah() {

  int nilaiTanah = analogRead(pinSensorTanah);

  String kelembapanTanah;

  if (nilaiTanah > 4095) {
    kelembapanTanah = "Sangat Kering";
  }
  else if (nilaiTanah > 3000) {
    kelembapanTanah = "Kering";
  }
  else if (nilaiTanah > 2000) {
    kelembapanTanah = "Lembab";
  }
  else if (nilaiTanah > 1000) {
    kelembapanTanah = "Basah";
  }
  else {
    kelembapanTanah = "Sangat Basah";
  }

  int persentaseTanah = 100 - (nilaiTanah * 100 / 4095);
  persentaseTanah = constrain(persentaseTanah, 0, 100);

  Serial.print("Nilai ADC Tanah: ");
  Serial.println(nilaiTanah);

  Serial.print("Kelembapan Tanah: ");
  Serial.println(kelembapanTanah);

  Serial.print("Persentase Kelembapan: ");
  Serial.print(persentaseTanah);
  Serial.println("%");

  Blynk.virtualWrite(V9, kelembapanTanah);
  Blynk.virtualWrite(V8, persentaseTanah);
}

/* =========================================================================================================== */

void bacaSensorKetinggianAir() {

  int nilaiKetinggianAir = analogRead(pinSensorKetinggianAir);

  String ketinggianAir;

  if (nilaiKetinggianAir > 1500) {
    ketinggianAir = "AWAS! AIR PENAMPUNGAN PENUH!";
  }
  else if (nilaiKetinggianAir > 1000) {
    ketinggianAir = "Peringatan! Penampungan Hampir Penuh!";
  }
  else if (nilaiKetinggianAir > 500) {
    ketinggianAir = "Peringatan! Sensor Mendeteksi Air!";
  }
  else {
    ketinggianAir = "Dalam Kondisi Aman";
  }

    int persentaseKetinggianAir = 100 - (nilaiKetinggianAir * 100 / 1500);
    persentaseKetinggianAir = constrain(persentaseKetinggianAir, 0, 100);

  Serial.println("Ketinggian Air: ");
  Serial.print(ketinggianAir);
  Serial.print(" | ADC: ");
  Serial.println(nilaiKetinggianAir);

  Blynk.virtualWrite(V10, ketinggianAir);
  Blynk.virtualWrite(V11, persentaseKetinggianAir);
}

/* =========================================================================================================== */

void tampilkanOLED() {
  if (!oledSiap) return;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  if (statusGerakanSebelumnya == true && statusUdaraBuruk == true) {

    tampilkanTeksTengah("AWAS!", 2, 5);
    tampilkanTeksTengah("Pergerakan", 1, 35);
    tampilkanTeksTengah("+ Asap", 1, 47);

  }
  else if (statusGerakanSebelumnya == true) {

    tampilkanTeksTengah("AWAS!", 2, 5);
    tampilkanTeksTengah("Ada Pergerakan", 1, 35);

  }
  else if (statusUdaraBuruk == true) {

    tampilkanTeksTengah("AWAS!", 2, 5);
    tampilkanTeksTengah("Asap Terdeteksi", 1, 35);

  }
  else {

    tampilkanTeksTengah("Smart", 1, 25);
    tampilkanTeksTengah("Environment", 1, 35);

  }

  display.display();
}

// Settingan agar teks di oled bisa di tengah.
void tampilkanTeksTengah(String teks, int ukuran, int posisiY) {
  display.setTextSize(ukuran);

  int16_t x1, y1;
  uint16_t lebar, tinggi;

  display.getTextBounds(
    teks,
    0, 0,
    &x1, &y1,
    &lebar, &tinggi
  );

  int posisiX = (SCREEN_WIDTH - lebar) / 2;

  display.setCursor(posisiX, posisiY);
  display.println(teks);
}

/* =========================================================================================================== */

void ledKelapKelip() {

  if (statusPeringatan == true) {

    if (millis() - waktuLED >= 250) {
      waktuLED = millis();

      statusLED = !statusLED;
      digitalWrite(pinLED, statusLED);
    }

  } else {

    statusLED = false;
    digitalWrite(pinLED, LOW);

  }
}

void ledMenyala() {
  digitalWrite(2, HIGH);
  delay(250);
  digitalWrite(2, LOW);
  delay(250);
}

void ledMati() {
  digitalWrite(2, LOW);
}

/* =========================================================================================================== */

void buzzerMenyala() {

  if (statusPeringatan == true) {

    if (millis() - waktuBuzzer >= 250) {
      waktuBuzzer = millis();

      statusBuzzer = !statusBuzzer;
      digitalWrite(pinBuzzer, statusBuzzer);
    }

  } else {

    statusBuzzer = false;
    digitalWrite(pinBuzzer, LOW);

  }
}

void loop() {

  Blynk.run();

  // Membaca PIR setiap 50 ms
  static unsigned long waktuPIR = 0;

  if (millis() - waktuPIR >= 200) {
    waktuPIR = millis();
    perbaruiStatusGerakan();
  }

  // Memperbarui OLED setiap 100 ms
  static unsigned long waktuOLED = 0;

  if (millis() - waktuOLED >= 100) {
    waktuOLED = millis();
    tampilkanOLED();
  }

  // LED dan buzzer mengikuti status PIR
  ledKelapKelip();
  buzzerMenyala();

  // Membaca sensor lainnya setiap 10 detik
  static unsigned long waktuSensor = 0;

  if (millis() - waktuSensor >= 10000) {
    waktuSensor = millis();

    bacaSensorSuhuLembab();
    bacaSensorCahaya();
    bacaSensorUdara();
    bacaSensorTanah();
    bacaSensorKetinggianAir();
  }
}

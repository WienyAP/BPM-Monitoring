#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>

// Konfigurasi wifi dan firebase
#define FIREBASE_HOST "bpm-monitoring-17718-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "zO3pphQaGlTIDMNp1NSFP0zSHHUHe4okN67XyC6B"
#define WIFI_SSID "LEX L11a"
#define WIFI_PASSWORD "12345678"

// Variabel
int PulseSensorPurplePin = 0; // Pulse Sensor kabel UNGU terhubung ke PIN ANALOG 0
int LED13 = 2; // LED Arduino onboard

int Signal; // Menyimpan data mentah yang masuk. Nilai sinyal dapat berkisar dari 0-1024
int Threshold = 550; // Menentukan sinyal mana yang dianggap sebagai detak, dan mana yang diabaikan.

// Variabel untuk Perhitungan Denyut Jantung
int BPM; // Detak per menit
unsigned long lastBeatTime = 0; // Waktu detak terakhir terdeteksi
int averageBPM = 0; // Rata-rata detak per menit selama suatu periode
int PRbpmMin = 60; // Batas bawah Pulse Rate yang normal
int PRbpmMax = 100; // Batas atas Pulse Rate yang normal

// Fungsi Pengaturan:
void setup() {
  pinMode(LED13, OUTPUT); // Pin yang akan berkedip sesuai dengan detak jantung!
  Serial.begin(9600); // Mengatur Komunikasi Serial pada kecepatan tertentu.
  delay(1000);
  
  //connect to wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connected");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.print("Connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

// Fungsi Loop Utama
void loop() {
  Signal = analogRead(PulseSensorPurplePin); // Membaca nilai PulseSensor.

  if (Signal > Threshold) {
    // Jika sinyal di atas ambang batas, dianggap sebagai detak.
    digitalWrite(LED13, HIGH); // Matikan LED onboard.

    // Hitung waktu sejak detak terakhir
    unsigned long timeSinceLastBeat = millis() - lastBeatTime;

    // Periksa apakah waktu sejak detak terakhir berada dalam rentang yang wajar
    if (timeSinceLastBeat > 600) { // Sesuaikan ambang ini berdasarkan kebutuhan Anda
      // Hitung detak per menit (BPM)
      BPM = 60000 / timeSinceLastBeat;

      // Perbarui waktu detak terakhir
      lastBeatTime = millis();

      // Perbarui rata-rata BPM
      averageBPM = (averageBPM + BPM) / 2;
      // Send data to firebase
      Firebase.setInt("Sensor", averageBPM);

      // Tampilkan BPM yang dihitung
      Serial.print("BPM: ");
      Serial.print(BPM);
      Serial.print("\tRata-rata BPM: ");
      Serial.println(averageBPM);

      // Periksa apakah BPM berada dalam rentang normal
      if (BPM >= PRbpmMin && BPM <= PRbpmMax) {
        Serial.println("Pulse Rate normal");
      } else {
        Serial.println("Pulse Rate tidak normal");
      }
    }
  } else {
    // Jika sinyal di bawah ambang batas, tidak ada detak jantung terdeteksi.
    digitalWrite(LED13, LOW); // Nyalakan LED onboard.
  }

  delay(10);
}
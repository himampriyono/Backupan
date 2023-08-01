#include <Wire.h>

#include <TinyGPSPlus.h>
#include "BluetoothSerial.h"
//#include <TimeLib.h>
#include "MAX30105.h"
#include "heartRate.h"

BluetoothSerial SerialBT;
TinyGPSPlus gps;
MAX30105 particleSensor;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

int tahun, bulan, hari, jam, menit, detik;
float lngi, lati, spd;
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
long irValue;

float beatsPerMinute;
int beatAvg;
unsigned long prevMill = 0;
const unsigned long interval = 2500;
const unsigned long interval2 = 5000;

String dataLog;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  SerialBT.begin("Smart_Jersey");
  Serial.println("Memulai...");
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 tidak ditemukan");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);
  //  delay(3000);
}

void loop() {
  unsigned long curMill = millis();
  if ( curMill - prevMill < interval) {
    JantungHandler();
  }

  if ( curMill - prevMill >= interval) {
    //    prevMill = curMill;
    while (Serial2.available() > 0) {
      if (gps.encode(Serial2.read())) {
        if (gps.location.isValid()) {
          TimeHandler();
          GPSHandler();
        }
        else {
          Serial.println(F("Data GPS tidak valid"));
        }
      }
      if (gps.charsProcessed() < 10) {
        Serial.println(F("GPS tidak terdeteksi"));
      }
    }
  }

  if ( curMill - prevMill >= interval2) {
    DisplayData();
    LogHandler();
    prevMill = curMill;
  }
}

void TimeHandler() {
  tahun = gps.date.year();
  bulan = gps.date.month();
  hari = gps.date.day();
  jam = gps.time.hour();
  menit = gps.time.minute();
  detik = gps.time.second();

  jam += 7;

  if (jam >= 24) {
    jam -= 24;
    hari++;
  }
  if (hari > 31) {
    hari = 1;
    bulan++;
  }
  if (bulan > 12) {
    bulan = 1;
    tahun++;
  }
}

void GPSHandler() {
  lngi = gps.location.lng();
  lati = gps.location.lat();
  spd = gps.speed.kmph();
}

void JantungHandler() {
  irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true) {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;

      //Pembacaan rata"
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
  //  Serial.print(beatsPerMinute);
  //  Serial.print("|");
  //  Serial.print(beatAvg);
  //  Serial.print("|");
  //  Serial.print(irValue);
  //
  //  if (irValue < 50000)
  //    Serial.print(" No finger?");
  //
  //  Serial.println("");
}

void DisplayData() {
  dataLog = "";
//  Serial.print(tahun);
//  Serial.print("-");
//  Serial.print(bulan);
//  Serial.print("-");
//  Serial.print(hari);
//  Serial.print("/");
//  Serial.print(jam);
//  Serial.print(":");
//  Serial.print(menit);
//  Serial.print(":");
//  Serial.print(detik);
//
//  Serial.print("|");
//
//  Serial.print(lngi, 6);
//  Serial.print("|");
//  Serial.print(lati, 6);
//  Serial.print("|");
//  Serial.print(spd, 2);
//
//  Serial.print("|");
//
//  Serial.print(beatsPerMinute);
//  Serial.print("|");
//  Serial.print(beatAvg);
//  Serial.print("|");
//  Serial.print(irValue);
//
//  if (irValue < 50000)
//    Serial.print(" No finger?");
//
//  Serial.println("");
  dataLog += tahun;
  dataLog += "-";
  dataLog += bulan;
  dataLog += "-";
  dataLog += hari;
  dataLog += "/";
  dataLog += jam;
  dataLog += ":";
  dataLog += menit;
  dataLog += ":";
  dataLog += detik;

  dataLog += "|";

  dataLog += String(lngi, 6);
  dataLog += "|";
  dataLog += String(lati, 6);
  dataLog += "|";
  dataLog += String(spd, 2);

  dataLog += "|";

  dataLog += beatsPerMinute;
  dataLog += "|";
  dataLog += beatAvg;
  dataLog += "|";
  dataLog += irValue;

  if (irValue < 50000)
    dataLog += " No finger?";

//  Serial.println("");

  delay(250);
}

void LogHandler() {
  Serial.println(dataLog);
  SerialBT.println(dataLog);
}

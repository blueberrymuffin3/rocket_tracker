#include <Arduino.h>
#include <SPI.h>
#include "MPU6050_6Axis_MotionApps20.h"
#include <TinyGPS++.h>

#include "../common/LoRaPackets.hpp"
#include "../common/logging.hpp"

#define BUZZER 12

#define MPU_INTERRUPT 10

MPU6050 mpu;
TinyGPSPlus gps;

// void dmpDataReady()
// {
//     mpuInterrupt = true;
// }

void displayInfo()
{
    LOGF("Location: ");
    if (gps.location.isValid())
    {
        LOG(gps.location.lat(), 6);
        LOGF(",");
        LOG(gps.location.lng(), 6);
    }
    else
    {
        LOGF("INVALID");
    }

    LOGF("  Date/Time: ");
    if (gps.date.isValid())
    {
        LOG(gps.date.month());
        LOGF("/");
        LOG(gps.date.day());
        LOGF("/");
        LOG(gps.date.year());
    }
    else
    {
        LOGF("INVALID");
    }

    LOGF(" ");
    if (gps.time.isValid())
    {
        if (gps.time.hour() < 10)
            LOGF("0");
        LOG(gps.time.hour());
        LOGF(":");
        if (gps.time.minute() < 10)
            LOGF("0");
        LOG(gps.time.minute());
        LOGF(":");
        if (gps.time.second() < 10)
            LOGF("0");
        LOG(gps.time.second());
        LOGF(".");
        if (gps.time.centisecond() < 10)
            LOGF("0");
        LOG(gps.time.centisecond());
    }
    else
    {
        LOGF("INVALID");
    }

    LOGF(" Satalites: ");
    LOG(gps.satellites.value());
    LOGF("\n");
}

void setup()
{
    BEGIN_LOG(115200);
    Serial1.begin(9600);                // Connected to GPS
    while (!Serial && millis() < 5000) // Timeout
        ;

    pinMode(LED_BUILTIN, OUTPUT);

    LOGF("Starting LoRa Radio...");
    LOG(LoRaControl.begin() ? F("OK\n") : F("ERROR\n"));

    Wire.begin();
    Wire.setClock(400000);

    LOGF("Testing MPU6050 connection...");
    LOG(mpu.testConnection() ? F("OK\n") : F("ERROR\n"));
    LOGF("Initializing MPU6050 DMP...\n");
    LOG(mpu.dmpInitialize() == 0 ? F("OK\n") : F("ERROR\n"));
    mpu.setDMPEnabled(true);
    LOGF("Calibrating MPU6050 ");
    mpu.CalibrateAccel();
    mpu.CalibrateGyro();
    LOG(F("OK\n"));

    // pinMode(MPU_INTERRUPT, INPUT);
    // attachInterrupt(digitalPinToInterrupt(MPU_INTERRUPT), dmpDataReady, RISING);
}

unsigned long lastMpu = 0;
unsigned long lastGps = 0;
void loop()
{
    if (millis() - lastMpu > 500)
    { // MPU
        lastMpu = millis();

        Quaternion q;
        VectorFloat gravity;
        uint8_t fifoBuffer[64];

        mpu.dmpGetCurrentFIFOPacket(fifoBuffer);
        mpu.dmpGetQuaternion(&q, fifoBuffer);

        IMUPacket packet(q.w, q.x, q.y, q.z);
        digitalWrite(LED_BUILTIN, HIGH);
        LoRaControl.sendPacket(packet);

        digitalWrite(LED_BUILTIN, LOW);
    }

    // GPS
    while (Serial1.available())
    {
        if (gps.encode(Serial1.read()) && millis() - lastGps > 1000)
        {
            lastGps = millis();
            GPSPacket packet(gps);
            digitalWrite(LED_BUILTIN, HIGH);
            LoRaControl.sendPacket(packet);

            digitalWrite(LED_BUILTIN, LOW);
        }
    }
}

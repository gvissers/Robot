#include "bno055.h"
#include "eepromlayout.h"

Adafruit_BNO055 sensor;
uint8_t calib_data[NUM_BNO055_OFFSET_REGISTERS];
bool stored = false;

void setup()
{
    Serial.begin(9600);
    sensor.begin();

    if (EEPromLayout::read("BNOC", calib_data, sizeof(calib_data)) == sizeof(calib_data))
    {
        Serial.println("Setting calibration constants from old EEPROM data");
        sensor.setSensorOffsets(calib_data);
    }
}

void loop()
{
    uint8_t system, gyro, accel, mag;
    sensor.getCalibration(&system, &gyro, &accel, &mag);
    Serial.print("system: ");
    Serial.print(system);
    Serial.print(", gyro: ");
    Serial.print(gyro);
    Serial.print(", accel: ");
    Serial.print(accel);
    Serial.print(", mag: ");
    Serial.println(mag);
    if (system == 3 && gyro == 3 && accel == 3 && mag == 3)
    {
        sensor.getSensorOffsets(calib_data);
        Serial.print("accel x: ");
        Serial.print(calib_data[0] | (calib_data[1] << 8));
        Serial.print(", y: ");
        Serial.print(calib_data[2] | (calib_data[3] << 8));
        Serial.print(", z: ");
        Serial.println(calib_data[4] | (calib_data[5] << 8));

        Serial.print("mag x: ");
        Serial.print(calib_data[6] | (calib_data[7] << 8));
        Serial.print(", y: ");
        Serial.print(calib_data[8] | (calib_data[9] << 8));
        Serial.print(", z: ");
        Serial.println(calib_data[10] | (calib_data[11] << 8));

        Serial.print("gyro x: ");
        Serial.print(calib_data[12] | (calib_data[13] << 8));
        Serial.print(", y: ");
        Serial.print(calib_data[14] | (calib_data[15] << 8));
        Serial.print(", z: ");
        Serial.println(calib_data[16] | (calib_data[17] << 8));

        Serial.print("accel radius: ");
        Serial.print(calib_data[18] | (calib_data[19] << 8));
        Serial.print(", mag radius: ");
        Serial.println(calib_data[20] | (calib_data[21] << 8));

        if (!stored)
        {
            if (EEPromLayout::write("BNOC", calib_data, sizeof(calib_data)))
            {
                Serial.println("Values stored in EEPROM");
                stored = true;
            }
            else
            {
                Serial.println("Failed to write values to EEPROM");
            }
        }
    }

    delay(1000);
}
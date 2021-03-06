/***************************************************************************
  This is a library for the BNO055 orientation sensor

  Designed specifically to work with the Adafruit BNO055 Breakout.

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products

  These sensors use I2C to communicate, 2 pins are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by KTOWN for Adafruit Industries.

  MIT license, all text above must be included in any redistribution
 ***************************************************************************/

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <math.h>
#include <limits.h>

#include "bno055.h"

/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/

/**************************************************************************/
/*!
    @brief  Sets up the HW
*/
/**************************************************************************/
bool Adafruit_BNO055::begin(adafruit_bno055_opmode_t mode)
{
    /* Enable I2C */
    Wire.begin();

    /* Make sure we have the right device */
    uint8_t id = read8(BNO055_CHIP_ID_ADDR);
    if (id != BNO055_ID)
    {
        delay(1000); // hold on for boot
        id = read8(BNO055_CHIP_ID_ADDR);
        if (id != BNO055_ID)
            return false;  // still not? ok bail
    }

    /* Switch to config mode (just in case since this is the default) */
    setMode(OPERATION_MODE_CONFIG);

    /* Reset */
    write8(BNO055_SYS_TRIGGER_ADDR, 0x20);
    while (read8(BNO055_CHIP_ID_ADDR) != BNO055_ID)
        delay(10);
    delay(50);

    /* Set to normal power mode */
    write8(BNO055_PWR_MODE_ADDR, POWER_MODE_NORMAL);
    delay(10);

    write8(BNO055_PAGE_ID_ADDR, 0);

    /* Set the output units */
    /*
    uint8_t unitsel = (0 << 7) | // Orientation = Android
                        (0 << 4) | // Temperature = Celsius
                        (0 << 2) | // Euler = Degrees
                        (1 << 1) | // Gyro = Rads
                        (0 << 0);  // Accelerometer = m/s^2
    write8(BNO055_UNIT_SEL_ADDR, unitsel);
    */

    /* Configure axis mapping (see section 3.4) */
    /*
    write8(BNO055_AXIS_MAP_CONFIG_ADDR, REMAP_CONFIG_P2); // P0-P7, Default is P1
    delay(10);
    write8(BNO055_AXIS_MAP_SIGN_ADDR, REMAP_SIGN_P2); // P0-P7, Default is P1
    delay(10);
    */

    write8(BNO055_SYS_TRIGGER_ADDR, 0x0);
    delay(10);
    /* Set the requested operating mode (see section 3.3) */
    setMode(mode);
    delay(20);

    return true;
}

/**************************************************************************/
/*!
    @brief  Puts the chip in the specified operating mode
*/
/**************************************************************************/
void Adafruit_BNO055::setMode(adafruit_bno055_opmode_t mode)
{
    _mode = mode;
    write8(BNO055_OPR_MODE_ADDR, _mode);
    delay(30);
}

/**************************************************************************/
/*!
    @brief  Use the external 32.768KHz crystal
*/
/**************************************************************************/
void Adafruit_BNO055::setExtCrystalUse(boolean usextal)
{
    adafruit_bno055_opmode_t modeback = _mode;

    /* Switch to config mode (just in case since this is the default) */
    setMode(OPERATION_MODE_CONFIG);
    delay(25);
    write8(BNO055_PAGE_ID_ADDR, 0);
    write8(BNO055_SYS_TRIGGER_ADDR, usextal ? 0x80 : 0x00);
    delay(10);
    /* Set the requested operating mode (see section 3.3) */
    setMode(modeback);
    delay(20);
}


/**************************************************************************/
/*!
    @brief  Gets the latest system status info
*/
/**************************************************************************/
void Adafruit_BNO055::getSystemStatus(uint8_t *system_status,
    uint8_t *self_test_result, uint8_t *system_error)
{
    write8(BNO055_PAGE_ID_ADDR, 0);

    if (system_status)
        *system_status = read8(BNO055_SYS_STAT_ADDR);

    if (self_test_result)
        *self_test_result = read8(BNO055_SELFTEST_RESULT_ADDR);

    if (system_error)
        *system_error = read8(BNO055_SYS_ERR_ADDR);

    delay(200);
}

void Adafruit_BNO055::getRevInfo(adafruit_bno055_rev_info_t* info) const
{
    /* Check the accelerometer revision */
    info->accel_rev = read8(BNO055_ACCEL_REV_ID_ADDR);

    /* Check the magnetometer revision */
    info->mag_rev = read8(BNO055_MAG_REV_ID_ADDR);

    /* Check the gyroscope revision */
    info->gyro_rev = read8(BNO055_GYRO_REV_ID_ADDR);

    /* Check the SW revision */
    info->bl_rev = read8(BNO055_BL_REV_ID_ADDR);

    uint8_t a, b;
    a = read8(BNO055_SW_REV_ID_LSB_ADDR);
    b = read8(BNO055_SW_REV_ID_MSB_ADDR);
    info->sw_rev = (((uint16_t)b) << 8) | ((uint16_t)a);
}

/**************************************************************************/
/*!
    @brief  Gets current calibration state.  Each value should be a uint8_t
            pointer and it will be set to 0 if not calibrated and 3 if
            fully calibrated.
*/
/**************************************************************************/
void Adafruit_BNO055::getCalibration(uint8_t* sys, uint8_t* gyro,
    uint8_t* accel, uint8_t* mag) const
{
    uint8_t calData = read8(BNO055_CALIB_STAT_ADDR);
    if (sys)
        *sys = (calData >> 6) & 0x03;
    if (gyro)
        *gyro = (calData >> 4) & 0x03;
    if (accel)
        *accel = (calData >> 2) & 0x03;
    if (mag)
        *mag = calData & 0x03;
}

/**************************************************************************/
/*!
    @brief  Gets a vector reading from the specified source
*/
/**************************************************************************/
imu::Vector<3> Adafruit_BNO055::getVector(adafruit_vector_type_t type) const
{
    imu::Vector<3> xyz;
    uint8_t buffer[6];
    int16_t x, y, z;

    /* Read vector data (6 bytes) */
    readLen((adafruit_bno055_reg_t)type, buffer, 6);

    x = ((int16_t)buffer[0]) | (((int16_t)buffer[1]) << 8);
    y = ((int16_t)buffer[2]) | (((int16_t)buffer[3]) << 8);
    z = ((int16_t)buffer[4]) | (((int16_t)buffer[5]) << 8);

    /* Convert the value to an appropriate range (section 3.6.4) */
    /* and assign the value to the Vector type */
    switch(type)
    {
        case VECTOR_MAGNETOMETER:
            /* 1uT = 16 LSB */
        case VECTOR_EULER:
            /* 1 degree = 16 LSB */
            xyz[0] = ((double)x)/16.0;
            xyz[1] = ((double)y)/16.0;
            xyz[2] = ((double)z)/16.0;
            break;
        case VECTOR_GYROSCOPE:
            /* 1rps = 900 LSB */
            xyz[0] = ((double)x)/900.0;
            xyz[1] = ((double)y)/900.0;
            xyz[2] = ((double)z)/900.0;
            break;
        case VECTOR_ACCELEROMETER:
        case VECTOR_LINEARACCEL:
        case VECTOR_GRAVITY:
            /* 1m/s^2 = 100 LSB */
            xyz[0] = ((double)x)/100.0;
            xyz[1] = ((double)y)/100.0;
            xyz[2] = ((double)z)/100.0;
            break;
    }

    return xyz;
}

imu::Quaternion Adafruit_BNO055::getQuat() const
{
    uint8_t buffer[8];
    int16_t x, y, z, w;

    /* Read quat data (8 bytes) */
    readLen(BNO055_QUATERNION_DATA_W_LSB_ADDR, buffer, 8);
    w = (((uint16_t)buffer[1]) << 8) | ((uint16_t)buffer[0]);
    x = (((uint16_t)buffer[3]) << 8) | ((uint16_t)buffer[2]);
    y = (((uint16_t)buffer[5]) << 8) | ((uint16_t)buffer[4]);
    z = (((uint16_t)buffer[7]) << 8) | ((uint16_t)buffer[6]);

    /* Assign to Quaternion
     * See http://ae-bst.resource.bosch.com/media/products/dokumente/bno055/BST_BNO055_DS000_12~1.pdf
     * 3.6.5.5 Orientation (Quaternion)
     */
    const double scale = 1.0 / (1<<14);
    return imu::Quaternion(scale*w, scale*x, scale*y, scale*z);
}

void Adafruit_BNO055::getSensor(sensor_t *sensor)
{
    /* Insert the sensor name in the fixed length char array */
    strncpy(sensor->name, "BNO055", sizeof(sensor->name));
    sensor->version = 1;
    sensor->sensor_id = _sensorID;
    sensor->type = SENSOR_TYPE_ORIENTATION;
    sensor->max_value = 0.0f;
    sensor->min_value = 0.0f;
    sensor->resolution = 0.01f;
    sensor->min_delay = 0;
}

bool Adafruit_BNO055::getEvent(sensors_event_t *event)
{
    /* Clear the event */
    memset(event, 0, sizeof(*event));

    event->version = sizeof(sensors_event_t);
    event->sensor_id = _sensorID;
    event->type = SENSOR_TYPE_ORIENTATION;
    event->timestamp = millis();

    /* Get a Euler angle sample for orientation */
    imu::Vector<3> euler = getVector(Adafruit_BNO055::VECTOR_EULER);
    event->orientation.x = euler.x();
    event->orientation.y = euler.y();
    event->orientation.z = euler.z();

    return true;
}

bool Adafruit_BNO055::getSensorOffsets(uint8_t* calib_data)
{
    if (!isFullyCalibrated())
        return false;

    adafruit_bno055_opmode_t lastMode = _mode;
    setMode(OPERATION_MODE_CONFIG);
    readLen(ACCEL_OFFSET_X_LSB_ADDR, calib_data, NUM_BNO055_OFFSET_REGISTERS);
    setMode(lastMode);
    return true;
}

bool Adafruit_BNO055::getSensorOffsets(adafruit_bno055_offsets_t& offsets)
{
    if (!isFullyCalibrated())
        return false;

    adafruit_bno055_opmode_t lastMode = _mode;
    setMode(OPERATION_MODE_CONFIG);
    delay(25);

    offsets.accel_offset_x = (read8(ACCEL_OFFSET_X_MSB_ADDR) << 8) | (read8(ACCEL_OFFSET_X_LSB_ADDR));
    offsets.accel_offset_y = (read8(ACCEL_OFFSET_Y_MSB_ADDR) << 8) | (read8(ACCEL_OFFSET_Y_LSB_ADDR));
    offsets.accel_offset_z = (read8(ACCEL_OFFSET_Z_MSB_ADDR) << 8) | (read8(ACCEL_OFFSET_Z_LSB_ADDR));

    offsets.gyro_offset_x = (read8(GYRO_OFFSET_X_MSB_ADDR) << 8) | (read8(GYRO_OFFSET_X_LSB_ADDR));
    offsets.gyro_offset_y = (read8(GYRO_OFFSET_Y_MSB_ADDR) << 8) | (read8(GYRO_OFFSET_Y_LSB_ADDR));
    offsets.gyro_offset_z = (read8(GYRO_OFFSET_Z_MSB_ADDR) << 8) | (read8(GYRO_OFFSET_Z_LSB_ADDR));

    offsets.mag_offset_x = (read8(MAG_OFFSET_X_MSB_ADDR) << 8) | (read8(MAG_OFFSET_X_LSB_ADDR));
    offsets.mag_offset_y = (read8(MAG_OFFSET_Y_MSB_ADDR) << 8) | (read8(MAG_OFFSET_Y_LSB_ADDR));
    offsets.mag_offset_z = (read8(MAG_OFFSET_Z_MSB_ADDR) << 8) | (read8(MAG_OFFSET_Z_LSB_ADDR));

    offsets.accel_radius = (read8(ACCEL_RADIUS_MSB_ADDR) << 8) | (read8(ACCEL_RADIUS_LSB_ADDR));
    offsets.mag_radius = (read8(MAG_RADIUS_MSB_ADDR) << 8) | (read8(MAG_RADIUS_LSB_ADDR));

    setMode(lastMode);
    return true;
}


void Adafruit_BNO055::setSensorOffsets(const uint8_t* calib_data)
{
    adafruit_bno055_opmode_t lastMode = _mode;
    setMode(OPERATION_MODE_CONFIG);
    delay(25);

    /* A writeLen() would make this much cleaner */
    write8(ACCEL_OFFSET_X_LSB_ADDR, calib_data[0]);
    write8(ACCEL_OFFSET_X_MSB_ADDR, calib_data[1]);
    write8(ACCEL_OFFSET_Y_LSB_ADDR, calib_data[2]);
    write8(ACCEL_OFFSET_Y_MSB_ADDR, calib_data[3]);
    write8(ACCEL_OFFSET_Z_LSB_ADDR, calib_data[4]);
    write8(ACCEL_OFFSET_Z_MSB_ADDR, calib_data[5]);

    write8(GYRO_OFFSET_X_LSB_ADDR, calib_data[6]);
    write8(GYRO_OFFSET_X_MSB_ADDR, calib_data[7]);
    write8(GYRO_OFFSET_Y_LSB_ADDR, calib_data[8]);
    write8(GYRO_OFFSET_Y_MSB_ADDR, calib_data[9]);
    write8(GYRO_OFFSET_Z_LSB_ADDR, calib_data[10]);
    write8(GYRO_OFFSET_Z_MSB_ADDR, calib_data[11]);

    write8(MAG_OFFSET_X_LSB_ADDR, calib_data[12]);
    write8(MAG_OFFSET_X_MSB_ADDR, calib_data[13]);
    write8(MAG_OFFSET_Y_LSB_ADDR, calib_data[14]);
    write8(MAG_OFFSET_Y_MSB_ADDR, calib_data[15]);
    write8(MAG_OFFSET_Z_LSB_ADDR, calib_data[16]);
    write8(MAG_OFFSET_Z_MSB_ADDR, calib_data[17]);

    write8(ACCEL_RADIUS_LSB_ADDR, calib_data[18]);
    write8(ACCEL_RADIUS_MSB_ADDR, calib_data[19]);

    write8(MAG_RADIUS_LSB_ADDR, calib_data[20]);
    write8(MAG_RADIUS_MSB_ADDR, calib_data[21]);

    setMode(lastMode);
}

void Adafruit_BNO055::setSensorOffsets(const adafruit_bno055_offsets_t& offsets)
{
    adafruit_bno055_opmode_t lastMode = _mode;
    setMode(OPERATION_MODE_CONFIG);
    delay(25);

    write8(ACCEL_OFFSET_X_LSB_ADDR, (offsets.accel_offset_x) & 0x0FF);
    write8(ACCEL_OFFSET_X_MSB_ADDR, (offsets.accel_offset_x >> 8) & 0x0FF);
    write8(ACCEL_OFFSET_Y_LSB_ADDR, (offsets.accel_offset_y) & 0x0FF);
    write8(ACCEL_OFFSET_Y_MSB_ADDR, (offsets.accel_offset_y >> 8) & 0x0FF);
    write8(ACCEL_OFFSET_Z_LSB_ADDR, (offsets.accel_offset_z) & 0x0FF);
    write8(ACCEL_OFFSET_Z_MSB_ADDR, (offsets.accel_offset_z >> 8) & 0x0FF);

    write8(GYRO_OFFSET_X_LSB_ADDR, (offsets.gyro_offset_x) & 0x0FF);
    write8(GYRO_OFFSET_X_MSB_ADDR, (offsets.gyro_offset_x >> 8) & 0x0FF);
    write8(GYRO_OFFSET_Y_LSB_ADDR, (offsets.gyro_offset_y) & 0x0FF);
    write8(GYRO_OFFSET_Y_MSB_ADDR, (offsets.gyro_offset_y >> 8) & 0x0FF);
    write8(GYRO_OFFSET_Z_LSB_ADDR, (offsets.gyro_offset_z) & 0x0FF);
    write8(GYRO_OFFSET_Z_MSB_ADDR, (offsets.gyro_offset_z >> 8) & 0x0FF);

    write8(MAG_OFFSET_X_LSB_ADDR, (offsets.mag_offset_x) & 0x0FF);
    write8(MAG_OFFSET_X_MSB_ADDR, (offsets.mag_offset_x >> 8) & 0x0FF);
    write8(MAG_OFFSET_Y_LSB_ADDR, (offsets.mag_offset_y) & 0x0FF);
    write8(MAG_OFFSET_Y_MSB_ADDR, (offsets.mag_offset_y >> 8) & 0x0FF);
    write8(MAG_OFFSET_Z_LSB_ADDR, (offsets.mag_offset_z) & 0x0FF);
    write8(MAG_OFFSET_Z_MSB_ADDR, (offsets.mag_offset_z >> 8) & 0x0FF);

    write8(ACCEL_RADIUS_LSB_ADDR, (offsets.accel_radius) & 0x0FF);
    write8(ACCEL_RADIUS_MSB_ADDR, (offsets.accel_radius >> 8) & 0x0FF);

    write8(MAG_RADIUS_LSB_ADDR, (offsets.mag_radius) & 0x0FF);
    write8(MAG_RADIUS_MSB_ADDR, (offsets.mag_radius >> 8) & 0x0FF);

    setMode(lastMode);
}


/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/

bool Adafruit_BNO055::write8(adafruit_bno055_reg_t reg, byte value)
{
    Wire.beginTransmission(_address);
#if ARDUINO >= 100
    Wire.write((uint8_t)reg);
    Wire.write((uint8_t)value);
#else
    Wire.send(reg);
    Wire.send(value);
#endif
    Wire.endTransmission();

    /* XXX TODO: Check for error! */
    return true;
}

byte Adafruit_BNO055::read8(adafruit_bno055_reg_t reg) const
{
    byte value = 0;

    Wire.beginTransmission(_address);
#if ARDUINO >= 100
    Wire.write((uint8_t)reg);
#else
    Wire.send(reg);
#endif
    Wire.endTransmission();
    Wire.requestFrom(_address, (byte)1);
#if ARDUINO >= 100
    value = Wire.read();
#else
    value = Wire.receive();
#endif

  return value;
}

bool Adafruit_BNO055::readLen(adafruit_bno055_reg_t reg, byte* buffer,
    uint8_t len) const
{
    Wire.beginTransmission(_address);
#if ARDUINO >= 100
    Wire.write((uint8_t)reg);
#else
    Wire.send(reg);
#endif
    Wire.endTransmission();
    Wire.requestFrom(_address, (byte)len);

    for (uint8_t i = 0; i < len; i++)
    {
#if ARDUINO >= 100
        buffer[i] = Wire.read();
#else
        buffer[i] = Wire.receive();
#endif
    }

    /* XXX TODO: Check for errors! */
    return true;
}
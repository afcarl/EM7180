/* 
   FullTest.ino: Example sketch for running EM7180 SENtral sensor hub in master mode.

   Adapted from

     https://github.com/kriswiner/EM7180_SENtral_sensor_hub/tree/master/WarmStartandAccelCal

   This file is part of EM7180.

   EM7180 is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   EM7180 is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with EM7180.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "EM7180.h"

#ifdef __MK20DX256__
#include <i2c_t3.h>
#define NOSTOP I2C_NOSTOP
#else
#include <Wire.h>
#define NOSTOP false
#endif

static const uint8_t  ARES           = 8;    // Gs
static const uint16_t GRES           = 2000; // degrees per second
static const uint16_t MRES           = 1000; // microTeslas
static const uint8_t  MAG_RATE       = 100;  // Hz
static const uint16_t ACCEL_RATE     = 200;  // Hz
static const uint16_t GYRO_RATE      = 200;  // Hz
static const uint8_t  BARO_RATE      = 50;   // Hz
static const uint8_t  Q_RATE_DIVISOR = 3;    // 1/3 gyro rate
 
EM7180 em7180 = EM7180(ARES, GRES, MRES, MAG_RATE, ACCEL_RATE, GYRO_RATE, BARO_RATE, Q_RATE_DIVISOR);

void setup()
{
#ifdef __MK20DX256__
    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);
#else
    Wire.begin();
#endif

    delay(100);

    Serial.begin(115200);

    // Start the EM7180 in master mode, polling instead of interrupt
    if (!em7180.begin()) {

        while (true) {
            Serial.println(em7180.getErrorString());
        }
    }
}

void loop()
{  
    em7180.checkEventStatus();

    if (em7180.gotError()) {
        Serial.print("ERROR: ");
        Serial.println(em7180.getErrorString());
        return;
    }

    /*
       Define output variables from updated quaternion---these are Tait-Bryan
       angles, commonly used in aircraft orientation.  In this coordinate
       system, the positive z-axis is down toward Earth.  Yaw is the angle
       between Sensor x-axis and Earth magnetic North (or true North if
       corrected for local declination, looking down on the sensor positive
       yaw is counterclockwise.  Pitch is angle between sensor x-axis and
       Earth ground plane, toward the Earth is positive, up toward the sky is
       negative.  Roll is angle between sensor y-axis and Earth ground plane,
       y-axis up is positive roll.  These arise from the definition of the
       homogeneous rotation matrix constructed from q.  Tait-Bryan
       angles as well as Euler angles are non-commutative; that is, the get
       the correct orientation the rotations must be applied in the correct
       order which for this configuration is yaw, pitch, and then roll.  For
       more see http://en.wikipedia.org/wiki/Conversion_between_q_and_Euler_angles 
       which has additional links.
     */

    if (em7180.gotQuaternion()) {

        float qw, qx, qy, qz;

        em7180.readQuaternion(qw, qx, qy, qz);

        float roll  = atan2(2.0f * (qw * qx + qy * qz), qw * qw - qx * qx - qy * qy + qz * qz);
        float pitch = -asin(2.0f * (qx * qz - qw * qy));
        float yaw   = atan2(2.0f * (qx * qy + qw * qz), qw * qw + qx * qx - qy * qy - qz * qz);   

        pitch *= 180.0f / PI;
        yaw   *= 180.0f / PI; 
        yaw   += 13.8f; // Declination at Danville, California is 13 degrees 48 minutes and 47 seconds on 2014-04-04
        if(yaw < 0) yaw   += 360.0f ; // Ensure yaw stays between 0 and 360
        roll  *= 180.0f / PI;

        Serial.print("Quaternion Yaw, Pitch, Roll: ");
        Serial.print(yaw, 2);
        Serial.print(", ");
        Serial.print(pitch, 2);
        Serial.print(", ");
        Serial.println(roll, 2);
    }

    if (em7180.gotAccelerometer()) {
        int16_t ax, ay, az;
        em7180.readAccelerometer(ax, ay, az);
        Serial.print("Accel: ");
        Serial.print(ax);
        Serial.print(", ");
        Serial.print(ay);
        Serial.print(", ");
        Serial.println(az);
    }

    if (em7180.gotGyrometer()) {
        int16_t gx, gy, gz;
        em7180.readGyrometer(gx, gy, gz);
        Serial.print("Gyro: ");
        Serial.print(gx);
        Serial.print(", ");
        Serial.print(gy);
        Serial.print(", ");
        Serial.println(gz);
    }

    /*
       Or define output variable according to the Android system, where
       heading (0 to 360) is defined by the angle between the y-axis and True
       North, pitch is rotation about the x-axis (-180 to +180), and roll is
       rotation about the y-axis (-90 to +90) In this systen, the z-axis is
       pointing away from Earth, the +y-axis is at the "top" of the device
       (cellphone) and the +x-axis points toward the right of the device.
     */ 

    if (em7180.gotBarometer()) 
    {
        float temperature, pressure;

        em7180.readBarometer(pressure, temperature);

        Serial.println("Baro:");
        Serial.print("  Altimeter temperature = "); 
        Serial.print( temperature, 2); 
        Serial.println(" C"); 
        Serial.print("  Altimeter pressure = "); 
        Serial.print(pressure, 2);  
        Serial.println(" mbar");
        float altitude = (1.0f - powf(pressure / 1013.25f, 0.190295f)) * 44330.0f;
        Serial.print("  Altitude = "); 
        Serial.print(altitude, 2); 
        Serial.println(" m\n");
    }
}



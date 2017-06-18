/* 
   AltitudeFusion.ino: Altitude estimation through baro/accelerometer fusion

   Adapted from

     https://github.com/kriswiner/Teensy_Flight_Controller/blob/master/EM7180_MPU9250_BMP280

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

static EM7180 em7180;

#define UPDATE_HZ 10
#define CALIBRATE_SEC 5
#define WINDOW_SIZE 10

static uint32_t millisPrev;

void setup()
{
#ifdef __MK20DX256__
    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);
#else
    Wire.begin();
#endif

    delay(100);

    Serial.begin(38400);

    // Start the EM710
    uint8_t status = em7180.begin(8, 2000, 1000);
    while (status) {
        Serial.println(EM7180::errorToString(status));
    }

    millisPrev = millis();
}

void loop()
{  
    static float altitudeBaseline;
    static float altitudeWindow[WINDOW_SIZE];
    static uint32_t altitudeCount;
    static uint32_t millisStart;

    uint8_t errorStatus = em7180.poll();

    if (errorStatus) {
        Serial.print("ERROR: ");
        Serial.println(EM7180::errorToString(errorStatus));
        return;
    }

    if (!millisStart) {
        millisStart = millis();
    }

    if ((millis() - millisPrev) > 1000/UPDATE_HZ) { 

        float temperature, pressure;
        em7180.getBaro(pressure, temperature);
        float altitude = 4430769.40f * (1.0f - pow((pressure/1013.25f), 0.190284f));

        if (!altitudeBaseline) {

            Serial.println("Calibrating barometer ...");

            altitudeWindow[altitudeCount] = altitude;
            altitudeCount = (altitudeCount + 1) % WINDOW_SIZE;

            if ((millis() - millisStart) > CALIBRATE_SEC*1000) { 

                for (int k=0; k<WINDOW_SIZE; ++k) {
                    altitudeBaseline += altitudeWindow[k];
                }
                altitudeBaseline /= WINDOW_SIZE;
            }
        }

        else {

            int16_t ax, ay, az;
            em7180.getAccelRaw(ax, ay, az);
            Serial.print("Accel Z: ");
            Serial.println(az);

            Serial.print("Altitude = "); 
            Serial.print(altitude, 2); 
            Serial.print(" cm    Baseline = ");
            Serial.print(altitudeBaseline, 2); 
            Serial.println(" cm\n");
        }

        millisPrev = millis(); 
    }
}
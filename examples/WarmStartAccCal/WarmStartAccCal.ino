/* 
   WamStartAccAcall.ino: IMU calibration

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

// EM7180 SENtral register map
// see http://www.emdeveloper.com/downloads/7180/EMSentral_EM7180_Register_Map_v1_3.pdf
//
#define EM7180_QX                 0x00  // this is a 32-bit normalized floating point number read from registers 0x00-03
#define EM7180_QY                 0x04  // this is a 32-bit normalized floating point number read from registers 0x04-07
#define EM7180_QZ                 0x08  // this is a 32-bit normalized floating point number read from registers 0x08-0B
#define EM7180_QW                 0x0C  // this is a 32-bit normalized floating point number read from registers 0x0C-0F
#define EM7180_QTIME              0x10  // this is a 16-bit unsigned integer read from registers 0x10-11
#define EM7180_MX                 0x12  // int16_t from registers 0x12-13
#define EM7180_MY                 0x14  // int16_t from registers 0x14-15
#define EM7180_MZ                 0x16  // int16_t from registers 0x16-17
#define EM7180_MTIME              0x18  // uint16_t from registers 0x18-19
#define EM7180_AX                 0x1A  // int16_t from registers 0x1A-1B
#define EM7180_AY                 0x1C  // int16_t from registers 0x1C-1D
#define EM7180_AZ                 0x1E  // int16_t from registers 0x1E-1F
#define EM7180_ATIME              0x20  // uint16_t from registers 0x20-21
#define EM7180_GX                 0x22  // int16_t from registers 0x22-23
#define EM7180_GY                 0x24  // int16_t from registers 0x24-25
#define EM7180_GZ                 0x26  // int16_t from registers 0x26-27
#define EM7180_GTIME              0x28  // uint16_t from registers 0x28-29
#define EM7180_Baro               0x2A  // start of two-byte MS5637 pressure data, 16-bit signed interger
#define EM7180_BaroTIME           0x2C  // start of two-byte MS5637 pressure timestamp, 16-bit unsigned
#define EM7180_Temp               0x2E  // start of two-byte MS5637 temperature data, 16-bit signed interger
#define EM7180_TempTIME           0x30  // start of two-byte MS5637 temperature timestamp, 16-bit unsigned
#define EM7180_QRateDivisor       0x32  // uint8_t 
#define EM7180_EnableEvents       0x33
#define EM7180_HostControl        0x34
#define EM7180_EventStatus        0x35
#define EM7180_SensorStatus       0x36
#define EM7180_SentralStatus      0x37
#define EM7180_AlgorithmStatus    0x38
#define EM7180_FeatureFlags       0x39
#define EM7180_ParamAcknowledge   0x3A
#define EM7180_SavedParamByte0    0x3B
#define EM7180_SavedParamByte1    0x3C
#define EM7180_SavedParamByte2    0x3D
#define EM7180_SavedParamByte3    0x3E
#define EM7180_ActualMagRate      0x45
#define EM7180_ActualAccelRate    0x46
#define EM7180_ActualGyroRate     0x47
#define EM7180_ActualBaroRate     0x48
#define EM7180_ActualTempRate     0x49
#define EM7180_ErrorRegister      0x50
#define EM7180_AlgorithmControl   0x54
#define EM7180_MagRate            0x55
#define EM7180_AccelRate          0x56
#define EM7180_GyroRate           0x57
#define EM7180_BaroRate           0x58
#define EM7180_TempRate           0x59
#define EM7180_LoadParamByte0     0x60
#define EM7180_LoadParamByte1     0x61
#define EM7180_LoadParamByte2     0x62
#define EM7180_LoadParamByte3     0x63
#define EM7180_ParamRequest       0x64
#define EM7180_ROMVersion1        0x70
#define EM7180_ROMVersion2        0x71
#define EM7180_RAMVersion1        0x72
#define EM7180_RAMVersion2        0x73
#define EM7180_ProductID          0x90
#define EM7180_RevisionID         0x91
#define EM7180_RunStatus          0x92
#define EM7180_UploadAddress      0x94 // uint16_t registers 0x94 (MSB)-5(LSB)
#define EM7180_UploadData         0x96  
#define EM7180_CRCHost            0x97  // uint32_t from registers 0x97-9A
#define EM7180_ResetRequest       0x9B   
#define EM7180_PassThruStatus     0x9E   
#define EM7180_PassThruControl    0xA0
#define EM7180_ACC_LPF_BW         0x5B  //Register GP36
#define EM7180_GYRO_LPF_BW        0x5C  //Register GP37
#define EM7180_BARO_LPF_BW        0x5D  //Register GP38
#define EM7180_GP36               0x5B
#define EM7180_GP37               0x5C
#define EM7180_GP38               0x5D
#define EM7180_GP39               0x5E
#define EM7180_GP40               0x5F
#define EM7180_GP50               0x69
#define EM7180_GP51               0x6A
#define EM7180_GP52               0x6B
#define EM7180_GP53               0x6C
#define EM7180_GP54               0x6D
#define EM7180_GP55               0x6E
#define EM7180_GP56               0x6F

#define EM7180_ADDRESS           0x28   // Address of the EM7180 SENtral sensor hub
#define M24512DFM_DATA_ADDRESS   0x50   // Address of the 500 page M24512DRC EEPROM data buffer, 1024 bits (128 8-bit bytes) per page

static const float MAGNETIC_DECLINATION =  13.8f; // Declination at Danville, California is 13 degrees 48 minutes and 47 seconds on 2014-04-04

/*************************************************************************************************/
/*************                                                                     ***************/
/*************                 Enumerators and Structure Variables                 ***************/
/*************                                                                     ***************/
/*************************************************************************************************/

struct acc_cal
{
  int16_t accZero_max[3];
  int16_t accZero_min[3];
};

struct Sentral_WS_params
{
  uint8_t Sen_param[35][4];
};

/*************************************************************************************************/
/*************                                                                     ***************/
/*************                        Global Scope Variables                       ***************/
/*************                                                                     ***************/
/*************************************************************************************************/

// General purpose variables
static bool               accel_cal;
static int16_t            accel_cal_saved;
static uint16_t           calibratingA;
static acc_cal            global_conf;
static Sentral_WS_params  WS_params;

#ifdef __MK20DX256__
#include <i2c_t3.h>
#define NOSTOP I2C_NOSTOP
#else
#include <Wire.h>
#define NOSTOP false
#endif

//===================================================================================================================
// I2C read/write functions for the MPU9250 and AK8963 sensors
//===================================================================================================================

static void writeByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
    Wire.beginTransmission(address);  // Initialize the Tx buffer
    Wire.write(subAddress);           // Put slave register address in Tx buffer
    Wire.write(data);                 // Put data in Tx buffer
    Wire.endTransmission();           // Send the Tx buffer
}

static uint8_t readByte(uint8_t address, uint8_t subAddress)
{
    uint8_t data; // `data` will store the register data   
    Wire.beginTransmission(address);         // Initialize the Tx buffer
    Wire.write(subAddress);                  // Put slave register address in Tx buffer
    Wire.endTransmission(NOSTOP);        // Send the Tx buffer, but send a restart to keep connection alive
    Wire.requestFrom(address, (size_t) 1);   // Read one byte from slave register address 
    data = Wire.read();                      // Fill Rx buffer with result
    return data;                             // Return data read from slave register
}

static void readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest)
{  
    Wire.beginTransmission(address);            // Initialize the Tx buffer
    Wire.write(subAddress);                     // Put slave register address in Tx buffer
    Wire.endTransmission(NOSTOP);           // Send the Tx buffer, but send a restart to keep connection alive
    uint8_t i = 0;
    Wire.requestFrom(address, (size_t) count);  // Read bytes from slave register address 
    while (Wire.available())
    {
        dest[i++] = Wire.read();
    }                                           // Put read results in the Rx buffer
}

//===================================================================================================================
//====== Sentral parameter management functions
//===================================================================================================================

static void EM7180_set_gyro_FS (uint16_t gyro_fs)
{
    uint8_t bytes[4], stat;
    bytes[0] = gyro_fs & (0xFF);
    bytes[1] = (gyro_fs >> 8) & (0xFF);
    bytes[2] = 0x00;
    bytes[3] = 0x00;
    writeByte(EM7180_ADDRESS, EM7180_LoadParamByte0, bytes[0]); //Gyro LSB
    writeByte(EM7180_ADDRESS, EM7180_LoadParamByte1, bytes[1]); //Gyro MSB
    writeByte(EM7180_ADDRESS, EM7180_LoadParamByte2, bytes[2]); //Unused
    writeByte(EM7180_ADDRESS, EM7180_LoadParamByte3, bytes[3]); //Unused

    // Parameter 75; 0xCB is 75 decimal with the MSB set high to indicate a paramter write processs
    writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0xCB);

    // Request parameter transfer procedure
    writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x80);

    // Check the parameter acknowledge register and loop until the result matches parameter request byte
    stat = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    while(!(stat==0xCB)) {
        stat = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    }
    writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x00); //Parameter request = 0 to end parameter transfer process
    writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x00); // Re-start algorithm
}

static void EM7180_set_mag_acc_FS (uint16_t mag_fs, uint16_t acc_fs) {
    uint8_t bytes[4], stat;
    bytes[0] = mag_fs & (0xFF);
    bytes[1] = (mag_fs >> 8) & (0xFF);
    bytes[2] = acc_fs & (0xFF);
    bytes[3] = (acc_fs >> 8) & (0xFF);
    writeByte(EM7180_ADDRESS, EM7180_LoadParamByte0, bytes[0]); //Mag LSB
    writeByte(EM7180_ADDRESS, EM7180_LoadParamByte1, bytes[1]); //Mag MSB
    writeByte(EM7180_ADDRESS, EM7180_LoadParamByte2, bytes[2]); //Acc LSB
    writeByte(EM7180_ADDRESS, EM7180_LoadParamByte3, bytes[3]); //Acc MSB

    // Parameter 74; 0xCA is 74 decimal with the MSB set high to indicate a paramter write processs
    writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0xCA);

    //Request parameter transfer procedure
    writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x80);

    // Check the parameter acknowledge register and loop until the result matches parameter request byte
    stat = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    while(!(stat==0xCA)) {
        stat = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    }
    // Parameter request = 0 to end parameter transfer process
    writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x00);
    writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x00); // Re-start algorithm
}

static void EM7180_set_integer_param (uint8_t param, uint32_t param_val)
{
    uint8_t bytes[4], stat;
    bytes[0] = param_val & (0xFF);
    bytes[1] = (param_val >> 8) & (0xFF);
    bytes[2] = (param_val >> 16) & (0xFF);
    bytes[3] = (param_val >> 24) & (0xFF);

    // Parameter is the decimal value with the MSB set high to indicate a paramter write processs
    param = param | 0x80;
    writeByte(EM7180_ADDRESS, EM7180_LoadParamByte0, bytes[0]); //Param LSB
    writeByte(EM7180_ADDRESS, EM7180_LoadParamByte1, bytes[1]);
    writeByte(EM7180_ADDRESS, EM7180_LoadParamByte2, bytes[2]);
    writeByte(EM7180_ADDRESS, EM7180_LoadParamByte3, bytes[3]); //Param MSB
    writeByte(EM7180_ADDRESS, EM7180_ParamRequest, param);

    // Request parameter transfer procedure
    writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x80);

    // Check the parameter acknowledge register and loop until the result matches parameter request byte
    stat = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    while(!(stat==param)) {
        stat = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    }
    // Parameter request = 0 to end parameter transfer process
    writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x00);
    writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x00); // Re-start algorithm
}

static void EM7180_set_WS_params()
{
    uint8_t param = 1;
    uint8_t stat;

    // Parameter is the decimal value with the MSB set high to indicate a paramter write processs
    param = param | 0x80;
    writeByte(EM7180_ADDRESS, EM7180_LoadParamByte0, WS_params.Sen_param[0][0]);
    writeByte(EM7180_ADDRESS, EM7180_LoadParamByte1, WS_params.Sen_param[0][1]);
    writeByte(EM7180_ADDRESS, EM7180_LoadParamByte2, WS_params.Sen_param[0][2]);
    writeByte(EM7180_ADDRESS, EM7180_LoadParamByte3, WS_params.Sen_param[0][3]);
    writeByte(EM7180_ADDRESS, EM7180_ParamRequest, param);

    // Request parameter transfer procedure
    writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x80);

    // Check the parameter acknowledge register and loop until the result matches parameter request byte
    stat = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    while(!(stat==param))
    {
        stat = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    }
    for(uint8_t i=1; i<35; i++)
    {
        param = (i+1) | 0x80;
        writeByte(EM7180_ADDRESS, EM7180_LoadParamByte0, WS_params.Sen_param[i][0]);
        writeByte(EM7180_ADDRESS, EM7180_LoadParamByte1, WS_params.Sen_param[i][1]);
        writeByte(EM7180_ADDRESS, EM7180_LoadParamByte2, WS_params.Sen_param[i][2]);
        writeByte(EM7180_ADDRESS, EM7180_LoadParamByte3, WS_params.Sen_param[i][3]);
        writeByte(EM7180_ADDRESS, EM7180_ParamRequest, param);

        // Check the parameter acknowledge register and loop until the result matches parameter request byte
        stat = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
        while(!(stat==param))
        {
            stat = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
        }
    }
    // Parameter request = 0 to end parameter transfer process
    writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x00);
}

static void EM7180_get_WS_params()
{
    uint8_t param = 1;
    uint8_t stat;

    writeByte(EM7180_ADDRESS, EM7180_ParamRequest, param);
    delay(10);

    // Request parameter transfer procedure
    writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x80);
    delay(10);

    // Check the parameter acknowledge register and loop until the result matches parameter request byte
    stat = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    while(!(stat==param))
    {
        stat = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    }

    // Parameter is the decimal value with the MSB set low (default) to indicate a paramter read processs
    WS_params.Sen_param[0][0] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte0);
    WS_params.Sen_param[0][1] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte1);
    WS_params.Sen_param[0][2] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte2);
    WS_params.Sen_param[0][3] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte3);

    for(uint8_t i=1; i<35; i++)
    {
        param = (i+1);
        writeByte(EM7180_ADDRESS, EM7180_ParamRequest, param);
        delay(10);

        // Check the parameter acknowledge register and loop until the result matches parameter request byte
        stat = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
        while(!(stat==param))
        {
            stat = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
        }
        WS_params.Sen_param[i][0] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte0);
        WS_params.Sen_param[i][1] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte1);
        WS_params.Sen_param[i][2] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte2);
        WS_params.Sen_param[i][3] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte3);
    }
    // Parameter request = 0 to end parameter transfer process
    writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x00);

    // Re-start algorithm
    writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x00);
}

static void EM7180_acc_cal_upload()
{
    int64_t big_cal_num;
    union
    {
        int16_t cal_num;
        unsigned char cal_num_byte[2];
    };

    if (!accel_cal)
    {
        cal_num_byte[0] = 0;
        cal_num_byte[1] = 0;
    } else
    {
        big_cal_num = (4096000000/(global_conf.accZero_max[0] - global_conf.accZero_min[0])) - 1000000;
        cal_num = (int16_t)big_cal_num;
    }
    writeByte(EM7180_ADDRESS, EM7180_GP36, cal_num_byte[0]);
    writeByte(EM7180_ADDRESS, EM7180_GP37, cal_num_byte[1]);

    if (!accel_cal)
    {
        cal_num_byte[0] = 0;
        cal_num_byte[1] = 0;
    } else
    {
        big_cal_num = (4096000000/(global_conf.accZero_max[1] - global_conf.accZero_min[1])) - 1000000;
        cal_num = (int16_t)big_cal_num;
    }
    writeByte(EM7180_ADDRESS, EM7180_GP38, cal_num_byte[0]);
    writeByte(EM7180_ADDRESS, EM7180_GP39, cal_num_byte[1]);  

    if (!accel_cal)
    {
        cal_num_byte[0] = 0;
        cal_num_byte[1] = 0;
    } else
    {
        big_cal_num = (4096000000/(global_conf.accZero_max[2] - global_conf.accZero_min[2])) - 1000000;
        cal_num = (int16_t)big_cal_num;
    }
    writeByte(EM7180_ADDRESS, EM7180_GP40, cal_num_byte[0]);
    writeByte(EM7180_ADDRESS, EM7180_GP50, cal_num_byte[1]);

    if (!accel_cal)
    {
        cal_num_byte[0] = 0;
        cal_num_byte[1] = 0;
    } else
    {
        big_cal_num = (((2048 - global_conf.accZero_max[0]) + (-2048 - global_conf.accZero_min[0]))*100000)/4096;
        cal_num = (int16_t)big_cal_num;
    }
    writeByte(EM7180_ADDRESS, EM7180_GP51, cal_num_byte[0]);
    writeByte(EM7180_ADDRESS, EM7180_GP52, cal_num_byte[1]);

    if (!accel_cal)
    {
        cal_num_byte[0] = 0;
        cal_num_byte[1] = 0;
    } else
    {
        big_cal_num = (((2048 - global_conf.accZero_max[1]) + (-2048 - global_conf.accZero_min[1]))*100000)/4096;
        cal_num = (int16_t)big_cal_num;
    }
    writeByte(EM7180_ADDRESS, EM7180_GP53, cal_num_byte[0]);
    writeByte(EM7180_ADDRESS, EM7180_GP54, cal_num_byte[1]);

    if (!accel_cal)
    {
        cal_num_byte[0] = 0;
        cal_num_byte[1] = 0;
    } else
    {
        big_cal_num = (((2048 - global_conf.accZero_max[2]) + (-2048 - global_conf.accZero_min[2]))*100000)/4096;
        cal_num = -(int16_t)big_cal_num;
    }
    writeByte(EM7180_ADDRESS, EM7180_GP55, cal_num_byte[0]);
    writeByte(EM7180_ADDRESS, EM7180_GP56, cal_num_byte[1]);
}

static void WS_PassThroughMode()
{
    uint8_t stat = 0;

    // First put SENtral in standby mode
    writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x01);
    delay(5);

    // Place SENtral in pass-through mode
    writeByte(EM7180_ADDRESS, EM7180_PassThruControl, 0x01);
    delay(5);
    stat = readByte(EM7180_ADDRESS, EM7180_PassThruStatus);
    while(!(stat & 0x01))
    {
        stat = readByte(EM7180_ADDRESS, EM7180_PassThruStatus);
        delay(5);
    }
}

static void WS_Resume()
{
    uint8_t stat = 0;

    // Cancel pass-through mode
    writeByte(EM7180_ADDRESS, EM7180_PassThruControl, 0x00);
    delay(5);
    stat = readByte(EM7180_ADDRESS, EM7180_PassThruStatus);
    while((stat & 0x01))
    {
        stat = readByte(EM7180_ADDRESS, EM7180_PassThruStatus);
        delay(5);
    }

    // Re-start algorithm
    writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x00);
    delay(5);
    stat = readByte(EM7180_ADDRESS, EM7180_AlgorithmStatus);
    while((stat & 0x01))
    {
        stat = readByte(EM7180_ADDRESS, EM7180_AlgorithmStatus);
        delay(5);
    }
}

static void readSenParams()
{
    uint8_t data[140];
    uint8_t paramnum;
    M24512DFMreadBytes(M24512DFM_DATA_ADDRESS, 0x7f, 0x80, 12, &data[128]); // Page 255
    delay(100);
    M24512DFMreadBytes(M24512DFM_DATA_ADDRESS, 0x7f, 0x00, 128, &data[0]); // Page 254
    for (paramnum = 0; paramnum < 35; paramnum++) // 35 parameters
    {
        for (uint8_t i= 0; i < 4; i++)
        {
            WS_params.Sen_param[paramnum][i] = data[(paramnum*4 + i)];
        }
    }
}

static void writeSenParams()
{
    uint8_t data[140];
    uint8_t paramnum;
    for (paramnum = 0; paramnum < 35; paramnum++) // 35 parameters
    {
        for (uint8_t i= 0; i < 4; i++)
        {
            data[(paramnum*4 + i)] = WS_params.Sen_param[paramnum][i];
        }
    }
    M24512DFMwriteBytes(M24512DFM_DATA_ADDRESS, 0x7f, 0x80, 12, &data[128]); // Page 255
    delay(100);
    M24512DFMwriteBytes(M24512DFM_DATA_ADDRESS, 0x7f, 0x00, 128, &data[0]); // Page 254
}

static void Accel_cal_check(int16_t accelCount[3])
{
    static int64_t a[3] = {0, 0, 0}, b[3] = {0, 0, 0};

    if (calibratingA > 0)
    {
        for (uint8_t axis = 0; axis < 3; axis++)
        {
            if (accelCount[axis] > 1024)
            {
                // Sum up 512 readings
                a[axis] += accelCount[axis];
            }
            if (accelCount[axis] < -1024)
            {
                b[axis] += accelCount[axis];
            }
            // Clear global variables for next reading
            accelCount[axis] = 0;
        }

        // Calculate averages, and store values in EEPROM at end of calibration
        if (calibratingA == 1)
        {
            for (uint8_t axis = 0; axis < 3; axis++)
            {
                if (a[axis]>>9 > 1024)
                {
                    global_conf.accZero_max[axis] = a[axis]>>9;
                }
                if (b[axis]>>9 < -1024)
                {
                    global_conf.accZero_min[axis] = b[axis]>>9;
                }
                a[axis] = 0;
                b[axis] = 0;
            }

            //Write accZero to EEPROM
            delay(100);

            // Put the Sentral in pass-thru mode
            WS_PassThroughMode();

            // Store accelerometer calibration data to the M24512DFM I2C EEPROM
            writeAccCal();


            // Take Sentral out of pass-thru mode and re-start algorithm
            WS_Resume();
            accel_cal_saved++;
            if (accel_cal_saved > 6) accel_cal_saved = 0;
        }
        calibratingA--;
    }
}

static void readAccelCal()
{
    uint8_t data[12];
    uint8_t axis;

    M24512DFMreadBytes(M24512DFM_DATA_ADDRESS, 0x7f, 0x8c, 12, data); // Page 255
    for (axis = 0; axis < 3; axis++)
    {
        global_conf.accZero_max[axis] = ((int16_t)(data[(2*axis + 1)]<<8) | data[2*axis]);
        global_conf.accZero_min[axis] = ((int16_t)(data[(2*axis + 7)]<<8) | data[(2*axis + 6)]);
    }
}

static void writeAccCal()
{
    uint8_t data[12];
    uint8_t axis;
    for (axis = 0; axis < 3; axis++)
    {
        data[2*axis] = (global_conf.accZero_max[axis] & 0xff);
        data[(2*axis + 1)] = (global_conf.accZero_max[axis] >> 8);
        data[(2*axis + 6)] = (global_conf.accZero_min[axis] & 0xff);
        data[(2*axis + 7)] = (global_conf.accZero_min[axis] >> 8);
    }
    M24512DFMwriteBytes(M24512DFM_DATA_ADDRESS, 0x7f, 0x8c, 12, data); // Page 255
}

//===================================================================================================================
//====== Set of useful function to access acceleration. gyroscope, magnetometer, and temperature data
//===================================================================================================================

static float uint32_reg_to_float (uint8_t *buf)
{
    union {
        uint32_t ui32;
        float f;
    } u;

    u.ui32 =     (((uint32_t)buf[0]) +
            (((uint32_t)buf[1]) <<  8) +
            (((uint32_t)buf[2]) << 16) +
            (((uint32_t)buf[3]) << 24));
    return u.f;
}

static void readSENtralAccelData(int16_t * destination)
{
    uint8_t rawData[6];  // x/y/z accel register data stored here
    readBytes(EM7180_ADDRESS, EM7180_AX, 6, &rawData[0]);       // Read the six raw data registers into data array
    destination[0] = (int16_t) (((int16_t)rawData[1] << 8) | rawData[0]);  // Turn the MSB and LSB into a signed 16-bit value
    destination[1] = (int16_t) (((int16_t)rawData[3] << 8) | rawData[2]);  
    destination[2] = (int16_t) (((int16_t)rawData[5] << 8) | rawData[4]); 
}


static void readSENtralQuatData(float * destination)
{
    uint8_t rawData[16];  // x/y/z quaternion register data stored here
    readBytes(EM7180_ADDRESS, EM7180_QX, 16, &rawData[0]);       // Read the sixteen raw data registers into data array
    destination[0] = uint32_reg_to_float (&rawData[0]);
    destination[1] = uint32_reg_to_float (&rawData[4]);
    destination[2] = uint32_reg_to_float (&rawData[8]);
    destination[3] = uint32_reg_to_float (&rawData[12]);  // SENtral stores quats as qx, qy, qz, q0!

}

//===================================================================================================================
//====== I2C Communication Support Functions
//===================================================================================================================

// I2C communication with the M24512DFM EEPROM is a little different from I2C communication with the usual motion sensor
// since the address is defined by two bytes

void M24512DFMwriteByte(uint8_t device_address, uint8_t data_address1, uint8_t data_address2, uint8_t  data)
{
    Wire.beginTransmission(device_address);   // Initialize the Tx buffer
    Wire.write(data_address1);                // Put slave register address in Tx buffer
    Wire.write(data_address2);                // Put slave register address in Tx buffer
    Wire.write(data);                         // Put data in Tx buffer
    Wire.endTransmission();                   // Send the Tx buffer
}

static void M24512DFMwriteBytes(uint8_t device_address, uint8_t data_address1, uint8_t data_address2, uint8_t count, uint8_t * dest)
{
    if (count > 128)
    {
        count = 128;
        Serial.print("Page count cannot be more than 128 bytes!");
    }
    Wire.beginTransmission(device_address);   // Initialize the Tx buffer
    Wire.write(data_address1);                // Put slave register address in Tx buffer
    Wire.write(data_address2);                // Put slave register address in Tx buffer
    for(uint8_t i=0; i < count; i++)
    {
        Wire.write(dest[i]);                    // Put data in Tx buffer
    }
    Wire.endTransmission();                   // Send the Tx buffer
}

static void M24512DFMreadBytes(uint8_t device_address, uint8_t data_address1, uint8_t data_address2, uint8_t count, uint8_t * dest)
{  
    Wire.beginTransmission(device_address);            // Initialize the Tx buffer
    Wire.write(data_address1);                         // Put slave register address in Tx buffer
    Wire.write(data_address2);                         // Put slave register address in Tx buffer
    Wire.endTransmission(NOSTOP);                  // Send the Tx buffer, but send a restart to keep connection alive
    uint8_t i = 0;
    Wire.requestFrom(device_address, (size_t)count);  // Read bytes from slave register address 
    while (Wire.available())
    {
        dest[i++] = Wire.read();
    }                                                   // Put read results in the Rx buffer
}

// simple function to scan for I2C devices on the bus
static void I2Cscan() 
{
    // scan for i2c devices
    byte error, address;
    int nDevices;

    Serial.println("Scanning...");

    nDevices = 0;
    for(address = 1; address < 127; address++ ) 
    {
        // Force serial output
        Serial.flush();

        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.print("I2C device found at address 0x");
            if (address<16) 
                Serial.print("0");
            Serial.print(address,HEX);
            Serial.println("  !");
            nDevices++;
        }
        else if (error==4) 
        {
            Serial.print("Unknow error at address 0x");
            if (address<16) 
                Serial.print("0");
            Serial.println(address,HEX);
        }    
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");
}

static void sensorError(const char * errmsg)
{
    Serial.println(errmsg);
    while (true)
        ;
}

// ======================================================================================

void setup()
{  
#ifdef __MK20DX256__
    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);
#else
    Wire.begin();
#endif

    delay(100);
    Serial.begin(115200);
    delay(1000);

    // should detect SENtral at 0x28
    I2Cscan();

    // Read SENtral device information
    uint16_t ROM1 = readByte(EM7180_ADDRESS, EM7180_ROMVersion1);
    uint16_t ROM2 = readByte(EM7180_ADDRESS, EM7180_ROMVersion2);
    Serial.print("EM7180 ROM Version: 0x"); 
    Serial.print(ROM1, HEX); 
    Serial.print(ROM2, HEX); 
    Serial.println(" Should be: 0xE69");
    uint16_t RAM1 = readByte(EM7180_ADDRESS, EM7180_RAMVersion1);
    uint16_t RAM2 = readByte(EM7180_ADDRESS, EM7180_RAMVersion2);
    Serial.print("EM7180 RAM Version: 0x"); 
    Serial.print(RAM1); 
    Serial.println(RAM2);
    uint8_t PID = readByte(EM7180_ADDRESS, EM7180_ProductID);
    Serial.print("EM7180 ProductID: 0x"); 
    Serial.print(PID, HEX); 
    Serial.println(" Should be: 0x80");
    uint8_t RID = readByte(EM7180_ADDRESS, EM7180_RevisionID);
    Serial.print("EM7180 RevisionID: 0x"); 
    Serial.print(RID, HEX); 
    Serial.println(" Should be: 0x02");

    // Give some time to read the screen
    delay(1000);

    // Check SENtral status, make sure EEPROM upload of firmware was accomplished
    byte stat = (readByte(EM7180_ADDRESS, EM7180_SentralStatus) & 0x01);
    if (readByte(EM7180_ADDRESS, EM7180_SentralStatus) & 0x01)  Serial.println("EEPROM detected on the sensor bus!");
    if (readByte(EM7180_ADDRESS, EM7180_SentralStatus) & 0x02)  Serial.println("EEPROM uploaded config file!");
    if (readByte(EM7180_ADDRESS, EM7180_SentralStatus) & 0x04)  Serial.println("EEPROM CRC incorrect!");
    if (readByte(EM7180_ADDRESS, EM7180_SentralStatus) & 0x08)  Serial.println("EM7180 in initialized state!");
    if (readByte(EM7180_ADDRESS, EM7180_SentralStatus) & 0x10)  Serial.println("No EEPROM detected!");

    int count = 0;
    while(!stat)
    {
        writeByte(EM7180_ADDRESS, EM7180_ResetRequest, 0x01);
        delay(500);  
        count++;  
        stat = (readByte(EM7180_ADDRESS, EM7180_SentralStatus) & 0x01);
        if (readByte(EM7180_ADDRESS, EM7180_SentralStatus) & 0x01)  Serial.println("EEPROM detected on the sensor bus!");
        if (readByte(EM7180_ADDRESS, EM7180_SentralStatus) & 0x02)  Serial.println("EEPROM uploaded config file!");
        if (readByte(EM7180_ADDRESS, EM7180_SentralStatus) & 0x04)  Serial.println("EEPROM CRC incorrect!");
        if (readByte(EM7180_ADDRESS, EM7180_SentralStatus) & 0x08)  Serial.println("EM7180 in initialized state!");
        if (readByte(EM7180_ADDRESS, EM7180_SentralStatus) & 0x10)  Serial.println("No EEPROM detected!");
        if (count > 10) break;
    }

    if (!(readByte(EM7180_ADDRESS, EM7180_SentralStatus) & 0x04))  Serial.println("EEPROM upload successful!");

    // Take user input to choose Warm Start or not...
    Serial.println("Send '1' for Warm Start, '0' for no Warm Start");
    uint8_t serial_input = Serial.read();
    while(!(serial_input == '1') && !(serial_input == '0'))
    {
        serial_input = Serial.read();
        delay(500);
    }

    bool warm_start = (serial_input == '1');

    if (warm_start)
    {
        Serial.println("!!!Warm Start active!!!");

        // Put the Sentral in pass-thru mode
        WS_PassThroughMode();

        // Fetch the WarmStart data from the M24512DFM I2C EEPROM
        readSenParams();

        // Take Sentral out of pass-thru mode and re-start algorithm
        WS_Resume();
    } else
    {
        Serial.println("***No Warm Start***");
    }
    // Take user input to choose Warm Start or not...
    // "2" from the keboard is ASCII "1" which gives integer value 50
    // "0" from the keboard is ASCII "0" which gives integer value 48
    Serial.println("Send '2' to apply Accelerometer Cal, '0' to not apply Accelerometer Cal");
    serial_input = Serial.read();
    while(!(serial_input == '2') && !(serial_input == '0'))
    {
        serial_input = Serial.read();
        delay(500);
    }
    if (serial_input == '2')
    {
        accel_cal = 1;
    } else
    {
        accel_cal = 0;
    }
    if (accel_cal)
    {
        Serial.println("!!!Accel Cal Active!!!");

        // Put the Sentral in pass-thru mode
        WS_PassThroughMode();

        // Fetch the WarmStart data from the M24512DFM I2C EEPROM
        readAccelCal();
        Serial.print("X-acc max: "); Serial.println(global_conf.accZero_max[0]);
        Serial.print("Y-acc max: "); Serial.println(global_conf.accZero_max[1]);
        Serial.print("Z-acc max: "); Serial.println(global_conf.accZero_max[2]);
        Serial.print("X-acc min: "); Serial.println(global_conf.accZero_min[0]);
        Serial.print("Y-acc min: "); Serial.println(global_conf.accZero_min[1]);
        Serial.print("Z-acc min: "); Serial.println(global_conf.accZero_min[2]);

        // Take Sentral out of pass-thru mode and re-start algorithm
        WS_Resume();
    } else
    {
        Serial.println("***No Accel Cal***");
    }
    // Give some time to read the screen
    delay(1000);

    // Set SENtral in initialized state to configure registers
    writeByte(EM7180_ADDRESS, EM7180_HostControl, 0x00); 

    // Load Accel Cal
    if (accel_cal)
    {
        EM7180_acc_cal_upload();
    }

    // Force initialize
    writeByte(EM7180_ADDRESS, EM7180_HostControl, 0x01);

    // Load Warm Start parameters
    if (warm_start)
    {
        EM7180_set_WS_params();
    }

    // Set SENtral in initialized state to configure registers
    writeByte(EM7180_ADDRESS, EM7180_HostControl, 0x00); 

    //Setup LPF bandwidth (BEFORE setting ODR's)
    writeByte(EM7180_ADDRESS, EM7180_ACC_LPF_BW, 0x03); // 41Hz
    writeByte(EM7180_ADDRESS, EM7180_GYRO_LPF_BW, 0x01); // 184Hz

    // Set accel/gyro/mage desired ODR rates
    writeByte(EM7180_ADDRESS, EM7180_QRateDivisor, 0x02); // 100 Hz
    writeByte(EM7180_ADDRESS, EM7180_MagRate, 0x64); // 100 Hz
    writeByte(EM7180_ADDRESS, EM7180_AccelRate, 0x14); // 200/10 Hz
    writeByte(EM7180_ADDRESS, EM7180_GyroRate, 0x14); // 200/10 Hz
    writeByte(EM7180_ADDRESS, EM7180_BaroRate, 0x80 | 0x32);  // set enable bit and set Baro rate to 25 Hz

    // Configure operating mode
    writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x00); // read scale sensor data

    // Enable interrupt to host upon certain events
    // choose host interrupts when any sensor updated (0x40), new gyro data (0x20), new accel data (0x10),
    // new mag data (0x08), quaternions updated (0x04), an error occurs (0x02), or the SENtral needs to be reset(0x01)
    writeByte(EM7180_ADDRESS, EM7180_EnableEvents, 0x07);

    // Enable EM7180 run mode
    writeByte(EM7180_ADDRESS, EM7180_HostControl, 0x01); // set SENtral in normal run mode
    delay(100);

    // EM7180 parameter adjustments
    Serial.println("Beginning Parameter Adjustments");

    // Read sensor default FS values from parameter space
    writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x4A); // Request to read parameter 74
    writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x80); // Request parameter transfer process
    byte param_xfer = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    while(!(param_xfer==0x4A))
    {
        param_xfer = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    }
    uint8_t param[4];
    param[0] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte0);
    param[1] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte1);
    param[2] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte2);
    param[3] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte3);
    uint16_t EM7180_mag_fs = ((int16_t)(param[1]<<8) | param[0]);
    uint16_t EM7180_acc_fs = ((int16_t)(param[3]<<8) | param[2]);
    Serial.print("Magnetometer Default Full Scale Range: +/-"); Serial.print(EM7180_mag_fs); Serial.println("uT");
    Serial.print("Accelerometer Default Full Scale Range: +/-"); Serial.print(EM7180_acc_fs); Serial.println("g");
    writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x4B); // Request to read  parameter 75
    param_xfer = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    while(!(param_xfer==0x4B))
    {
        param_xfer = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    }
    param[0] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte0);
    param[1] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte1);
    param[2] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte2);
    param[3] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte3);
    uint16_t EM7180_gyro_fs = ((int16_t)(param[1]<<8) | param[0]);
    Serial.print("Gyroscope Default Full Scale Range: +/-"); Serial.print(EM7180_gyro_fs); Serial.println("dps");
    writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x00); //End parameter transfer
    writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x00); // re-enable algorithm

    // Disable stillness mode
    EM7180_set_integer_param (0x49, 0x00);

    // Write desired sensor full scale ranges to the EM7180
    EM7180_set_mag_acc_FS (0x3E8, 0x08); // 1000 uT, 8 g
    EM7180_set_gyro_FS (0x7D0); // 2000 dps

    // Read sensor new FS values from parameter space
    writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x4A); // Request to read  parameter 74
    writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x80); // Request parameter transfer process
    param_xfer = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    while(!(param_xfer==0x4A))
    {
        param_xfer = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    }
    param[0] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte0);
    param[1] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte1);
    param[2] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte2);
    param[3] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte3);
    EM7180_mag_fs = ((int16_t)(param[1]<<8) | param[0]);
    EM7180_acc_fs = ((int16_t)(param[3]<<8) | param[2]);
    Serial.print("Magnetometer New Full Scale Range: +/-"); Serial.print(EM7180_mag_fs); Serial.println("uT");
    Serial.print("Accelerometer New Full Scale Range: +/-"); Serial.print(EM7180_acc_fs); Serial.println("g");
    writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x4B); // Request to read  parameter 75
    param_xfer = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    while(!(param_xfer==0x4B))
    {
        param_xfer = readByte(EM7180_ADDRESS, EM7180_ParamAcknowledge);
    }
    param[0] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte0);
    param[1] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte1);
    param[2] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte2);
    param[3] = readByte(EM7180_ADDRESS, EM7180_SavedParamByte3);
    EM7180_gyro_fs = ((int16_t)(param[1]<<8) | param[0]);
    Serial.print("Gyroscope New Full Scale Range: +/-"); Serial.print(EM7180_gyro_fs); Serial.println("dps");
    writeByte(EM7180_ADDRESS, EM7180_ParamRequest, 0x00); //End parameter transfer
    writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x00); // re-enable algorithm

    // Read EM7180 status
    uint8_t runStatus = readByte(EM7180_ADDRESS, EM7180_RunStatus);
    if (runStatus & 0x01) Serial.println(" EM7180 run status = normal mode");
    uint8_t algoStatus = readByte(EM7180_ADDRESS, EM7180_AlgorithmStatus);
    if (algoStatus & 0x01) Serial.println(" EM7180 standby status");
    if (algoStatus & 0x02) Serial.println(" EM7180 algorithm slow");
    if (algoStatus & 0x04) Serial.println(" EM7180 in stillness mode");
    if (algoStatus & 0x08) Serial.println(" EM7180 mag calibration completed");
    if (algoStatus & 0x10) Serial.println(" EM7180 magnetic anomaly detected");
    if (algoStatus & 0x20) Serial.println(" EM7180 unreliable sensor data");
    uint8_t passthruStatus = readByte(EM7180_ADDRESS, EM7180_PassThruStatus);
    if (passthruStatus & 0x01) Serial.print(" EM7180 in passthru mode!");
    uint8_t eventStatus = readByte(EM7180_ADDRESS, EM7180_EventStatus);
    if (eventStatus & 0x01) Serial.println(" EM7180 CPU reset");
    if (eventStatus & 0x02) Serial.println(" EM7180 Error");

    // Give some time to read the screen
    delay(1000);

    // Check sensor status
    uint8_t sensorStatus = readByte(EM7180_ADDRESS, EM7180_SensorStatus);
    if (sensorStatus & 0x01) sensorError("Magnetometer not acknowledging!");
    if (sensorStatus & 0x02) sensorError("Accelerometer not acknowledging!");
    if (sensorStatus & 0x04) sensorError("Gyro not acknowledging!");
    if (sensorStatus & 0x10) sensorError("Magnetometer ID not recognized!");
    if (sensorStatus & 0x20) sensorError("Accelerometer ID not recognized!");
    if (sensorStatus & 0x40) sensorError("Gyro ID not recognized!");

    // Give some time to read the screen
    delay(1000);
}

void loop()
{  
    static bool warm_start_saved;

    // Used to control display output rate
    static uint32_t delt_t;
    static uint32_t count;

    static float Quat[4];

    static float ax;
    static float ay;
    static float az;

    uint8_t serial_input = Serial.read();

    if (serial_input == '1') {

        delay(100);
        EM7180_get_WS_params();

        // Put the Sentral in pass-thru mode
        WS_PassThroughMode();

        // Store WarmStart data to the M24512DFM I2C EEPROM
        writeSenParams();

        // Take Sentral out of pass-thru mode and re-start algorithm
        WS_Resume();
        warm_start_saved = true;
    }

    if (serial_input == '2') {
        calibratingA = 512;
    }

    // Check event status register, way to check data ready by polling rather than interrupt
    uint8_t eventStatus = readByte(EM7180_ADDRESS, EM7180_EventStatus); // reading clears the register

    // Check for errors
    // Error detected, what is it?
    if (eventStatus & 0x02) { 

        uint8_t errorStatus = readByte(EM7180_ADDRESS, EM7180_ErrorRegister);
        if (!errorStatus)
        {
            Serial.print(" EM7180 sensor status = "); Serial.println(errorStatus);
            if (errorStatus == 0x11) Serial.print("Magnetometer failure!");
            if (errorStatus == 0x12) Serial.print("Accelerometer failure!");
            if (errorStatus == 0x14) Serial.print("Gyro failure!");
            if (errorStatus == 0x21) Serial.print("Magnetometer initialization failure!");
            if (errorStatus == 0x22) Serial.print("Accelerometer initialization failure!");
            if (errorStatus == 0x24) Serial.print("Gyro initialization failure!");
            if (errorStatus == 0x30) Serial.print("Math error!");
            if (errorStatus == 0x80) Serial.print("Invalid sample rate!");
        }
        // Handle errors ToDo
    }

    // if no errors, see if new data is ready
    // new acceleration data available
    if (eventStatus & 0x10) { 

        int16_t accelCount[3];

        readSENtralAccelData(accelCount);

        // Now we'll calculate the accleration value into actual g's
        ax = (float)accelCount[0]*0.000488;  // get actual g value
        ay = (float)accelCount[1]*0.000488;    
        az = (float)accelCount[2]*0.000488;

        // Manages accelerometer calibration; is active when calibratingA > 0
        Accel_cal_check(accelCount);
    }

    if (eventStatus & 0x04) {
        readSENtralQuatData(Quat);
    }

    // Serial print and/or display at 0.5 s rate independent of data rates
    delt_t = millis() - count;

    // update LCD once per half-second independent of read rate
    if (delt_t > 500) { 

        float Yaw  = atan2(2.0f * (Quat[0] * Quat[1] + Quat[3] * Quat[2]), Quat[3] * Quat[3] + Quat[0] * Quat[0] - Quat[1] * Quat[1] - Quat[2] * Quat[2]);   
        Yaw   *= 180.0f / PI; 
        Yaw   += MAGNETIC_DECLINATION;
        if (Yaw < 0) Yaw   += 360.0f ; // Ensure yaw stays between 0 and 360

        Serial.print("ax = "); Serial.print((int)1000*ax);  
        Serial.print(" ay = "); Serial.print((int)1000*ay); 
        Serial.print(" az = "); Serial.print((int)1000*az); Serial.println(" mg");
        Serial.print("Hardware yaw: ");
        Serial.println(Yaw, 2);

        if (warm_start_saved) {
            Serial.println("Warm Start configuration saved!");
        } 
        else {
            Serial.println("Send '1' to store Warm Start configuration");
        }
        if (accel_cal_saved > 0) {
            Serial.print("Accel Cals Complete:"); Serial.println(accel_cal_saved);
        } 
        else {
            Serial.println("Send '2' to store Accel Cal");
        }

        count = millis(); 
    }
}


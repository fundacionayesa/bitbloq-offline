/*
 * BitbloqGyro.cpp
 *
 * Copyright 2016 Alberto Valero <alberto.valero@bq.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */
 
/* Includes ------------------------------------------------------------------*/
#include "BitbloqGyro.h"


namespace Bitbloq{

/**
 * Alternate Constructor which can call your own function to map the _AD0 and _INT to arduino port,
 * no pins are used or initialized here
 * \param[in]
 *   _AD0 - arduino gpio number
 * \param[in]
 *   _INT - arduino gpio number
 */
/**
 * Alternate Constructor which can call your own function to map the _AD0 and _INT to arduino port
 * and change the i2c device address, no pins are used or initialized here
 * \param[in]
 *   _AD0 - arduino gpio number
 * \param[in]
 *   _INT - arduino gpio number
 * \param[in]
 *   address - the i2c address you want to set
 */
Gyro::Gyro(uint8_t AD0, uint8_t INT, uint8_t address)
{
  Device_Address = address;
  _AD0 = AD0;
  _INT = INT;
}

/**
 * \par Function
 *   setpin
 * \par Description
 *   Set the PIN of the button module.
 * \param[in]
 *   AD0 - pin mapping for arduino
 * \param[in]
 *   INT - pin mapping for arduino
 * \par Output
 *   None
 * \return
 *   None.
 * \par Others
 *   Set global variable _AD0, _INT, s1 and s2
 */
void Gyro::setpin(uint8_t AD0, uint8_t INT)
{
  _AD0 = AD0;
  _INT = INT;
}

/**
 * \par Function
 *   begin
 * \par Description
 *   Initialize the Gyro.
 * \param[in]
 *   None
 * \par Output
 *   None
 * \return
 *   None
 * \par Others
 *   You can check the MPU6050 datasheet for the registor address.
 */
void Gyro::begin(void)
{
  gSensitivity = 65.5; //for 500 deg/s, check data sheet
  gx = 0;
  gy = 0;
  gz = 0;
  gyrX = 0;
  gyrY = 0;
  gyrZ = 0;
  accX = 0;
  accY = 0;
  accZ = 0;
  gyrXoffs = 0;
  gyrYoffs = 0;
  gyrZoffs = 0;
  Wire.begin();
  delay(800);
  writeReg(0x6b, 0x00);//close the sleep mode
  writeReg(0x1a, 0x01);//configurate the digital low pass filter
  writeReg(0x1b, 0x08);//set the gyro scale to 500 deg/s

  deviceCalibration();
}

/**
 * \par Function
 *   update
 * \par Description
 *   Update some calculated angle values to the variable.
 * \param[in]
 *   None
 * \par Output
 *   None
 * \return
 *   None
 * \par Others
 *   The angle values are calculated by complementary filter.
 *   The time constant of filter is set to 0.5 second, but period dt is not a constant, 
 *   so the filter coefficient will be calculated dynamically.
 */
void Gyro::update(void)
{
  static unsigned long	last_time = 0;
  int8_t return_value;
  double dt, filter_coefficient;
  /* read imu data */
  return_value = readData(0x3b, i2cData, 14);
  if(return_value != 0)
  {
    return;
  }

  double ax, ay, az;
  /* assemble 16 bit sensor data */
  accX = ( (i2cData[0] << 8) | i2cData[1] );
  accY = ( (i2cData[2] << 8) | i2cData[3] );
  accZ = ( (i2cData[4] << 8) | i2cData[5] );  
  gyrX = ( ( (i2cData[8] << 8) | i2cData[9] ) - gyrXoffs) / gSensitivity;
  gyrY = ( ( (i2cData[10] << 8) | i2cData[11] ) - gyrYoffs) / gSensitivity;
  gyrZ = ( ( (i2cData[12] << 8) | i2cData[13] ) - gyrZoffs) / gSensitivity;  
  ax = atan2(accX, sqrt( pow(accY, 2) + pow(accZ, 2) ) ) * 180 / 3.1415926;
  ay = atan2(accY, sqrt( pow(accX, 2) + pow(accZ, 2) ) ) * 180 / 3.1415926;  

  dt = (double)(millis() - last_time) / 1000;
  last_time = millis();

  if(accZ > 0)
  {
    gx = gx - gyrY * dt;
    gy = gy + gyrX * dt;
  }
  else
  {
    gx = gx + gyrY * dt;
    gy = gy - gyrX * dt;
  }
  gz += gyrZ * dt;
  gz = gz - 360 * floor(gz / 360);
  if(gz > 180)
  {
    gz = gz - 360;
  }

  /*
     complementary filter
     set 0.5sec = tau = dt * A / (1 - A)
     so A = tau / (tau + dt)
  */
  filter_coefficient = 0.5 / (0.5 + dt);
  gx = gx * filter_coefficient + ax * (1 - filter_coefficient);
  gy = gy * filter_coefficient + ay * (1 - filter_coefficient);   
}

/**
 * \par Function
 *   fast_update
 * \par Description
 *   Fast update some calculated angle values to the variable.
 * \param[in]
 *   None
 * \par Output
 *   None
 * \return
 *   None
 * \par Others
 *   The angle values are calculated by complementary filter.
 *   The time constant of filter is set to 0.5 second, but period dt is not a constant, 
 *   so the filter coefficient will be calculated dynamically.
 */
void Gyro::fast_update(void)
{
  static unsigned long	last_time = 0;
  int8_t return_value;
  double dt, filter_coefficient;

  dt = (double)(millis() - last_time) / 1000.0;
  last_time = millis();

  /* read imu data */
  return_value = readData(0x3b, i2cData, 14);
  if(return_value != 0)
  {
    return;
  }

  double ax, ay, az;
  /* assemble 16 bit sensor data */
  accX = ( (i2cData[0] << 8) | i2cData[1] );
  accY = ( (i2cData[2] << 8) | i2cData[3] );
  accZ = ( (i2cData[4] << 8) | i2cData[5] );  
  gyrX = ( ( (i2cData[8] << 8) | i2cData[9] ) - gyrXoffs) / gSensitivity;
  gyrY = ( ( (i2cData[10] << 8) | i2cData[11] ) - gyrYoffs) / gSensitivity;
  gyrZ = ( ( (i2cData[12] << 8) | i2cData[13] ) - gyrZoffs) / gSensitivity;  
  ax = atan2(accX, sqrt( pow(accY, 2) + pow(accZ, 2) ) ) * 180 / 3.1415926;
  ay = atan2(accY, sqrt( pow(accX, 2) + pow(accZ, 2) ) ) * 180 / 3.1415926;  

  if(accZ > 0)
  {
    gx = gx - gyrY * dt;
    gy = gy + gyrX * dt;
  }
  else
  {
    gx = gx + gyrY * dt;
    gy = gy - gyrX * dt;
  }
  gz += gyrZ * dt;
  
  gz = gz - 360 * floor(gz / 360);
  if(gz > 180)
  {
    gz = gz - 360;
  }

  gy = 0.98 * gy + 0.02 * ay;
  gx = 0.98 * gx + 0.02 * ax; 
}

/**
 * \par Function
 *   getDevAddr
 * \par Description
 *   Get the device address of Gyro.
 * \param[in]
 *   None
 * \par Output
 *   None
 * \return
 *   The device address of Gyro
 * \par Others
 *   None
 */
uint8_t Gyro::getDevAddr(void)
{
  return Device_Address;
}

/**
 * \par Function
 *   getAngleY
 * \par Description
 *   Get the angle value of X-axis.
 * \param[in]
 *   None
 * \par Output
 *   None
 * \return
 *   The angle value of X-axis
 * \par Others
 *   X-axis angle value is calculated by complementary filter.
 */
double Gyro::getAngleX(void)
{
  return gx;
}

/**
 * \par Function
 *   getAngleY
 * \par Description
 *   Get the angle value of Y-axis.
 * \param[in]
 *   None
 * \par Output
 *   None
 * \return
 *   The angle value of Y-axis
 * \par Others
 *   Y-axis angle value is calculated by complementary filter.
 */
double Gyro::getAngleY(void)
{
  return gy;
}

/**
 * \par Function
 *   getAngleZ
 * \par Description
 *   Get the angle value of Z-axis.
 * \param[in]
 *   None
 * \par Output
 *   None
 * \return
 *   The angle value of Z-axis
 * \par Others
 *   Z-axis angle value is integral of Z-axis angular velocity.
 */
double Gyro::getAngleZ(void)
{
  return gz;
}

/**
 * \par Function
 *   getGyroX
 * \par Description
 *   Get the data of gyroXrate.
 * \param[in]
 *   None
 * \par Output
 *   None
 * \return
 *   The data of gyroXrate
 * \par Others
 *   None
 */
double Gyro::getGyroX(void)
{
  return gyrX;
}

/**
 * \par Function
 *   getGyroY
 * \par Description
 *   Get the data of gyroYrate.
 * \param[in]
 *   None
 * \par Output
 *   None
 * \return
 *   The data of gyroYrate
 * \par Others
 *   None
 */
double Gyro::getGyroY(void)
{
  return gyrY;
}

/**
 * \par Function
 *   getAngle
 * \par Description
 *   Get the angle value of setting axis.
 * \param[in]
 *   index - Axis settings(1:X-axis, 2:Y-axis, 3:Z-axis)
 * \par Output
 *   None
 * \return
 *   The angle value of setting axis
 * \par Others
 *   Z-axis angle value is integral of Z-axis angular velocity.
 */
double Gyro::getAngle(uint8_t index)
{
  if(index == 1)
  {
    return getAngleX();
  }
  else if(index == 2)
  {
    return getAngleY();
  }
  else if(index == 3)
  {
    return getAngleZ();
  }
} 

/**
 * \par Function
 *   deviceCalibration
 * \par Description
 *   Calibration function for the Gyro. 
 * \param[in]
 *   None
 * \par Output
 *   None
 * \return
 *   None.
 * \par Others
 *   The calibration function will be called in initial process, please keep the 
 *   device in a rest status at that time.
 */
void Gyro::deviceCalibration(void)
{
  int8_t return_value;
  uint16_t x = 0;
  uint16_t num = 500;
  long xSum	= 0, ySum = 0, zSum = 0;
  for(x = 0; x < num; x++)
  {
    return_value = readData(0x43, i2cData, 6);
    xSum += ( (i2cData[0] << 8) | i2cData[1] );
    ySum += ( (i2cData[2] << 8) | i2cData[3] );
    zSum += ( (i2cData[4] << 8) | i2cData[5] );
  }
  gyrXoffs = xSum / num;
  gyrYoffs = ySum / num;
  gyrZoffs = zSum / num;
}

/**
 * \par Function
 *   writeReg
 * \par Description
 *   Write the registor of i2c device.
 * \param[in]
 *   reg - the address of registor.
 * \param[in]
 *   data - the data that will be written to the registor.
 * \par Output
 *   None
 * \return
 *   Return the error code.
 *   the definition of the value of variable return_value:
 *   0:success
 *   1:BUFFER_LENGTH is shorter than size
 *   2:address send, nack received
 *   3:data send, nack received
 *   4:other twi error
 *   refer to the arduino official library twi.c
 * \par Others
 *   To set the registor for initializing.
 */
int8_t Gyro::writeReg(int16_t reg, uint8_t data)
{
  int8_t return_value = 0;
  return_value = writeData(reg, &data, 1);
  return(return_value);
}

/**
 * \par Function
 *   readData
 * \par Description
 *   Write the data to i2c device.
 * \param[in]
 *   start - the address which will write the data to.
 * \param[in]
 *   pData - the head address of data array.
 * \param[in]
 *   size - set the number of data will be written to the devide.
 * \par Output
 *   None
 * \return
 *   Return the error code.
 *   the definition of the value of variable return_value:
 *   0:success
 *   1:BUFFER_LENGTH is shorter than size
 *   2:address send, nack received
 *   3:data send, nack received
 *   4:other twi error
 *   refer to the arduino official library twi.c
 * \par Others
 *   Calling the official i2c library to read data.
 */
int8_t Gyro::readData(uint8_t start, uint8_t *buffer, uint8_t size)
{
  int16_t i = 0;
  int8_t return_value = 0;
  Wire.beginTransmission(Device_Address);
  return_value = Wire.write(start);
  if(return_value != 1)
  {
    return(I2C_ERROR);
  }
  return_value = Wire.endTransmission(false);
  if(return_value != 0)
  {
    return(return_value);
  }
  delayMicroseconds(1);
  /* Third parameter is true: relase I2C-bus after data is read. */
  Wire.requestFrom(Device_Address, size, (uint8_t)true);
  while(Wire.available() && i < size)
  {
    buffer[i++] = Wire.read();
  }
  delayMicroseconds(1);
  if(i != size)
  {
    return(I2C_ERROR);
  }
  return(0); //return: no error 
}

/**
 * \par Function
 *   writeData
 * \par Description
 *   Write the data to i2c device.
 * \param[in]
 *   start - the address which will write the data to.
 * \param[in]
 *   pData - the head address of data array.
 * \param[in]
 *   size - set the number of data will be written to the devide.
 * \par Output
 *   None
 * \return
 *   Return the error code.
 *   the definition of the value of variable return_value:
 *   0:success
 *   1:BUFFER_LENGTH is shorter than size
 *   2:address send, nack received
 *   3:data send, nack received
 *   4:other twi error
 *   refer to the arduino official library twi.c
 * \par Others
 *   Calling the official i2c library to write data.
 */
int8_t Gyro::writeData(uint8_t start, const uint8_t *pData, uint8_t size)
{
  int8_t return_value = 0;
  Wire.beginTransmission(Device_Address);
  return_value = Wire.write(start); 
  if(return_value != 1)
  {
    return(I2C_ERROR);
  }
  Wire.write(pData, size);  
  return_value = Wire.endTransmission(true); 
  return(return_value); //return: no error                     
}

} //end namespace

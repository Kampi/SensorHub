/*
 * VEML6070.cpp
 *
 *  Copyright (C) Daniel Kampert, 2020
 *	Website: www.kampis-elektroecke.de
 *  File info: Driver for the VEML6070 I2C UVA light sensor.

  GNU GENERAL PUBLIC LICENSE:
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

  Errors and omissions should be reported to DanielKampert@kampis-elektroecke.de
 */

/** @file VEML6070.cpp
 *  @brief Driver for the VEML6070 I2C UVA light sensor.
 *
 *  @author Daniel Kampert
 */

#include "VEML6070.h"

#define VEML6070_ARA_ADDRESS                    0x18
#define VEML6070_WRITE_ADDRESS                  0x38
#define VEML6070_READ_LSB_ADDRESS               0x38
#define VEML6070_READ_MSB_ADDRESS               0x39

#define VEML6070_ACK                            24

bool VEML6070::initialized(void) const
{
    return this->_mInitialized;
}

VEML6070::VEML6070(void)
{
    this->_mCommand.Bitfield.Reserved = 0x01;
}

VEML6070::~VEML6070(void)
{
}

VEML6070::Error VEML6070::Initialize(void)
{
    pinMode(VEML6070_ACK, INPUT);

    if(!Wire.isEnabled())
    {
	    Wire.begin();
    }

    if(this->_clearAck())
    {
        return this->_mError;
    }

    this->_mInitialized = true;

    return NO_ERROR;
}

VEML6070::Error VEML6070::SetIntegrationTime(VEML6070::IntegrationTime Time)
{
    this->_mCommand.Bitfield.IT = Time;

    return this->_mError;
}

VEML6070::Error VEML6070::Measure(uint16_t* UV)
{
    if(!Wire.isEnabled())
    {
        return NOT_CONNECTED;
    }

    if(!this->_mInitialized)
    {
        return NOT_INITIALIZED;
    }

    if(UV == NULL)
    {
        return INVALID_PARAMETER;
    }

    Wire.requestFrom(VEML6070_READ_LSB_ADDRESS, 1);
	while(!Wire.available());
    *UV = Wire.read();
    *UV <<= 0x08;

    Wire.requestFrom(VEML6070_READ_MSB_ADDRESS, 1);
	while(!Wire.available());
    *UV |= Wire.read();

	return NO_ERROR;
}

VEML6070::Error VEML6070::Sleep(bool Enable)
{
    this->_mCommand.Bitfield.SD = Enable;
    return this->_writeCommand();
}

VEML6070::Error VEML6070::_clearAck(void)
{
    if(!Wire.isEnabled())
    {
        return NOT_CONNECTED;
    }

    Wire.requestFrom(VEML6070_ARA_ADDRESS, 1);
	while(!Wire.available());
    Wire.read();

    return NO_ERROR;
}

VEML6070::Error VEML6070::_writeCommand(void)
{
    if(!Wire.isEnabled())
    {
        return NOT_CONNECTED;
    }

	Wire.beginTransmission(VEML6070_WRITE_ADDRESS);
	Wire.write(this->_mCommand.Register);

	if(Wire.endTransmission())
    {
	    return TRANSMISSION_ERROR;
    }

    return NO_ERROR;
}
/*
 * MCP9808.cpp
 *
 *  Copyright (C) Daniel Kampert, 2020
 *	Website: www.kampis-elektroecke.de
 *  File info: Driver for the MCP9808 I2C temperature sensor.

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

/** @file MCP9808/MCP9808.cpp
 *  @brief Driver for the MCP9808 I2C temperature sensor.
 *
 *  @author Daniel Kampert
 */

#include "MCP9808.h"

#define	MCP9808_REGISTER_CONFIG 	    0x01
#define	MCP9808_REGISTER_ALERT_UPPER 	0x02
#define	MCP9808_REGISTER_ALERT_LOWER	0x03
#define	MCP9808_REGISTER_ALERT_CRIT     0x04
#define	MCP9808_REGISTER_TEMP	        0x05
#define	MCP9808_REGISTER_MANU_ID 	    0x06
#define	MCP9808_REGISTER_DEV_ID 	    0x07
#define	MCP9808_REGISTER_RES 		    0x08

#define MCP9808_MANUFACTURER_ID         0x0054
#define MCP9808_DEVICE_ID               0x04

#define MCP9808_BIT_INT_CLEAR          0x05
#define MCP9808_BIT_ALERT_STAT          0x04
#define MCP9808_BIT_ALTERT_CNT          0x03
#define MCP9808_BIT_ALTERT_SEL          0x02
#define MCP9808_BIT_POLARITY            0x01
#define MCP9808_BIT_ALERT_MOD           0x00

#define MCP9808_ADDRESS                 0x18

bool MCP9808::initialized(void) const
{
    return this->_mInitialized;
}

uint8_t MCP9808::id(void) const
{
    return this->_mID;
}

MCP9808::Error MCP9808::resolution(MCP9808::Resolution* Resolution)
{
    return this->_readBytes(MCP9808_REGISTER_RES, (uint8_t*)Resolution, 1);
}

MCP9808::Error MCP9808::mode(MCP9808::Mode* Mode)
{
    uint8_t Temp[2];

    if(this->_readBytes(MCP9808_REGISTER_CONFIG, Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    *Mode = (MCP9808::Mode)(Temp[0] & 0x01);

    return NO_ERROR;
}

MCP9808::Error MCP9808::hysteresis(MCP9808::Hysteresis* Hysteresis)
{
    uint8_t Temp[2];

    if(this->_readBytes(MCP9808_REGISTER_CONFIG, Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    *Hysteresis = (MCP9808::Hysteresis)((Temp[0] & 0x06) >> 0x01);

    return NO_ERROR;
}

MCP9808::Error MCP9808::trigger(MCP9808::AlertTrigger* Trigger)
{
    uint8_t Temp[2];

    if(this->_readBytes(MCP9808_REGISTER_TEMP, Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    *Trigger = (MCP9808::AlertTrigger)(Temp[0] >> 0x05);

    return NO_ERROR;
}

MCP9808::MCP9808(void)
{
    this->_mInitialized = false;
}

MCP9808::~MCP9808(void)
{
}

MCP9808::Error MCP9808::Initialize(void)
{
    uint8_t Temp[2];

    if(!Wire.isEnabled())
    {
	    Wire.begin();
    }

    this->_readBytes(MCP9808_REGISTER_DEV_ID, Temp, sizeof(Temp));

    this->_mID = Temp[0];
    if(this->_mID != MCP9808_DEVICE_ID)
    {
        return NO_DEVICE;
    }

    this->_mInitialized = true;

    return this->SetMode(CONTINUOUS);
}

MCP9808::Error MCP9808::LockDevice(void)
{
    uint8_t Temp[2];

    if(this->_readBytes(MCP9808_REGISTER_CONFIG, Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    Temp[1] |= 0xC0;

    return this->_writeBytes(MCP9808_REGISTER_CONFIG, Temp, sizeof(Temp));
}

MCP9808::Error MCP9808::UnlockDevice(void)
{
    uint8_t Temp[2];

    if(this->_readBytes(MCP9808_REGISTER_CONFIG, Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    Temp[1] &= ~0xC0;

    return this->_writeBytes(MCP9808_REGISTER_CONFIG, Temp, sizeof(Temp));
}

MCP9808::Error MCP9808::EnableComparator(float Lower, float Upper, float Crit, MCP9808::Polarity Polarity, MCP9808::Hysteresis Hysteresis, bool TCritOnly)
{
    uint8_t Temp[2];

    if(this->_readBytes(MCP9808_REGISTER_CONFIG, Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    if(Temp[1] & 0xC0)
    {
        return DEVICE_LOCKED;
    }

    if(this->_writeTempRegister(MCP9808_REGISTER_ALERT_UPPER, Upper) | this->_writeTempRegister(MCP9808_REGISTER_ALERT_LOWER, Lower) | this->_writeTempRegister(MCP9808_REGISTER_ALERT_CRIT, Crit))
    {
        return this->_mError;
    }

    if(this->_readBytes(MCP9808_REGISTER_CONFIG, Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    Temp[0] |= (Hysteresis & 0x03) << 0x01;
    Temp[1] &= ~((0x01 << MCP9808_BIT_POLARITY) | (0x01 << MCP9808_BIT_ALERT_MOD));
    Temp[1] |= (0x01 << MCP9808_BIT_INT_CLEAR) | (0x01 << MCP9808_BIT_ALERT_STAT) | (0x01 << MCP9808_BIT_ALTERT_CNT) | ((Polarity & 0x01) << MCP9808_BIT_POLARITY);

    if(TCritOnly)
    {
        Temp[1] |= (0x01 << MCP9808_BIT_ALTERT_SEL);
    }
    else
    {
        Temp[1] &= ~(0x01 << MCP9808_BIT_ALTERT_SEL);
    }

    return this->_writeBytes(MCP9808_REGISTER_CONFIG, Temp, sizeof(Temp));
}

MCP9808::Error MCP9808::EnableInterrupt(float Upper, float Lower, MCP9808::Polarity Polarity, MCP9808::Hysteresis Hysteresis)
{
    uint8_t Temp[2];

    if(this->_writeTempRegister(MCP9808_REGISTER_ALERT_UPPER, Upper) | this->_writeTempRegister(MCP9808_REGISTER_ALERT_LOWER, Lower))
    {
        return this->_mError;
    }

    if(this->_readBytes(MCP9808_REGISTER_CONFIG, Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    Temp[1] &= ~(0x01 << MCP9808_BIT_POLARITY);
    Temp[1] |= (0x01 << MCP9808_BIT_INT_CLEAR) | (0x01 << MCP9808_BIT_ALERT_STAT) | (0x01 << MCP9808_BIT_ALTERT_CNT) | ((Polarity & 0x01) << MCP9808_BIT_POLARITY)  | (0x01 << MCP9808_BIT_ALERT_MOD);

    return this->_writeBytes(MCP9808_REGISTER_CONFIG, Temp, sizeof(Temp));
}

MCP9808::Error MCP9808::ClearInterrupt(void)
{
    uint8_t Temp[2];

    if(this->SetMode(MCP9808::CONTINUOUS) || this->_readBytes(MCP9808_REGISTER_CONFIG, Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    Temp[1] |= (0x01 << MCP9808_BIT_INT_CLEAR);

    return this->_writeBytes(MCP9808_REGISTER_CONFIG, Temp, sizeof(Temp));
}

MCP9808::Error MCP9808::SetMode(MCP9808::Mode Mode)
{
    uint8_t Temp[2];

    if(this->UnlockDevice() || this->_readBytes(MCP9808_REGISTER_CONFIG, Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    Temp[0] &= ~0x01;
    Temp[0] |= (Mode & 0x01);

    return this->_writeBytes(MCP9808_REGISTER_CONFIG, Temp, sizeof(Temp));
}

MCP9808::Error MCP9808::SetHysteresis(MCP9808::Hysteresis Hysteresis)
{
    uint8_t Temp[2];

    if(this->_readBytes(MCP9808_REGISTER_CONFIG, Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    Temp[0] &= ~0xF9;
    Temp[0] |= (Hysteresis & 0x01);

    return this->_writeBytes(MCP9808_REGISTER_CONFIG, Temp, sizeof(Temp));
}

MCP9808::Error MCP9808::SetResolution(MCP9808::Resolution Resolution)
{
    uint8_t Temp = Resolution & 0x03;

    return this->_writeBytes(MCP9808_REGISTER_RES, &Temp, sizeof(Temp));
}

MCP9808::Error MCP9808::Measure(float* Temperature)
{
    uint8_t Temp[2];

    // Switch the device into measurement mode
    if(this->SetMode(CONTINUOUS))
    {
        return this->_mError;
    }

    delay(250);

    if(this->_readBytes(MCP9808_REGISTER_TEMP, Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    Temp[0] &= 0x1F;
    if((Temp[0] & 0x10) == 0x10)
    {
        Temp[0] &= 0x0F;
        *Temperature = 256 - (Temp[0] << 0x04) + (Temp[1] >> 0x04);
    }
    else
    {
        *Temperature = (Temp[0] << 0x04) + (Temp[1] >> 0x04);
    }

    if(Temp[1] & 0x0F)
    {
        *Temperature += 1.0 / (Temp[1] & 0x0F);
    }
    else
    {
        *Temperature += 1.0;
    }

    // Switch the device back to sleep mode
    return this->SetMode(SHUTDOWN);

    return NO_ERROR;
}

MCP9808::Error MCP9808::_writeTempRegister(uint8_t Address, float Data)
{
    uint8_t Temp[2];
    uint16_t Integer;
    uint8_t Fractional;

    this->_mError = NO_ERROR;

    if(!Wire.isEnabled())
    {
        return NOT_CONNECTED;
    }

    if(!this->_mInitialized)
    {
        return NOT_INITIALIZED;
    }

    Integer = (uint8_t)Data;
    Fractional = (Data * 100) - (Integer * 100);
    Integer <<= 0x04;
    Temp[0] = (Integer & 0x1F00) >> 0x08;
    Temp[1] = (Integer & 0xFC) | ((Fractional & 0x03) << 0x02);

    return this->_writeBytes(Address, Temp, sizeof(Temp));
}

MCP9808::Error MCP9808::_readBytes(uint8_t Address, uint8_t* Data, uint8_t Length)
{
    this->_mError = NO_ERROR;

    if(!Wire.isEnabled())
    {
        this->_mError = NOT_CONNECTED;
    }

    if(!this->_mInitialized)
    {
        this->_mError = NOT_INITIALIZED;
    }

	Wire.beginTransmission(MCP9808_ADDRESS);
	Wire.write(Address);
	if(Wire.endTransmission())
    {
	    this->_mError = TRANSMISSION_ERROR;
    }

	Wire.requestFrom(MCP9808_ADDRESS, Length);
	while(!Wire.available());

    for(uint8_t i = 0x00; i < Length; i++)
    {
	    Data[i] = Wire.read();
    }
    
	return this->_mError;
}

MCP9808::Error MCP9808::_writeBytes(uint8_t Address, uint8_t* Data, uint8_t Length)
{
    this->_mError = NO_ERROR;

    if(!Wire.isEnabled())
    {
        this->_mError = NOT_CONNECTED;
    }

    if(!this->_mInitialized)
    {
        this->_mError = NOT_INITIALIZED;
    }

	Wire.beginTransmission(MCP9808_ADDRESS);
	Wire.write(Address);

    for(uint8_t i = 0x00; i < Length; i++)
    {
	    Wire.write(Data[i]);
    }

	if(Wire.endTransmission())
    {
	    this->_mError = TRANSMISSION_ERROR;
    }

	return this->_mError;
}
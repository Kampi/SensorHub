/*
 * BH1726.cpp
 *
 *  Copyright (C) Daniel Kampert, 2020
 *	Website: www.kampis-elektroecke.de
 *  File info: Driver for the Rohm I2C ambient light sensor.

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

/** @file BH1726.cpp
 *  @brief Driver for the Rohm I2C ambient light sensor.
 *
 *  @author Daniel Kampert
 */

#include "BH1726.h"

#define BH1726_ID                           0x72

#define BH1716_REGISTER_CONTROL             0x00
#define BH1726_REGISTER_TIMING              0x01
#define BH1726_REGISTER_INTERRUPT           0x02
#define BH1726_REGISTER_TH_LOW              0x03
#define BH1726_REGISTER_TH_HIGH             0x05
#define BH1726_REGISTER_GAIN                0x07
#define BH1726_REGISTER_ID                  0x12
#define BH1726_REGISTER_DATA0               0x14
#define BH1726_REGISTER_DATA1               0x16
#define BH1726_REGISTER_WAIT                0x18

#define BH1726_COMMAND_ADDRESS              0x80
#define BH1726_COMMAND_INT_RESET            0xE1
#define BH1726_COMMAND_SOFT_RESET           0xE4

#define BH1726_BIT_VALID                    0x04
#define BH1726_BIT_INT_EN                   0x04
#define BH1726_BIT_INT_LATCH                0x05

#define BH1726_ADDRESS1                     0x39
#define BH1726_ADDRESS2                     0x29

bool BH1726::initialized(void) const
{
    return this->_mInitialized;
}

bool BH1726::power(void) const
{
    return this->_mActive;
}

BH1726::BH1726(void)
{
    this->_mInitialized = false;
    this->_mActive = false;
    this->_mCallback = NULL;
}

BH1726::~BH1726(void)
{
}

BH1726::Error BH1726::Initialize(bool UseAddr)
{
    uint8_t ID;

    if(UseAddr)
    {
        this->_mAddress = BH1726_ADDRESS1;
    }
    else
    {
        this->_mAddress = BH1726_ADDRESS2;
    }

    if(!Wire.isEnabled())
    {
        Wire.begin();
    }

    this->_mInitialized = true;

    this->_readBytes(BH1726_REGISTER_ID, &ID, sizeof(ID));

    if(ID != BH1726_ID)
    {
        _mInitialized = false;

        return NO_DEVICE;
    }

    return this->Enable();
}

BH1726::Error BH1726::Enable(void)
{
    uint8_t Temp = 0x00;

    if(this->_readBytes(BH1716_REGISTER_CONTROL, &Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    Temp |= 0x03;

    if(this->_writeBytes(BH1716_REGISTER_CONTROL, &Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    this->_mActive = true;

    return this->_mError;
}

BH1726::Error BH1726::Disable(void)
{
    uint8_t Temp = 0x00;

    if(this->_readBytes(BH1716_REGISTER_CONTROL, &Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    Temp &= ~0x03;

    if(this->_writeBytes(BH1716_REGISTER_CONTROL, &Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    this->_mActive = false;

    return this->_mError;
}

BH1726::Error BH1726::SetThreshold(uint16_t Upper, uint16_t Lower, uint8_t Pin, BH1726::Persistence Persistance, BH1726::ThresholdCallback Callback)
{
    return this->SetThreshold(Upper, Lower, Pin, false, false, Persistance, Callback);
}

BH1726::Error BH1726::SetThreshold(uint16_t Upper, uint16_t Lower, uint8_t Pin, bool UseIntPullUp, bool UseLatch, BH1726::Persistence Persistance, BH1726::ThresholdCallback Callback)
{
    if(Callback == NULL)
    {
        return INVALID_PARAMETER;
    }

    uint8_t Low[2] = {(uint8_t)(Lower & 0xFF), (uint8_t)(Lower >> 0x08)};
    uint8_t High[2] = {(uint8_t)(Upper & 0xFF), (uint8_t)(Upper >> 0x08)};

    if(this->_writeBytes(BH1726_REGISTER_TH_LOW, Low, sizeof(Low)) || this->_writeBytes(BH1726_REGISTER_TH_HIGH, High, sizeof(High)))
    {
        return this->_mError;
    }

    if(UseIntPullUp)
    {
        pinMode(Pin, INPUT_PULLUP);
    }
    else
    {
        pinMode(Pin, INPUT);
    }

    this->_mCallback = Callback;
    if(!attachInterrupt(Pin, &BH1726::_callback, this, FALLING))
    {
        return INVALID_PARAMETER;
    }

    Low[0] = (0x01 << BH1726_BIT_INT_EN) | (uint8_t)Persistance;

    if(UseLatch)
    {
        Low[0] |= (0x01 << BH1726_BIT_INT_LATCH);
    }

    return this->_writeBytes(BH1726_REGISTER_INTERRUPT, &Low[0], 1);
}

BH1726::Error BH1726::ClearInterrupt(void)
{
    if(!Wire.isEnabled())
    {
        return NOT_CONNECTED;
    }

	Wire.beginTransmission(this->_mAddress);
	Wire.write(BH1726_COMMAND_INT_RESET);
	if(Wire.endTransmission())
    {
	    this->_mError = TRANSMISSION_ERROR;
    }

    return NO_ERROR;
}

BH1726::Error BH1726::SetGain(BH1726::Gain Gain)
{
    return this->SetGain(Gain, REG_0);
}

BH1726::Error BH1726::SetGain(BH1726::Gain Gain, BH1726::Register Register)
{
    uint8_t Temp = (uint8_t)Gain;

    if(Register & BH1726::REG_0)
    {
        Temp |= Gain << 0x02;
    }

    return this->_writeBytes(BH1726_REGISTER_GAIN, &Temp, sizeof(Temp));
}

BH1726::Error BH1726::SetTime(uint8_t Time)
{
    return this->_writeBytes(BH1726_REGISTER_TIMING, &Time, sizeof(Time));
}

BH1726::Error BH1726::Reset(void)
{
    if(!Wire.isEnabled())
    {
        return NOT_CONNECTED;
    }

	Wire.beginTransmission(this->_mAddress);
	Wire.write(BH1726_COMMAND_SOFT_RESET);
	if(Wire.endTransmission())
    {
	    return TRANSMISSION_ERROR;
    }

    return NO_ERROR;
}

BH1726::Error BH1726::Measure(uint16_t* Light)
{
    uint8_t Temp[2];

    if(Light == NULL)
    {
        return INVALID_PARAMETER;
    }

    if(this->Enable())
    {
        return this->_mError;
    }

    if(this->_wait())
    {
        return this->_mError;
    }

    if(this->_readBytes(BH1726_REGISTER_DATA0, Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    *Light = ((uint16_t)(Temp[1] << 0x08)) | Temp[0];

    return this->Disable();
}

BH1726::Error BH1726::_wait(void)
{
    uint8_t Temp;

    do
    {
        if(this->_readBytes(BH1716_REGISTER_CONTROL, &Temp, sizeof(Temp)))
        {
            return this->_mError;
        }
    } while(!(Temp & (0x01 << BH1726_BIT_VALID)));

    return NO_ERROR;
}

BH1726::Error BH1726::_readBytes(uint8_t Address, uint8_t* Data, uint8_t Length)
{
    this->_mError = NO_ERROR;

    if(!Wire.isEnabled())
    {
        this->_mError = NOT_CONNECTED;
        return this->_mError;
    }

    if(!this->_mInitialized)
    {
        this->_mError = NOT_INITIALIZED;
        return this->_mError;
    }

	Wire.beginTransmission(this->_mAddress);
	Wire.write(BH1726_COMMAND_ADDRESS | Address);
	if(Wire.endTransmission())
    {
	    this->_mError = TRANSMISSION_ERROR;
        return this->_mError;
    }

	Wire.requestFrom(this->_mAddress, Length);
	while(!Wire.available());

    for(uint8_t i = 0x00; i < Length; i++)
    {
	    Data[i] = Wire.read();
    }
    
	return this->_mError;
}

BH1726::Error BH1726::_writeBytes(uint8_t Address, uint8_t* Data, uint8_t Length)
{
    this->_mError = NO_ERROR;

    if(!Wire.isEnabled())
    {
        this->_mError = NOT_CONNECTED;
        return this->_mError;
    }

    if(!this->_mInitialized)
    {
        this->_mError = NOT_INITIALIZED;
        return this->_mError;
    }

	Wire.beginTransmission(this->_mAddress);
	Wire.write(BH1726_COMMAND_ADDRESS | Address);
	if(Wire.endTransmission())
    {
	    this->_mError = TRANSMISSION_ERROR;
    }

	Wire.beginTransmission(this->_mAddress);
	Wire.write(BH1726_COMMAND_ADDRESS | Address);

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

void BH1726::_callback(void)
{
    if(this->_mCallback != NULL)
    {
        this->_mCallback();
    }
}
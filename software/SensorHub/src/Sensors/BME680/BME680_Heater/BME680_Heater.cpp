/*
 * BME680_Heater.cpp
 *
 *  Copyright (C) Daniel Kampert, 2020
 *	Website: www.kampis-elektroecke.de
 *  File info: Heater class for the BME680 I2C environment sensor.

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

/** @file BME680/BME680_Heater.cpp
 *  @brief Heater class for the BME680 I2C environment sensor.
 *
 *  @author Daniel Kampert
 */

#include "BME680_Heater.h"

uint8_t BME680_Heater::index(void) const
{
    return this->_mIndex;
}

uint16_t BME680_Heater::temperature(void) const
{
    return this->_mTemperature;
}

uint16_t BME680_Heater::duration(void) const
{
    return this->_mDuration;
}

uint8_t BME680_Heater::current(void) const
{
    return this->_mCurrent;
}

BME680_Heater::BME680_Heater(uint8_t Index, uint16_t Temperature, uint16_t Duration)
{
    this->_init(Index, Temperature, Duration, 0);
}

BME680_Heater::BME680_Heater(uint8_t Index, uint16_t Temperature, uint16_t Duration, uint8_t Current)
{
    this->_init(Index, Temperature, Duration, Current);
}

void BME680_Heater::_init(uint8_t Index, uint16_t Temperature, uint16_t Duration, uint8_t Current)
{
    if(Index < 0)
    {
        this->_mIndex = 0;
    }
    else if(Index > 9)
    {
        this->_mIndex = 9;
    }
    else
    {
        this->_mIndex = Index;
    }

    if(Temperature < 200)
    {
        this->_mTemperature = 200;
    }
    else if(Temperature > 400)
    {
        this->_mTemperature = 400;
    }
    else
    {
        this->_mTemperature = Temperature;
    }

    if(Duration < 1)
    {
        this->_mDuration = 1;
    }
    else if(Duration > 4032)
    {
        this->_mDuration = 4032;
    }
    else
    {
        this->_mDuration = Duration;
    }

    if(Current < 0)
    {
        this->_mCurrent = 0;
    }
    else if(Current > 16)
    {
        this->_mCurrent = 16;
    }
    else
    {
        this->_mCurrent = Current;
    }
}
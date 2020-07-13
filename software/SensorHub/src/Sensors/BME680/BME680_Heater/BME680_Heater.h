/*
 * BME680_Heater.h
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

/** @file BME680/BME680_Heater.h
 *  @brief Heater class for the BME680 I2C environment sensor.
 *
 *  @author Daniel Kampert
 *  @bug No known bugs
 */

#pragma once

#include <stdio.h>

class BME680_Heater
{
	public:
        /** @brief  Getter for the Heater index.
         *  @return Heater index
         */
        uint8_t index(void) const;

        /** @brief  Getter for the Heater temperature.
         *  @return Heater temperature
         */
        uint16_t temperature(void) const;

        /** @brief  Getter for the Heater duration.
         *  @return Heater duration
         */
        uint16_t duration(void) const;

        /** @brief  Getter for the Heater current.
         *  @return Heater current
         */
        uint8_t current(void) const;

        /** @brief              Constructor.
         *  @param Index        Heater index
         *  @param Temperature  Heater temperature (must be between 200°C and 400°C)
         *  @param Duration     Heater duration (must be between 1 ms and 4032 ms)
         */
        BME680_Heater(uint8_t Index, uint16_t Temperature, uint16_t Duration);

        /** @brief              Constructor.
         *  @param Index        Heater index
         *  @param Temperature  Heater temperature (must be between 200°C and 400°C)
         *  @param Duration     Heater duration (must be between 1 ms and 4032 ms)
         *  @param Current      Heater current (must be between 0 mA and 16 mA)
         */
        BME680_Heater(uint8_t Index, uint16_t Temperature, uint16_t Duration, uint8_t Current);

    private:
        uint8_t _mIndex;
        uint16_t _mTemperature;
        uint16_t _mDuration;
        uint8_t _mCurrent;

        /** @brief              Initialize the Heater values.
         *  @param Index        Heater index
         *  @param Temperature  Heater temperature
         *  @param Duration     Heater duration
         *  @param Current      Heater current
         */
        void _init(uint8_t Index, uint16_t Temperature, uint16_t Duration, uint8_t Current);
};

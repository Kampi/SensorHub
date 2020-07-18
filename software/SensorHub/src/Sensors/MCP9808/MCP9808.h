/*
 * MCP9808.h
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

/** @file MCP9808/MCP9808.h
 *  @brief Driver for the MCP9808 I2C temperature sensor.
 *
 *  @author Daniel Kampert
 *  @bug No known bugs
 */

#pragma once

#include <Wire.h>
#include <math.h>
#include <stdio.h>
#include <application.h>

class MCP9808
{
	public:
        typedef enum
        {
            NO_ERROR = 0x00,
            NO_DEVICE = 0x01,
            NOT_CONNECTED = 0x02,
            DEVICE_LOCKED = 0x03,
            TRANSMISSION_ERROR = 0x04,
            NOT_INITIALIZED = 0x05,
        } Error;

        typedef enum
        {
            CONTINUOUS = 0x00,
            SHUTDOWN = 0x01,
        } Mode;

        typedef enum
        {
            RESOLUTION_LOWEST = 0x00,
            RESOLUTION_LOW = 0x01,
            RESOLUTION_HIGH = 0x02,
            RESOLUTION_HIGHEST = 0x03,
        } Resolution;

        typedef enum
        {
            HYSTERESIS_LOWEST = 0x00,
            HYSTERESIS_LOW = 0x01,
            HYSTERESIS_HIGH = 0x02,
            HYSTERESIS_HIGHEST = 0x03,
        } Hysteresis;

        typedef enum
        {
            POLARITY_LOW = 0x00,
            POLARITY_HIGH = 0x01,
        } Polarity;

        typedef enum
        {
            TA_TLOWER = 0x01,
            TA_TUPPER = 0x02,
            TA_TCRIT = 0x04,
        } AlertTrigger;

        bool initialized(void) const;
        uint8_t id(void) const;
        MCP9808::Error resolution(MCP9808::Resolution* Resolution);
        MCP9808::Error mode(MCP9808::Mode* Mode);
        MCP9808::Error hysteresis(MCP9808::Hysteresis* Hysteresis);
        MCP9808::Error trigger(MCP9808::AlertTrigger* Trigger);

		MCP9808(void);
        ~MCP9808(void);
		MCP9808::Error Initialize(void);
        MCP9808::Error LockDevice(void);
        MCP9808::Error UnlockDevice(void);
        MCP9808::Error EnableComparator(float Lower, float Upper, float Crit, MCP9808::Polarity Polarity = POLARITY_LOW, MCP9808::Hysteresis Hysteresis = HYSTERESIS_LOWEST, bool TCritOnly = false);
        MCP9808::Error EnableInterrupt(float Upper, float Lower, MCP9808::Polarity Polarity = POLARITY_LOW, MCP9808::Hysteresis Hysteresis = HYSTERESIS_LOWEST);
		MCP9808::Error ClearInterrupt(void);
		MCP9808::Error SetMode(MCP9808::Mode Mode);
        MCP9808::Error SetHysteresis(MCP9808::Hysteresis Hysteresis);
		MCP9808::Error SetResolution(MCP9808::Resolution Resolution);
        MCP9808::Error Measure(float* Temperature);

	private:
        uint8_t _mID;

        bool _mInitialized;

        MCP9808::Error _mError;

        MCP9808::Error _writeTempRegister(uint8_t Address, float Data);
        MCP9808::Error _readBytes(uint8_t Address, uint8_t* Data, uint8_t Length);
        MCP9808::Error _writeBytes(uint8_t Address, uint8_t* Data, uint8_t Length);
};

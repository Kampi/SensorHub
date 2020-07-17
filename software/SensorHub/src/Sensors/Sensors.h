/*
 * Sensors.h
 *
 *  Copyright (C) Daniel Kampert, 2020
 *	Website: www.kampis-elektroecke.de
 *  File info: Sensor driver for the SensorHub.

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

#pragma once

#include <Wire.h>
#include <stdio.h>
#include <application.h>
#include "BME680/BME680.h"
#include "BH1726/BH1726.h"
#include "MCP9808/MCP9808.h"
#include "VEML6070/VEML6070.h"

class Sensors
{
	public:
        typedef enum
        {
            NO_ERROR = 0x00,
            TEMP_SENSOR_FAILURE = 0x01,
            LIGHT_SENSOR_FAILURE = 0x02,
            ENV_SENSOR_FAILURE = 0x03,
            UV_SENSOR_FAILURE = 0x04,
            COMMUNICATION_ERROR = 0x05,
        } Error;

        typedef struct
        {
            float Temperature;
            BME680::Data Environment;
            float AmbientLight;
            struct
            {
                float Value;
                bool Valid;
            } IAQ;
            uint8_t UV;
            float SolarVoltage;
            float BatteryVoltage;
        } SensorData;

        static Sensors::Error lastError(void);
        static bool initialized(void);

		static Sensors::Error Initialize(void);
        static Sensors::Error UpdateData(Sensors::SensorData* Data);

	private:
        static bool _mInitialized;

        static uint8_t _mSamples;

        static float _mGasBaseLine;

        static Sensors::Error _mLastError;

        static BH1726 _mBH1726;
        static MCP9808 _mMCP9808;
        static BME680 _mBME680;
        static VEML6070 _mVEML6070;
};

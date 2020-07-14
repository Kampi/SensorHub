/*
 * Sensors.cpp
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

#include "Sensors.h"

#define SOLAR_VOLTAGE               A0
#define TEMPERATURE_ALERT           D3 
#define LIGHT_ALERT                 D2
#define TEMP_BASELINE               21.0f
#define HUM_BASELINE                40.0f
#define TEMP_WEIGHT                 0.10
#define HUM_WEIGHT                  0.10
#define GAS_WEIGHT                  0.80
#define IAQ_SAMPLES                 100

MCP9808 Sensors::_mMCP9808;
BH1726 Sensors::_mBH1726;
BME680 Sensors::_mBME680;
VEML6070 Sensors::_mVEML6070;

Sensors::Error Sensors::_mLastError;

bool Sensors::_mInitialized;

uint8_t Sensors::_mSamples;

float Sensors::_mGasBaseLine;

Sensors::Error Sensors::lastError(void)
{
    return Sensors::_mLastError;
}

bool Sensors::initialized(void)
{
    return Sensors::_mInitialized;
}

Sensors::Error Sensors::Initialize(void)
{
    Sensors::_mInitialized = false;
    Sensors::_mSamples = 0x00;

    if(Sensors::_mMCP9808.Initialize() || Sensors::_mMCP9808.SetResolution(MCP9808::RESOLUTION_HIGHEST))
    {
        Sensors::_mLastError = TEMP_SENSOR_FAILURE;
        return TEMP_SENSOR_FAILURE;
    }

    if(Sensors::_mBH1726.Initialize(false) || Sensors::_mBH1726.SetGain(BH1726::GAIN_X1) || Sensors::_mBH1726.SetTime(BH1726_DEFAULT_TIME))
    {
        Sensors::_mLastError = LIGHT_SENSOR_FAILURE;
        return LIGHT_SENSOR_FAILURE;
    }

    if(Sensors::_mBME680.Initialize(false))
    {
        Sensors::_mLastError = ENV_SENSOR_FAILURE;
        return ENV_SENSOR_FAILURE;
    }

    if(Sensors::_mVEML6070.Initialize() || Sensors::_mVEML6070.SetIntegrationTime(VEML6070::TIME_X1))
    {
        Sensors::_mLastError = UV_SENSOR_FAILURE;
        return UV_SENSOR_FAILURE;
    }

    Sensors::_mInitialized = true;

    return Sensors::UpdateData(NULL);
}

Sensors::Error Sensors::UpdateData(Sensors::SensorData* Data)
{
    uint16_t UV;
    uint16_t AmbientLight;
    BME680_Heater HeaterProfile(0, 320, 200, 0);

    Data->IAQ.Value = 0.0;

    if(!Sensors::_mInitialized)
    {
        Sensors::_mLastError = COMMUNICATION_ERROR;
        return COMMUNICATION_ERROR;
    }

    if(Sensors::_mMCP9808.Measure(&Data->Temperature))
    {
        Sensors::_mLastError = TEMP_SENSOR_FAILURE;
        return TEMP_SENSOR_FAILURE;
    }

    if(Sensors::_mBH1726.Measure(&AmbientLight))
    {
        Sensors::_mLastError = LIGHT_SENSOR_FAILURE;
        return LIGHT_SENSOR_FAILURE;
    }

    Sensors::_mBME680.SetMode(BME680::FORCED);
    if(Sensors::_mBME680.Measure(HeaterProfile, &Data->Environment))
    {
        Sensors::_mLastError = ENV_SENSOR_FAILURE;
        return ENV_SENSOR_FAILURE;
    }

    if(Sensors::_mVEML6070.Measure(&UV))
    {
        Sensors::_mLastError = UV_SENSOR_FAILURE;
        return UV_SENSOR_FAILURE;
    }

    // Wait for a stable sensor output before calculating the baseline
    if(Data->Environment.GasValid == true)
    {
        if(Sensors::_mSamples < IAQ_SAMPLES)
        {
            Sensors::_mSamples++;
            Sensors::_mGasBaseLine += Sensors::_mGasBaseLine / IAQ_SAMPLES;
            Data->IAQ.Valid = false;
        }
        else
        {
            Data->IAQ.Valid = true;
        }

        // Calculate the IAQ index
        // (based on https://forum.iot-usergroup.de/t/indoor-air-quality-index/416/2)
        double TempCoef = 0.0;
        double HumCoef = 0.0;
        double GasCoef = 0.0;
        double TempOffset = Data->Temperature - TEMP_BASELINE;
        double HumOffset = Data->Environment.Humidity - HUM_BASELINE;
        double GasOffset = Sensors::_mGasBaseLine - Data->Environment.GasResistance;

        if(TempOffset > 0.0)
        {
            TempCoef = (100.0 - TEMP_BASELINE - TempOffset) / (100.0 - TEMP_BASELINE) * TEMP_WEIGHT;
        }
        else
        {
            TempCoef = (TEMP_BASELINE + TempOffset) / TEMP_BASELINE * TEMP_WEIGHT;
        }

        if(HumOffset > 0.0)
        {
            HumCoef = (100.0 - HUM_BASELINE - HumOffset) / (100.0 - HUM_BASELINE) * HUM_WEIGHT;
        }
        else
        {
            HumCoef = (HUM_BASELINE + HumOffset) / HUM_BASELINE * HUM_WEIGHT;
        }

        if(GasOffset > 0.0)
        {
            GasCoef = (Data->Environment.GasResistance / Sensors::_mGasBaseLine) * GAS_WEIGHT;
        }
        else
        {
            GasCoef = GAS_WEIGHT;
        }

        Data->IAQ.Value = (TempCoef + HumCoef + GasCoef) * 100.0;

        if(Data->IAQ.Value < 0.0)
        {
            Data->IAQ.Value = 0.0;
        }
        else if(Data->IAQ.Value > 100.0)
        {
            Data->IAQ.Value = 100.0;
        }
    }

    // Convert the UV measurement result into the UV index (approximation from the application note)
    Data->UV = (uint8_t)(0.005 * ((float)UV) - 0.4854);

    if(Data->UV < 0)
    {
        Data->UV = 0;
    }
    else if(Data->UV > 11)
    {
        Data->UV = 11;
    }

    // Convert the ambient light measurment result
    Data->AmbientLight = ((float)AmbientLight) / 4.0;

    // Get the solar voltage
    Data->Voltage = analogRead(A0) * 0.0008 * (122000 / 22000);

    Sensors::_mLastError = NO_ERROR;
    return NO_ERROR;
}
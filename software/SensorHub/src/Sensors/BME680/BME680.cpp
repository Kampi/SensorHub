/*
 * BME680.cpp
 *
 *  Copyright (C) Daniel Kampert, 2020
 *	Website: www.kampis-elektroecke.de
 *  File info: Driver for the BME680 I2C environment sensor.

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

/** @file BME680/BME680.cpp
 *  @brief Driver for the BME680 I2C environment sensor.
 *
 *  @author Daniel Kampert
 */

#include "BME680.h"

#define BME680_REGISTER_CONFIG				0x75
#define BME680_REGISTER_CTRL_MEAS			0x74
#define BME680_REGISTER_STATUS				0x73
#define BME680_REGISTER_CTRL_HUM			0x72
#define BME680_REGISTER_CTRL_GAS_1			0x71
#define BME680_REGISTER_CTRL_GAS_0			0x70
#define BME680_REGISTER_GAS_WAIT_0		    0x64
#define BME680_REGISTER_RES_HEAT_0		    0x5A
#define BME680_REGISTER_IDAC_HEAT_0	        0x50
#define BME680_REGISTER_GAS_R_LSB 			0x2B
#define BME680_REGISTER_GAS_R_MSB 			0x2A
#define BME680_REGISTER_HUM_LSB 			0x26
#define BME680_REGISTER_HUM_MSB 			0x25
#define BME680_REGISTER_TEMP_XLSB 			0x24
#define BME680_REGISTER_TEMP_LSB 			0x23
#define BME680_REGISTER_TEMP_MSB 			0x22
#define BME680_REGISTER_PRESS_XLSB			0x21
#define BME680_REGISTER_PRESS_LSB			0x20
#define BME680_REGISTER_PRESS_MSB			0x1F
#define BME680_REGISTER_EAS_STATUS			0x1D
#define BME680_REGISTER_SOFT_RESET			0xE0
#define BME680_REGISTER_ID					0xD0

#define BME680_BIT_GAS_MEASURE				0x06
#define BME680_BIT_MEASURE					0x05
#define BME680_BIT_RUN_GAS					0x04
#define BME680_BIT_GAS_VALID				0x05
#define BME680_BIT_HEAT_STAB				0x04
#define BME680_BIT_HEAT_OFF 				0x03

#define BME680_ID                           0x61

#define BME680_ADDRESS                      0x76

bool BME680::initialized(void) const
{
    return this->_mInitialized;
}

uint8_t BME680::id(void) const
{
    return this->_mID;
}

BME680::CalibCoef BME680::calibCoef(void) const
{
    return this->_mCalibCoef;
}

BME680::Error BME680::filter(BME680::Filter* Filter)
{
    uint8_t Temp;

    if(!this->_mInitialized)
    {
        return NOT_INITIALIZED;
    }

    if(Filter == NULL)
    {
        return INVALID_PARAMETER;
    }

    if(this->_readBytes(BME680_REGISTER_CONFIG, &Temp, 1))
    {
	    return this->_mError;
    }

    *Filter = (BME680::Filter)(Temp & 0x07);

    return NO_ERROR;
}

BME680::Error BME680::mode(BME680::Mode* Mode)
{
    uint8_t Temp;

    if(!this->_mInitialized)
    {
        return NOT_INITIALIZED;
    }

    if(Mode == NULL)
    {
        return INVALID_PARAMETER;
    }

    if(this->_readBytes(BME680_REGISTER_CTRL_MEAS, &Temp, 1))
    {
	    return TRANSMISSION_ERROR;
    }

    *Mode = (BME680::Mode)(Temp & 0x03);

    return NO_ERROR;
}

BME680::BME680(void)
{
    this->_mInitialized = false;
}

BME680::~BME680(void)
{
}

BME680::Error BME680::Initialize(bool SDO)
{
    _mAddress = BME680_ADDRESS | ((SDO & 0x01) << 0x00);

    if(!Wire.isEnabled())
    {
        Wire.begin();
    }

    _mInitialized = true;

    this->_readBytes(BME680_REGISTER_ID, &_mID, sizeof(_mID));

    if(_mID != BME680_ID)
    {
        _mInitialized = false;

        return NO_DEVICE;
    }

    if(this->_readCalibCoef())
    {
        _mInitialized = false;

        return this->_mError;
    }

    return NO_ERROR;
}

BME680::Error BME680::SwitchHeater(bool Enable)
{
    return this->_writeByte(BME680_REGISTER_CTRL_GAS_0, ((!Enable) & 0x01) << BME680_BIT_HEAT_OFF);
}

BME680::Error BME680::SetMode(BME680::Mode Mode)
{
    uint8_t Temp;

    if(this->_readBytes(BME680_REGISTER_CTRL_MEAS, &Temp, sizeof(Temp)))
    {
	    return this->_mError;
    }

	Temp &= ~0x03;
	Temp |= (Mode & 0x03);

    return this->_writeByte(BME680_REGISTER_CTRL_MEAS, Temp);
}

BME680::Error BME680::SetFilter(BME680::Filter Filter)
{
    uint8_t Temp;

    if(this->_readBytes(BME680_REGISTER_CONFIG, &Temp, sizeof(Temp)))
    {
	    return this->_mError;
    }

	Temp &= ~0x1C;
	Temp |= ((Filter & 0x03) << 0x02);

    return this->_writeByte(BME680_REGISTER_CONFIG, Temp);
}

BME680::Error BME680::MeasureTemperature(float* Temperature)
{
    return this->MeasureTemperature(Temperature, BME680::X1);
}

BME680::Error BME680::MeasureTemperature(float* Temperature, BME680::OSS OSS_Temperature)
{
    uint32_t RawTemp;

    if(Temperature == NULL)
    {
        return INVALID_PARAMETER;
    }

    if(this->_readTemperature(OSS_Temperature, &RawTemp))
    {
        return this->_mError;
    }

    this->_calcTemperature(RawTemp, Temperature);

    return NO_ERROR;
}

BME680::Error BME680::MeasurePressure(float* Pressure)
{
    return this->MeasurePressure(Pressure, BME680::X1, BME680::X1);
}

BME680::Error BME680::MeasurePressure(float* Pressure, BME680::OSS OSS_Pressure)
{
    return this->MeasurePressure(Pressure, OSS_Pressure, BME680::X1);
}

BME680::Error BME680::MeasurePressure(float* Pressure, BME680::OSS OSS_Pressure, BME680::OSS OSS_Temperature)
{
    float Temperature;
    uint32_t RawPressure;

    if(Pressure == NULL)
    {
        return INVALID_PARAMETER;
    }

    if(this->MeasureTemperature(&Temperature) || this->_readPressure(OSS_Pressure, &RawPressure))
    {
        return this->_mError;
    }

    this->_calcPressure(RawPressure, Pressure);

    return NO_ERROR;
}

BME680::Error BME680::MeasureHumidity(float* Humidity)
{
    return this->MeasureHumidity(Humidity, BME680::X1, BME680::X1);
}

BME680::Error BME680::MeasureHumidity(float* Humidity, BME680::OSS OSS_Humidity)
{
    return this->MeasureHumidity(Humidity, OSS_Humidity, BME680::X1);
}

BME680::Error BME680::MeasureHumidity(float* Humidity, BME680::OSS OSS_Humidity, BME680::OSS OSS_Temperature)
{
    float Temperature;
    uint16_t RawHumidity;

    if(Humidity == NULL)
    {
        return INVALID_PARAMETER;
    }

    if(this->MeasureTemperature(&Temperature) || this->_readHumidity(OSS_Humidity, &RawHumidity))
    {
        return this->_mError;
    }

    this->_calcHumidity(RawHumidity, Humidity);

    return NO_ERROR;
}

BME680::Error BME680::MeasureGas(BME680_Heater Profile, float* GasResistance, bool* Valid)
{
    uint16_t GasADC;
    uint8_t GasRange;

    if((GasResistance == NULL) || (Valid == NULL))
    {
        return INVALID_PARAMETER;
    }

    if(this->_readGas(Profile, &GasADC, &GasRange, Valid))
    {
        return this->_mError;
    }

    this->_calcGasResistance(GasADC, GasRange, GasResistance);

    return NO_ERROR;
}

BME680::Error BME680::Measure(BME680_Heater Profile, BME680::Data* Data)
{
    return this->Measure(Profile, Data, BME680::X1, BME680::X1, BME680::X1);
}

BME680::Error BME680::Measure(BME680_Heater Profile, BME680::Data* Data, BME680::OSS OSS_Humidity, BME680::OSS OSS_Pressure, BME680::OSS OSS_Temperature)
{
    uint8_t Temp[8];
    uint32_t RawTemperature;
    uint32_t RawPressure;
    uint16_t RawHumidity;
    uint16_t GasADC;
    uint8_t GasRange;

    if(Data == NULL)
    {
        return INVALID_PARAMETER;
    }

    if(this->_readBytes(BME680_REGISTER_CTRL_MEAS, Temp, 0x01))
    {
        return this->_mError;
    }

	Temp[0] &= 0xFC;
	Temp[0] |= ((OSS_Temperature & 0x07) << 0x05) | ((OSS_Pressure & 0x07) << 0x02);

    if(this->_writeByte(BME680_REGISTER_CTRL_MEAS, Temp[0]) || this->_writeByte(BME680_REGISTER_CTRL_HUM, (OSS_Humidity & 0x07)))
    {
        return this->_mError;
    }

    if(this->_conversionRunning() || this->_readBytes(BME680_REGISTER_PRESS_MSB, Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

	RawPressure = ((uint32_t)Temp[0] << 0x0C) | ((uint32_t)Temp[1] << 0x04) | ((uint32_t)Temp[2] >> 0x04);
	RawTemperature = ((uint32_t)Temp[3] << 0x0C) | ((uint32_t)Temp[4] << 0x04) | ((uint32_t)Temp[5] >> 0x04);
	RawHumidity = ((uint16_t)Temp[6] << 0x08) | (uint16_t)Temp[7];

    if(this->_readGas(Profile, &GasADC, &GasRange, &Data->GasValid))
    {
        return this->_mError;
    }

    this->_calcTemperature(RawTemperature, &Data->Temperature);
    this->_calcPressure(RawPressure, &Data->Pressure);
    this->_calcHumidity(RawHumidity, &Data->Humidity);
    this->_calcGasResistance(GasADC, GasRange, &Data->GasResistance);

    return NO_ERROR;
}

BME680::Error BME680::_readUChar(uint8_t Address, uint8_t* Data)
{
    return this->_readBytes(Address, Data, 0x01);
}

BME680::Error BME680::_readSChar(uint8_t Address, int8_t* Data)
{
    return this->_readUChar(Address, (uint8_t*)Data);
}

BME680::Error BME680::_readUInt(uint8_t Address, uint16_t* Data)
{
	uint8_t Temp[2] = {0x00, 0x00};

    if(this->_readBytes(Address, Temp, sizeof(Temp)))
    {
	    return this->_mError;
    }

	*Data = ((uint16_t)(Temp[0] << 0x08) | ((uint16_t)Temp[1]));

    return NO_ERROR;
}

BME680::Error BME680::_readSInt(uint8_t Address, int16_t* Data)
{
    return this->_readUInt(Address, (uint16_t*)Data);
}

BME680::Error BME680::_readBytes(uint8_t Address, uint8_t* Data, uint8_t Length)
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

	Wire.beginTransmission(_mAddress);
	Wire.write(Address);
	if(Wire.endTransmission())
    {
	    this->_mError = TRANSMISSION_ERROR;
    }

	Wire.requestFrom(_mAddress, Length);
	while(!Wire.available());

    for(uint8_t i = 0x00; i < Length; i++)
    {
	    Data[i] = Wire.read();
    }
    
	return this->_mError;
}

BME680::Error BME680::_writeByte(uint8_t Address, uint8_t Data)
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

	Wire.beginTransmission(_mAddress);
	Wire.write(Address);
	Wire.write(Data);
	if(Wire.endTransmission())
    {
	    this->_mError = TRANSMISSION_ERROR;
    }

	return this->_mError;
}

BME680::Error BME680::_conversionRunning(void)
{
    uint8_t Busy;

    do
    {
        if(this->_readBytes(BME680_REGISTER_EAS_STATUS, &Busy, sizeof(Busy)))
        {
            return this->_mError;
        }
    } while(Busy & (0x01 << BME680_BIT_MEASURE));

	return NO_ERROR;
}

BME680::Error BME680::_gasConversionRunning(void)
{
    uint8_t Busy;

    do
    {
        if(this->_readBytes(BME680_REGISTER_EAS_STATUS, &Busy, sizeof(Busy)))
        {
            return this->_mError;
        }
    } while(Busy & (0x01 << BME680_BIT_GAS_MEASURE));

	return NO_ERROR;
}

BME680::Error BME680::_setHeaterProfile(uint8_t Index)
{
    uint8_t Data;

    if((Index > 0x09) || (Index < 0x00))
    {
        return INVALID_PARAMETER;
    }

	if(this->_readBytes(BME680_REGISTER_CTRL_GAS_1, &Data, sizeof(Data)))
    {
        return this->_mError;
    }
    
	Data &= ~0x0F;
	Data |= Index;

	return this->_writeByte(BME680_REGISTER_CTRL_GAS_1, Data);
}

BME680::Error BME680::_setHeaterTemperature(uint8_t Index, uint16_t Temperature)
{
    float Temp;
    uint32_t RawTemperature;
    int32_t amb_temp;
	uint8_t heatr_res;
	int32_t var1;
	int32_t var2;
	int32_t var3;
	int32_t var4;
	int32_t var5;
	int32_t heatr_res_x100;

    if((Temperature > 400) || (Temperature < 200) || (Index > 9) || (Index < 0))
    {
        return INVALID_PARAMETER;
    }

    if(this->_readTemperature(X16, &RawTemperature))
    {
        return this->_mError;
    }

    this->_calcTemperature(RawTemperature, &Temp);
	amb_temp = (int32_t)(Temperature * 100.0);
	var1 = (((int32_t)amb_temp * this->_mCalibCoef.G3) / 1000) << 0x09;
	var2 = (this->_mCalibCoef.G1 + 784) * (((((this->_mCalibCoef.G2 + 154009) * Temperature * 5) / 100) + 3276800) / 10);
	var3 = var1 + (var2 >> 0x01);
	var4 = (var3 / (this->_mCalibCoef.HeatRange + 4));
	var5 = (131 * this->_mCalibCoef.HeatVal) + 65536;
	heatr_res_x100 = (int32_t)(((var4 / var5) - 250) * 34);
	heatr_res = (uint8_t)((heatr_res_x100 + 50) / 100);

	return this->_writeByte(BME680_REGISTER_RES_HEAT_0 + Index, heatr_res);
}

BME680::Error BME680::_setHeaterDuration(uint8_t Index, uint16_t Duration)
{
	uint8_t Multiplier = 0x00;

    if((Duration > 4032) || (Duration < 1) || (Index > 9) || (Index < 0))
    {
        return INVALID_PARAMETER;
    }

	while(Duration > 63)
    {
		Duration >>= 0x02;
		Multiplier++;
    }

	return this->_writeByte(BME680_REGISTER_GAS_WAIT_0 + Index, (Multiplier << 0x06) | Duration);
}

BME680::Error BME680::_setHeaterCurrent(uint8_t Index, uint8_t Current)
{
    uint8_t CurrentTemp = Current;

    if((Current > 0x10) || (Current < 0x00) || (Index > 0x09) || (Index < 0x00))
    {
        return INVALID_PARAMETER;
    }

    if(CurrentTemp > 0x00)
    {
	    CurrentTemp <<= 0x03;
	    CurrentTemp--;
	    CurrentTemp = ((uint8_t)CurrentTemp) & 0x7F;
    }
    else
    {
        if(this->_readBytes(BME680_REGISTER_TEMP_MSB, &CurrentTemp, sizeof(CurrentTemp)))
        {
            return this->_mError;
        }

        CurrentTemp &= ~0xFE;
    }

	return this->_writeByte(BME680_REGISTER_IDAC_HEAT_0 + Index, CurrentTemp << 0x01);
}

BME680::Error BME680::_readCalibCoef(void)
{
    uint8_t Temp[3];

    if(this->_readBytes(0x8A, (uint8_t*)&this->_mCalibCoef, 23) || this->_readBytes(0xE1, Temp, 3) || this->_readBytes(0xE4, (uint8_t*)&this->_mCalibCoef.H3, 11))
    {
        return this->_mError;
    }

    this->_mCalibCoef.H2 = (Temp[0] << 0x04) | (Temp[1] >> 0x04);
    this->_mCalibCoef.H1 = (Temp[2] << 0x04) | (Temp[1] >> 0x04);

    if(this->_readSChar(0x00, &this->_mCalibCoef.HeatVal) || this->_readSChar(0x02, (int8_t*)&this->_mCalibCoef.HeatRange))
    {
        return this->_mError;
    }

    this->_mCalibCoef.HeatRange >>= 0x04;
    if(this->_readSChar(0x04, &this->_mCalibCoef.RangeSWErr))
    {
        return this->_mError;
    }
    this->_mCalibCoef.RangeSWErr >>= 0x04;

    return NO_ERROR;
}

BME680::Error BME680::_readTemperature(BME680::OSS OSS_Temperature, uint32_t* RawTemp)
{
    uint8_t Temp[3];

    if(this->_readBytes(BME680_REGISTER_CTRL_MEAS, Temp, 0x01))
    {
        return this->_mError;
    }

	Temp[0] &= 0x1F;
	Temp[0] |= (OSS_Temperature & 0x07) << 0x05;

    if(this->_writeByte(BME680_REGISTER_CTRL_MEAS, Temp[0]) || this->_conversionRunning())
    {
        return this->_mError;
    }

    if(this->_readBytes(BME680_REGISTER_TEMP_MSB, Temp, 0x03))
    {
        return this->_mError;
    }

    *RawTemp = ((uint32_t)Temp[0] << 0x0C) | ((uint32_t)Temp[1] << 0x04) | ((uint32_t)Temp[2] >> 0x04);

	return NO_ERROR;
}

BME680::Error BME680::_readPressure(BME680::OSS OSS_Pressure, uint32_t* RawPressure)
{
    uint8_t Temp[3];

    if(this->_readBytes(BME680_REGISTER_CTRL_MEAS, Temp, 0x01))
    {
        return this->_mError;
    }

	Temp[0] &= 0xE3;
	Temp[0] |= (OSS_Pressure & 0x07) << 0x02;

    if(this->_writeByte(BME680_REGISTER_CTRL_MEAS, Temp[0]) || this->_conversionRunning())
    {
        return this->_mError;
    }

    if(this->_readBytes(BME680_REGISTER_PRESS_MSB, Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    *RawPressure = ((uint32_t)Temp[0] << 0x0C) | ((uint32_t)Temp[1] << 0x04) | ((uint32_t)Temp[2] >> 0x04);

	return NO_ERROR;
}

BME680::Error BME680::_readHumidity(BME680::OSS OSS_Humidity, uint16_t* RawHumidity)
{
    uint8_t Temp[2];

    if(this->_writeByte(BME680_REGISTER_CTRL_HUM, (OSS_Humidity & 0x07)) || this->_conversionRunning())
    {
        return this->_mError;
    }

    if(_readBytes(BME680_REGISTER_HUM_MSB, Temp, sizeof(Temp)))
    {
        return this->_mError;
    }

    *RawHumidity = ((uint16_t)Temp[0] << 0x08) | ((uint16_t)Temp[1]);

	return NO_ERROR;
}

BME680::Error BME680::_readGas(BME680_Heater Profile, uint16_t* GasADC, uint8_t* GasRange, bool* Valid)
{
    uint8_t Temp[2];

    if(this->_setHeaterTemperature(Profile.index(), Profile.temperature()) || this->_setHeaterDuration(Profile.index(), Profile.duration()) || this->_setHeaterProfile(Profile.index()))
    {
        return this->_mError;
    }

	if(this->_setHeaterCurrent(Profile.index(), Profile.current()))
    {
        return this->_mError;
    }

	if(this->SwitchHeater(true))
    {
        return this->_mError;
    }

    // Enable gas measuring
    if(this->_readBytes(BME680_REGISTER_CTRL_GAS_1, &Temp[0], 0x01))
    {
        return this->_mError;
    }

	Temp[0] |= (0x01 << BME680_BIT_RUN_GAS);

    if(this->_writeByte(BME680_REGISTER_CTRL_GAS_1, Temp[0]) || this->_gasConversionRunning())
    {
        return this->_mError;
    }

    // Check if the gas measurement is valid
    if(this->_readBytes(BME680_REGISTER_GAS_R_MSB, Temp, 0x02))
    {
        return this->_mError;
    }

	*Valid = true;
	if(!(Temp[1] & (0x01 << BME680_BIT_GAS_VALID)))
    {
	    *Valid = false;
    }

	if(!(Temp[1] & (0x01 << BME680_BIT_HEAT_STAB)))
    {
	    *Valid = false;
    }

    // Read the results
    *GasRange = Temp[1] & 0x0F;
    *GasADC = ((uint16_t)Temp[0] << 0x02) | (((uint16_t)Temp[1] & 0xC0) >> 0x06);

    return NO_ERROR;
}

void BME680::_calcTemperature(uint32_t RawTemperature, float* Temperature)
{
	int64_t var1;
	int64_t var2;
	int64_t var3;

	var1 = ((int32_t)RawTemperature >> 0x03) - ((int32_t)this->_mCalibCoef.T1 << 0x01);
	var2 = (var1 * (int32_t)this->_mCalibCoef.T2) >> 0x0B;
	var3 = ((var1 >> 0x01) * (var1 >> 0x01)) >> 0x0C;
	var3 = (var3 * ((int32_t)this->_mCalibCoef.T3 << 0x04)) >> 0x0E;
	this->_mCalibCoef.t_fine = (int32_t)(var2 + var3);

	*Temperature = (float)(((this->_mCalibCoef.t_fine * 0x05) + 0x80) >> 0x08) / 100.0;
}

void BME680::_calcPressure(uint32_t RawPressure, float* Pressure)
{
    int32_t var1;
	int32_t var2;
	int32_t var3;
	int32_t pressure_comp;

	var1 = (((int32_t)this->_mCalibCoef.t_fine) >> 0x01) - 64000;
	var2 = ((((var1 >> 0x02) * (var1 >> 2)) >> 0x0B) * (int32_t)this->_mCalibCoef.P6) >> 0x02;
	var2 = var2 + ((var1 * (int32_t)this->_mCalibCoef.P5) << 0x01);
	var2 = (var2 >> 0x02) + ((int32_t)this->_mCalibCoef.P4 << 0x10);
	var1 = (((((var1 >> 0x02) * (var1 >> 0x02)) >> 0x0D) * ((int32_t)this->_mCalibCoef.P3 << 0x05)) >> 0x03) + (((int32_t)this->_mCalibCoef.P2 * var1) >> 0x01);
	var1 = var1 >> 0x12;
	var1 = ((0x8000 + var1) * (int32_t)this->_mCalibCoef.P1) >> 0x0F;
	pressure_comp = 1048576 - RawPressure;
	pressure_comp = (int32_t)((pressure_comp - (var2 >> 0x0C)) * ((uint32_t)3125));

	if(pressure_comp >= (0x01 << 0x1E))
    {
		pressure_comp = ((pressure_comp / var1) << 0x01);
    }
	else
    {
		pressure_comp = ((pressure_comp << 0x01) / var1);
    }

	var1 = ((int32_t)this->_mCalibCoef.P9 * (int32_t)(((pressure_comp >> 0x03) * (pressure_comp >> 0x03)) >> 0x0D)) >> 0x0C;
	var2 = ((int32_t)(pressure_comp >> 0x02) * (int32_t)this->_mCalibCoef.P8) >> 0x0D;
	var3 = ((int32_t)(pressure_comp >> 0x08) * (int32_t)(pressure_comp >> 0x08) * (int32_t)(pressure_comp >> 0x08) * (int32_t)this->_mCalibCoef.P10) >> 0x11;

	*Pressure = (float)((pressure_comp) + ((var1 + var2 + var3 + ((int32_t)this->_mCalibCoef.P7 << 0x07)) >> 0x04)) / 100.0;
}

void BME680::_calcHumidity(uint16_t RawHumidity, float* Humidity)
{
	int32_t var1;
	int32_t var2;
	int32_t var3;
	int32_t var4;
	int32_t var5;
	int32_t var6;
	int32_t temp_scaled;

	temp_scaled = ((this->_mCalibCoef.t_fine * 0x05) + 0x80) >> 0x08;
    var1 = ((int32_t)RawHumidity - ((int32_t)((int32_t)(this->_mCalibCoef.H1 << 0x04)))) - (((temp_scaled * (int32_t)this->_mCalibCoef.H3) / ((int32_t) 100)) >> 0x01);
	var2 = ((int32_t)this->_mCalibCoef.H2 * (((temp_scaled * (int32_t)this->_mCalibCoef.H4) / ((int32_t)100)) + (((temp_scaled * ((temp_scaled * (int32_t)this->_mCalibCoef.H5) / ((int32_t)100))) >> 0x06) / ((int32_t)100)) + (int32_t)(0x01 << 0x0E))) >> 0x0A;
	var3 = var1 * var2;
	var4 = (((int32_t)this->_mCalibCoef.H6 << 0x07) + ((temp_scaled * (int32_t)this->_mCalibCoef.H7) / ((int32_t)100))) >> 0x04;
	var5 = ((var3 >> 0x0E) * (var3 >> 0x0E)) >> 0x0A;
	var6 = (var4 * var5) >> 0x01;

	*Humidity = (float)(((var3 + var6) >> 0x0A) / 4096.0);

    if(*Humidity > 100.0)
    {
        *Humidity = 100.0;
    }
    else if(*Humidity < 0.0)
    {
        *Humidity = 0.0;
    }
}

void BME680::_calcGasResistance(uint16_t GasADC, uint8_t GasRange, float* GasResistance)
{
	int64_t var1;
	uint64_t var2;
	int64_t var3;

	var1 = (int64_t)((1340 + (0x05 * (int64_t)this->_mCalibCoef.RangeSWErr)) * ((int64_t)const_array_1[GasRange])) >> 0x10;
	var2 = (((int64_t) ((int64_t)GasADC << 0x0F) - (int64_t)(16777216)) + var1);
	var3 = (((int64_t) const_array_2[GasRange] * (int64_t)var1) >> 0x09);
	*GasResistance = ((float)(var3 + ((int64_t)var2 >> 0x01))) / ((float)var2);
}
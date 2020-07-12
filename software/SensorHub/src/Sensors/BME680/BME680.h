/*
 * BME680.h
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

/** @file BME680/BME680.h
 *  @brief Driver for the BME680 I2C environment sensor.
 *
 *  @author Daniel Kampert
 *  @bug No known bugs
 */

#pragma once

#include <Wire.h>
#include <math.h>
#include <stdio.h>
#include <application.h>

#include "BME680_Heater/BME680_Heater.h"

class BME680
{
	public:
        /** @brief BME680 error codes.
         */
        typedef enum
        {
            NO_ERROR = 0x00,							        /**< No error. */
            NO_DEVICE = 0x01,                                   /**< No device found. */
            NOT_CONNECTED = 0x02,                               /**< Device not connected (i. e. Wire not enabled). */
            NOT_INITIALIZED = 0x03,							    /**< Device not initialized. Use #Initialize first. */
            INVALID_PARAMETER = 0x04,							/**< Invalid function parameter. */
            TRANSMISSION_ERROR = 0x05,							/**< General transmission error. */
        } Error;

        /** @brief BME680 oversampling modes.
         */
        typedef enum
        {
            SKIP = 0x00,							            /**< Skip this sensor. */
            X1 = 0x01,							                /**< Oversampling x1. */
            X2 = 0x02,							                /**< Oversampling x2. */
            X4 = 0x03,							                /**< Oversampling x4. */
            X8 = 0x04,							                /**< Oversampling x8. */
            X16 = 0x05,							                /**< Oversampling x16. */
        } OSS;

        /** @brief BME680 operation modes.
         */
        typedef enum
        {
            SLEEP = 0x00,							            /**< Sleep mode. */
            FORCED = 0x01,							            /**< Measurement mode. */
        } Mode;

        /** @brief BME680 filter coefficient length.
         */
        typedef enum
        {
            COEF_0 = 0x00,							            /**< Use no filter. */
            COEF_1 = 0x01,							            /**< Use 1 filter coefficient. */
            COEF_3 = 0x02,							            /**< Use 3 filter coefficients. */
            COEF_7 = 0x03,							            /**< Use 7 filter coefficients. */
            COEF_15 = 0x04,							            /**< Use 15 filter coefficients. */
            COEF_31 = 0x05,							            /**< Use 31 filter coefficients. */
            COEF_63 = 0x06,							            /**< Use 63 filter coefficients. */
            COEF_127 = 0x07,							        /**< Use 127 filter coefficients. */
        } Filter;

        /** @brief BME680 calibration parameters.
         */
        typedef struct
        {
            int16_t T2;							                /**< Calibration parameter T2. */
            int8_t T3;							                /**< Calibration parameter T3. */
            uint8_t Reserved;                                   /**< Reserved. */
            uint16_t P1;							            /**< Calibration parameter P1. */
            int16_t P2;							                /**< Calibration parameter P2. */
            int8_t P3;							                /**< Calibration parameter P3. */
            uint8_t Reserved1;                                  /**< Reserved. */
            int16_t P4;							                /**< Calibration parameter P4. */
            int16_t P5;							                /**< Calibration parameter P5. */
            int8_t P7;							                /**< Calibration parameter P6. */
            int8_t P6;							                /**< Calibration parameter P7. */
            uint16_t Reserved2;                                 /**< Reserved. */
            int16_t P8;							                /**< Calibration parameter P8. */
            int16_t P9;							                /**< Calibration parameter P9. */
            uint8_t P10;							            /**< Calibration parameter P10. */
            int8_t H3;							                /**< Calibration parameter H3. */
            int8_t H4;							                /**< Calibration parameter H4. */
            int8_t H5;							                /**< Calibration parameter H5. */
            uint8_t H6;							                /**< Calibration parameter H6. */
            int8_t H7;							                /**< Calibration parameter H7. */
            uint16_t T1;							            /**< Calibration parameter T1. */
            int16_t G2;							                /**< Calibration parameter G2. */
            int8_t G1;							                /**< Calibration parameter G1. */
            int8_t G3;							                /**< Calibration parameter G3. */
            uint16_t H1;							            /**< Calibration parameter H1. */
            uint16_t H2;							            /**< Calibration parameter H2. */
            uint8_t HeatRange;							        /**< Calibration parameter Heat range. */
            int8_t HeatVal;							            /**< Calibration parameter Heat balue. */
            int8_t RangeSWErr;							        /**< Calibration parameter Range switching error. */
            int32_t t_fine;							            /**< Calibration parameter t_fine (used by the calculations). */
        } __attribute__((packed)) CalibCoef;

        /** @brief BME680 measurement data.
         */
        typedef struct
        {
            float Temperature;							        /**< Result from temperature measurement. */
            float Pressure;							            /**< Result from pressure measurement. */
            float Humidity;							            /**< Result from humidity measurement. */
            float GasResistance;							    /**< Result from gas resistance measurement. */
            bool GasValid;							            /**< Flag the gas measurement result as valid or invalid. */
        } Data;        

        /** @brief  Get the initialization status of the sensor.
         *  @return Initialization status
         */
        bool initialized(void) const;

        /** @brief  Get the sensor ID.
         *  @return Sensor ID
         */
        uint8_t id(void) const;

        /** @brief  Get the calibration coefficients.
         *  @return Calibration coefficients
         */
        BME680::CalibCoef calibCoef(void) const;

        /** @brief          Get the filter settings.
         *  @param Filer    Pointer to filter settings
         *  @return         Error code
         */
        BME680::Error filter(BME680::Filter* Filter);

        /** @brief      Get the device mode.
         *  @param Mode Pointer to device mode
         *  @return     Error code
         */
        BME680::Error mode(BME680::Mode* Mode);

        /** @brief Constructor.
         */
		BME680(void);

        /** @brief Deconstructor.
         */
        ~BME680(void);

        /** @brief      Initialize the sensor. Must be called first.
         *  @param SDO  Set to use the SDO input of the sensor
         *  @return     Error code
         */
		BME680::Error Initialize(bool SDO);
    
        /** @brief          Enable/Disable the heater of the sensor.
         *  @param Enable   Enable/Disable
         *  @return         Error code
         */
        BME680::Error SwitchHeater(bool Enable);

        /** @brief      Set the device mode.
         *  @param Mode Device mode
         *  @return     Error code
         */
        BME680::Error SetMode(BME680::Mode Mode);

        /** @brief      Set the device filter.
         *  @param Mode Device filter
         *  @return     Error code
         */
        BME680::Error SetFilter(BME680::Filter Filter);

        /** @brief              Start a new temperature measurement.
         *                      NOTE: The device must be set in #FORCED mode first!
         *                      NOTE: This function will use #X1 for temperature measurement!
         *  @param Temperature  Pointer to temperature result
         *  @return             Error code
         */
        BME680::Error MeasureTemperature(float* Temperature);

        /** @brief                  Start a new temperature measurement.
         *                          NOTE: The device must be set in #FORCED mode first!
         *  @param Temperature      Pointer to temperature result
         *  @param OSS_Temperature  Temperature oversampling
         *  @return                 Error code
         */
        BME680::Error MeasureTemperature(float* Temperature, BME680::OSS OSS_Temperature);

        /** @brief                Start a new pressure measurement.
         *                        NOTE: The device must be set in #FORCED mode first!
         *                        NOTE: This function will use #X1 for temperature and pressure measurement!
         *  @param Pressure       Pointer to pressure result
         *  @return               Error code
         */
        BME680::Error MeasurePressure(float* Pressure);

        /** @brief                Start a new pressure measurement.
         *                        NOTE: The device must be set in #FORCED mode first!
         *                        NOTE: This function will use #X1 for temperature measurement!
         *  @param Pressure       Pointer to pressure result
         *  @param OSS_Pressure   Pressure oversampling
         *  @return               Error code
         */
        BME680::Error MeasurePressure(float* Pressure, BME680::OSS OSS_Pressure);

        /** @brief                  Start a new pressure measurement.
         *                          NOTE: The device must be set in #FORCED mode first!
         *  @param Pressure         Pointer to pressure result
         *  @param OSS_Pressure     Pressure oversampling
         *  @param OSS_Temperature  Temperature oversampling
         *  @return                 Error code
         */
        BME680::Error MeasurePressure(float* Pressure, BME680::OSS OSS_Pressure, BME680::OSS OSS_Temperature);

        /** @brief                  Start a new humidity measurement.
         *                          NOTE: The device must be set in #FORCED mode first! 
         *                          NOTE: This function will use #X1 for temperature and humidity measurement!
         *  @param Humidity         Pointer to humidity result
         *  @return                 Error code
         */
        BME680::Error MeasureHumidity(float* Humidity);

        /** @brief                  Start a new humidity measurement.
         *                          NOTE: The device must be set in #FORCED mode first! 
         *                          NOTE: This function will use #X1 for temperature measurement!
         *  @param Humidity         Pointer to humidity result
         *  @param OSS_Humidity     Pressure oversampling
         *  @return                 Error code
         */
        BME680::Error MeasureHumidity(float* Humidity, BME680::OSS OSS_Humidity);

        /** @brief                  Start a new humidity measurement.
         *                          NOTE: The device must be set in #FORCED mode first!
         *  @param Humidity         Pointer to humidity result
         *  @param OSS_Humidity     Humidity oversampling
         *  @param OSS_Temperature  Temperature oversampling
         *  @return                 Error code
         */
        BME680::Error MeasureHumidity(float* Humidity, BME680::OSS OSS_Humidity, BME680::OSS OSS_Temperature);

        /** @brief                  Start a new gas measurement.
         *                          NOTE: The device must be set in #FORCED mode first!
         *  @param Profile          \ref BME680_Heater heater profile for the measurement
         *  @param GasResistance    Pointer to gas resistance result
         *  @param Valid            Pointer to valid flag to indicate a valid measurement
         *  @return                 Error code
         */
        BME680::Error MeasureGas(BME680_Heater Profile, float* GasResistance, bool* Valid);

        /** @brief                  Start a new measurement.
         *                          NOTE: The device must be set in #FORCED mode first!
         *                          NOTE: This function will use #X1 for all measurements!
         *  @param Profile          \ref BME680_Heater heater profile for the measurement
         *  @param Data             Pointer to \ref BME680::Data object
         *  @return                 Error code
         */
        BME680::Error Measure(BME680_Heater Profile, BME680::Data* Data);

        /** @brief                  Start a new measurement.
         *                          NOTE: The device must be set in #FORCED mode first!
         *  @param Profile          \ref BME680_Heater heater profile for the measurement
         *  @param Data             Pointer to \ref BME680::Data object
         *  @param OSS_Humidity     Humidity oversampling
         *  @param OSS_Pressure     Pressure oversampling
         *  @param OSS_Temperature  Temperature oversampling
         *  @return                 Error code
         */
        BME680::Error Measure(BME680_Heater Profile, BME680::Data* Data, BME680::OSS OSS_Humidity, BME680::OSS OSS_Pressure, BME680::OSS OSS_Temperature);

	private:
    	const uint32_t const_array_1[16] = { 
            (uint32_t)(2147483647), (uint32_t)(2147483647), (uint32_t)(2147483647), (uint32_t)(2147483647),
		    (uint32_t)(2147483647), (uint32_t)(2126008810), (uint32_t)(2147483647), (uint32_t)(2130303777),
		    (uint32_t)(2147483647), (uint32_t)(2147483647), (uint32_t)(2143188679), (uint32_t)(2136746228),
		    (uint32_t)(2147483647), (uint32_t)(2126008810), (uint32_t)(2147483647), (uint32_t)(2147483647)
        };

	    const uint32_t const_array_2[16] = { 
            (uint32_t)(4096000000), (uint32_t)(2048000000), (uint32_t)(1024000000), (uint32_t)(512000000),
            (uint32_t)(255744255), (uint32_t)(127110228), (uint32_t)(64000000), (uint32_t)(32258064), (uint32_t)(16016016),
            (uint32_t)(8000000), (uint32_t)(4000000), (uint32_t)(2000000), (uint32_t)(1000000), (uint32_t)(500000),
            (uint32_t)(250000), (uint32_t)(125000) 
        };

        uint8_t _mID;
        uint8_t _mAddress;

        bool _mInitialized;

        BME680::CalibCoef _mCalibCoef;

        BME680:: Error _mError;

        /** @brief          Read a unsigned char (8 bit) from the sensor
         *  @param Address  Register Address
         *  @param Data     Pointer to data
         *  @return         Error code
         */
		BME680::Error _readUChar(uint8_t Address, uint8_t* Data);

        /** @brief          Read a signed char (8 bit) from the sensor
         *  @param Address  Register Address
         *  @param Data     Pointer to data
         *  @return         Error code
         */
		BME680::Error _readSChar(uint8_t Address, int8_t* Data);

        /** @brief          Read a unsigned integer (16 bit) from the sensor
         *  @param Address  Register Address
         *  @param Data     Pointer to data
         *  @return         Error code
         */
		BME680::Error _readUInt(uint8_t Address, uint16_t* Data);

        /** @brief          Read a signed integer (16 bit) from the sensor
         *  @param Address  Register Address
         *  @param Data     Pointer to data
         *  @return         Error code
         */
		BME680::Error _readSInt(uint8_t Address, int16_t* Data);

        /** @brief          Read n bytes from the sensor.
         *  @param Address  Register Address
         *  @param Data     Pointer to data
         *  @param Length   Byte count
         *  @return         Error code
         */
		BME680::Error _readBytes(uint8_t Address, uint8_t* Data, uint8_t Length);

        /** @brief          Write a byte to the sensor.
         *  @param Address  Register Address
         *  @param Data     Data byte
         *  @return         Error code
         */
		BME680::Error _writeByte(uint8_t Address, uint8_t Data);

        /** @brief  Wait until a conversion has finished.
         *  @return Error code
         */
        BME680::Error _conversionRunning(void);

        /** @brief  Wait until a gas conversion has finished.
         *  @return Error code
         */
        BME680::Error _gasConversionRunning(void);

        /** @brief          Set the heater profile for a gas measurement.
         *  @param Index    Heater profile index. Must be between 0 and 9.
         *  @return         Error code
         */
        BME680::Error _setHeaterProfile(uint8_t Index);

        /** @brief              Set the heater temperature.
         *  @param Index        Heater profile index. Must be between 0 and 9.
         *  @param Temperature  Heater temperature. Must be between 200 and 400.
         *  @return             Error code
         */
        BME680::Error _setHeaterTemperature(uint8_t Index, uint16_t Temperature);

        /** @brief          Set the heating duration for a heater.
         *  @param Index    Heater profile index. Must be between 0 and 9.
         *  @param Duration Heater Duration. Must be between 1 and 4032.
         *  @return         Error code
         */
        BME680::Error _setHeaterDuration(uint8_t Index, uint16_t Duration);

        /** @brief          Set an additional heater current.
         *  @param Index    Heater profile index. Must be between 0 and 9.
         *  @param Current  Heater Current. Must be between 1 and 16.
         *  @return         Error code
         */
        BME680::Error _setHeaterCurrent(uint8_t Index, uint8_t Current);

        /** @brief  Read the calibration values from the sensor.
         *  @return Error code
         */
        BME680::Error _readCalibCoef(void);

        /** @brief                  Start a new temperature measurement and read the result.
         *  @param OSS_Temperature  Temperature oversampling
         *  @param RawTemp          Pointer to temperature result
         *  @return                 Error code
         */
        BME680::Error _readTemperature(BME680::OSS OSS_Temperature, uint32_t* RawTemp);

        /** @brief                  Start a new pressure measurement and read the result.
         *  @param OSS_Pressure     Pressure oversampling
         *  @param RawPressure      Pointer to pressure result
         *  @return                 Error code
         */
        BME680::Error _readPressure(BME680::OSS OSS_Pressure, uint32_t* RawPressure);

        /** @brief                  Start a new humidity measurement and read the result.
         *  @param OSS_Humidity     Humidity oversampling
         *  @param RawHumidity      Pointer to humidity result
         *  @return                 Error code
         */
        BME680::Error _readHumidity(BME680::OSS OSS_Humidity, uint16_t* RawHumidity);

        /** @brief          Start a new gas measurement and read the result.
         *  @param Profile  Heater profile
         *  @param GasADC   Pointer to gas ADC value from sensor
         *  @param GasRange Pointer to gas range value from sensor
         *  @param valid    Pointer to valid flag
         *  @return         Error code
         */
        BME680::Error _readGas(BME680_Heater Profile, uint16_t* GasADC, uint8_t* GasRange, bool* Valid);

        /** @brief                  Calculate the temperature from raw temperature values.
         *  @param RawTemperature   Temperature value from sensor
         *  @param Temperature      Pointer to emperature result
         */
        void _calcTemperature(uint32_t RawTemperature, float* Temperature);

        /** @brief              Calculate the pressure from raw pressure values.
         *  @param RawPressure  Pressure value from sensor
         *  @param Pressure     Pointer to Pressure result
         */
        void _calcPressure(uint32_t RawPressure, float* Pressure);

        /** @brief              Calculate the humidity from raw humidity values.
         *  @param RawHumidity  Jumidity value from sensor
         *  @param Humidity     Pointer to humidity result
         */
        void _calcHumidity(uint16_t RawHumidity, float* Humidity);

        /** @brief                  Calculate the gas resistance from raw gas values.
         *  @param GasADC           Gas ADC value from sensor
         *  @param GasRange         Gas range value from sensor
         *  @param GasResistance    Pointer to gas resistance result
         */
        void _calcGasResistance(uint16_t GasADC, uint8_t GasRange, float* GasResistance);
};

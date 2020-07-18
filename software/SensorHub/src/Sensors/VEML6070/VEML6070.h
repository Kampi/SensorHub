/*
 * VEML6070.h
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

/** @file VEML6070.h
 *  @brief Driver for the VEML6070 I2C UVA light sensor.
 *
 *  @author Daniel Kampert
 *  @bug No known bugs
 */

#pragma once

#include <Wire.h>
#include <math.h>
#include <stdio.h>
#include <application.h>

class VEML6070
{
	public:
        /** @brief VEML6070 error codes.
         */
        typedef enum
        {
            NO_ERROR = 0x00,							        /**< No error */
            NOT_CONNECTED = 0x01,                               /**< Device not connected (i. e. Wire not enabled) */
            NOT_INITIALIZED = 0x02,							    /**< Device not initialized. Use #Initialize first */
            INVALID_PARAMETER = 0x03,							/**< Invalid function parameter */
            TRANSMISSION_ERROR = 0x04,							/**< General transmission error */
        } Error;

        /** @brief VEML6070 integration times.
         */
        typedef enum 
        {
            TIME_HALF = 0x00,                                   /**< Integration time 0.5T */
            TIME_X1 = 0x01,                                     /**< Integration time 1T */
            TIME_X2 = 0x02,                                     /**< Integration time 2T */
            TIME_X4 = 0x03,                                     /**< Integration time 4T */
        } IntegrationTime;

        /** @brief  Get the initialization status of the sensor.
         *  @return Initialization status
         */
        bool initialized(void) const;

        /** @brief Constructor.
         */
		VEML6070(void);

        /** @brief Deconstructor.
         */
        ~VEML6070(void);

        /** @brief      Initialize the sensor. Must be called first.
         *  @return     Error code
         */
		VEML6070::Error Initialize(void);

        /** @brief      Set the integration time of the sensor.
         *  @param Time Integration time settings
         *  @return     Error code
         */
        VEML6070::Error SetIntegrationTime(VEML6070::IntegrationTime Time);

        /** @brief      Start a new UV measurement.
         *  @param UV   Pointer to measurement result
         *  @return     Error code
         */
        VEML6070::Error Measure(uint16_t* UV);

        /** @brief          Enable/Disable the sleep mode of the sensor.
         *  @param Enable   Sleep enable/Disable
         *  @return         Error code
         */
        VEML6070::Error Sleep(bool Enable);

	private:
        typedef union
        {
            struct 
            {
                uint8_t SD:1;
                uint8_t Reserved:1;
                uint8_t IT:2;
                uint8_t ACK_THD:1;
                uint8_t ACK:1;
            } __attribute__((packed)) Bitfield;

            uint8_t Register;
        } CommandRegister;

        bool _mInitialized;

        CommandRegister _mCommand;

        VEML6070::Error _mError;

        /** @brief  Clear the sensor ACK.
         *  @return Error code
         */
        VEML6070::Error _clearAck(void);

        /** @brief  Transmit the current command register to the sensor.
         *  @return Error code
         */
        VEML6070::Error _writeCommand(void);
};

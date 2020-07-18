/*
 * BH1726.h
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

/** @file BH1726.h
 *  @brief Driver for the Rohm I2C ambient light sensor.
 *
 *  @author Daniel Kampert
 *  @bug No known bugs
 */

#pragma once

#include <Wire.h>
#include <math.h>
#include <stdio.h>
#include <application.h>

class BH1726
{
	public:
        /** @brief Default integration time of the sensor.
         */
        #define BH1726_DEFAULT_TIME                             0xDA

        /** @brief BH1726 error codes.
         */
        typedef enum
        {
            NO_ERROR = 0x00,							        /**< No error */
            NO_DEVICE = 0x01,                                   /**< No device found */
            NOT_CONNECTED = 0x02,                               /**< Device not connected (i. e. Wire not enabled) */
            NOT_INITIALIZED = 0x03,							    /**< Device not initialized. Use #Initialize first */
            INVALID_PARAMETER = 0x04,							/**< Invalid function parameter */
            TRANSMISSION_ERROR = 0x05,							/**< General transmission error */
        } Error;

        /** @brief BH1726 register sets.
         */
        typedef enum
        {
            REG_0 = 0x01,							            /**< Register set 0 */
            REG_1 = 0x02,							            /**< Register set 1 */
        } Register;

        /** @brief BH1726 gain settings.
         */
        typedef enum
        {
            GAIN_X1 = 0x00,							            /**< Gain x1 */
            GAIN_X2 = 0x01,							            /**< Gain x2 */
            GAIN_X64 = 0x02,							        /**< Gain x64 */
            GAIN_X128 = 0x03,							        /**< Gain x128 */
        } Gain;

        /** @brief BH1726 persistence settings.
         */
        typedef enum
        {
            PERSIST_0 = 0x00,							        /**< Interrupt becomes active at each measurement end */
            PERSIST_1 = 0x01,							        /**< Interrupt status is updated at each measurement end */
            PERSIST_2 = 0x02,							        /**< Interrupt status is updated if 2 consecutive threshold judgments are the same */
            PERSIST_3 = 0x02,							        /**< Interrupt status is updated if 3 consecutive threshold judgments are the same */
            PERSIST_4 = 0x02,							        /**< Interrupt status is updated if 4 consecutive threshold judgments are the same */
            PERSIST_5 = 0x02,							        /**< Interrupt status is updated if 5 consecutive threshold judgments are the same */
            PERSIST_6 = 0x02,							        /**< Interrupt status is updated if 6 consecutive threshold judgments are the same */
            PERSIST_7 = 0x02,							        /**< Interrupt status is updated if 7 consecutive threshold judgments are the same */
            PERSIST_8 = 0x02,							        /**< Interrupt status is updated if 8 consecutive threshold judgments are the same */
            PERSIST_9 = 0x02,							        /**< Interrupt status is updated if 9 consecutive threshold judgments are the same */
            PERSIST_10 = 0x02,							        /**< Interrupt status is updated if 10 consecutive threshold judgments are the same */
            PERSIST_11 = 0x02,							        /**< Interrupt status is updated if 11 consecutive threshold judgments are the same */
            PERSIST_12 = 0x02,							        /**< Interrupt status is updated if 12 consecutive threshold judgments are the same */
            PERSIST_13 = 0x02,							        /**< Interrupt status is updated if 13 consecutive threshold judgments are the same */
            PERSIST_14 = 0x02,						            /**< Interrupt status is updated if 14 consecutive threshold judgments are the same */
            PERSIST_15 = 0x02,							        /**< Interrupt status is updated if 15 consecutive threshold judgments are the same */
        } Persistence;

        /** @brief  Threshold callback.
         */
        typedef void(*ThresholdCallback)(void);

        /** @brief  Get the initialization status of the sensor.
         *  @return Initialization status
         */
        bool initialized(void) const;

        /** @brief  Get the power status of the sensor.
         *  @return #true when power on
         */
        bool power(void) const;

        /** @brief Constructor.
         */
		BH1726(void);

        /** @brief Deconstructor.
         */
        ~BH1726(void);

        /** @brief          Initialize the sensor. Must be called first.
         *  @param UseAddr  Use the ADDR pin of the sensor to activate the second I2C address
         *  @return         Error code
         */
		BH1726::Error Initialize(bool UseAddr);

        /** @brief  Enable the sensor.
         *  @return Error code
         */
		BH1726::Error Enable(void);

        /** @brief  Disable the sensor.
         *  @return Error code
         */
		BH1726::Error Disable(void);

        /** @brief                  Set the threshold for the sensor.
         *  @param Upper            Upper threshold
         *  @param Lower            Lower threshold
         *  @param Pin              Interrupt pin
         *  @param Persistance      Interrupt persistance settings
         *  @param Callback         Measurement complete callback
         *  @return                 Error code
         */
		BH1726::Error SetThreshold(uint16_t Upper, uint16_t Lower, uint8_t Pin, BH1726::Persistence Persistance, BH1726::ThresholdCallback Callback);

        /** @brief                  Set the threshold for the sensor.
         *  @param Upper            Upper threshold
         *  @param Lower            Lower threshold
         *  @param Pin              Interrupt pin
         *  @param UseIntPullUp     Enable internal pull up resistor
         *  @param UseLatch         Enable the latching function
         *  @param Persistance      Interrupt persistance settings
         *  @param Callback         Measurement complete callback
         *  @return                 Error code
         */
		BH1726::Error SetThreshold(uint16_t Upper, uint16_t Lower, uint8_t Pin, bool UseIntPullUp, bool UseLatch, BH1726::Persistence Persistance, BH1726::ThresholdCallback Callback);

        /** @brief  Clear the interrupt state of the sensor.
         *  @return Error code
         */
		BH1726::Error ClearInterrupt(void);

        /** @brief          Set the gain of the sensor.
         *  @param Gain     Sensor gain
         *  @return         Error code
         */
        BH1726::Error SetGain(BH1726::Gain Gain);

        /** @brief          Set the gain of the sensor.
         *  @param Gain     Sensor gain
         *  @param Register Target register set
         *  @return         Error code
         */
        BH1726::Error SetGain(BH1726::Gain Gain, BH1726::Register Register);

        /** @brief      Set the integration time of the sensor.
         *  @param Time Integration time
         *  @return     Error code
         */
        BH1726::Error SetTime(uint8_t Time);

        /** @brief  Perform a soft reset of the sensor.
         *  @return Error code
         */
        BH1726::Error Reset(void);

        /** @brief          Start a new ambient light measurement.
         *  @param Light    Pointer to measurement result
         *  @return         Error code
         */
        BH1726::Error Measure(uint16_t* Light);

	private:
        bool _mInitialized;
        bool _mActive;

        uint8_t _mAddress;

        BH1726::Error _mError;

        BH1726::ThresholdCallback _mCallback;

        /** @brief  Wait for a new measurement result.
         *  @return Error code
         */
        BH1726::Error _wait(void);

        /** @brief          Read n bytes from the sensor.
         *  @param Address  Register Address
         *  @param Data     Pointer to data
         *  @param Length   Byte count
         *  @return         Error code
         */
		BH1726::Error _readBytes(uint8_t Address, uint8_t* Data, uint8_t Length);

        /** @brief          Write a byte to the sensor.
         *  @param Address  Register Address
         *  @param Data     Pointer to data
         *  @param Length   Byte count
         *  @return         Error code
         */
		BH1726::Error _writeBytes(uint8_t Address, uint8_t* Data, uint8_t Length);

        /** @brief  Threshold callback function.
         */
        void _callback(void);
};

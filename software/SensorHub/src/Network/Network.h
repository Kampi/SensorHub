/*
 * Network.h
 *
 *  Copyright (C) Daniel Kampert, 2020
 *	Website: www.kampis-elektroecke.de
 *  File info: Network and bluetooth driver for the SensorHub.

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

#include <application.h>

#include "MQTT/mqtt.h"

class Network
{
	public:
        typedef enum
        {
            NO_ERROR = 0x00,
            NO_WIFI_CREDENTIALS = 0x01,
            NO_VALID_IP = 0x02,
            CONNECTION_ERROR = 0x03,
            TIMEOUT = 0x04,
        } Error;

        static Network::Error lastError(void);

		static Network::Error Initialize(void);
        static Network::Error Connect(uint32_t Timeout);
        static Network::Error Publish(const char* Topic, String Message);
        static Network::Error Publish(const char* Topic, char* Buffer, uint8_t Length);
        static Network::Error Setup(uint32_t Timeout);
        static void Disconnect(void);

	private:
        static MQTT _mClient;
        static IPAddress _mServerAddress;

        static Network::Error _mLastError;

        static BleCharacteristic _mServerIPCharacteristic;
        static BleAdvertisingData _mBluetoothAdvertise;

        static void _callback(uint16_t TopicLength, char* Topic, uint16_t PayloadLength, char* Payload, uint16_t ID, MQTT::QoS QoS, bool DUP);
        static void _bluetoothDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context);
};

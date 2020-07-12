/*
 * Network.cpp
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

#include "Network.h"

#define NETWORK_IP_LOC                  0x00

// Bluetooth service UUID
static const char* ServiceUUID = "b4250401-fb4b-4746-b2b0-93f0e61122c6";
static const char* ServerUUID = "b4250402-fb4b-4746-b2b0-93f0e61122c6";

BleCharacteristic Network::_mServerIPCharacteristic("Server address", BleCharacteristicProperty::WRITE_WO_RSP, ServerUUID, BleUuid(ServiceUUID), &Network::_bluetoothDataReceived, (void*)ServerUUID);
BleAdvertisingData Network::_mBluetoothAdvertise;

MQTT Network::_mClient;
IPAddress Network::_mServerAddress;

Network::Error Network::_mLastError;

Network::Error Network::lastError(void)
{
    return Network::_mLastError;
}

void Network::_callback(uint16_t TopicLength, char* Topic, uint16_t PayloadLength, char* Payload, uint16_t ID, MQTT::QoS QoS, bool DUP)
{
    Serial.printlnf("Topic lenght: %i", TopicLength);
    for(uint8_t i = 0x00; i < TopicLength; i++)
    {
        Serial.print(Topic[i]);
    }
    Serial.println();

    Serial.printlnf("Payload lenght: %i", PayloadLength);
    for(uint8_t i = 0x00; i < PayloadLength; i++)
    {
        Serial.print(Payload[i]);
    }

    Serial.println();
    Serial.printlnf("QoS: %i", QoS);
    Serial.printlnf("ID: %i", ID);
    Serial.printlnf("DUP: %i", DUP);
}

void Network::_bluetoothDataReceived(const uint8_t* Data, size_t Length, const BlePeerDevice& Peer, void* Context)
{
    if(Context == ServerUUID)
    {
        if(Length == 0x04)
        {
            Network::_mServerAddress = IPAddress(Data[0], Data[1], Data[2], Data[3]);

            Serial.printlnf("[INFO] Set IP address to: %i.%i.%i.%i", Data[0], Data[1], Data[2], Data[3]);

            // Save the IP address in the EEPROM
            EEPROM.put(NETWORK_IP_LOC, Network::_mServerAddress);
        }
    }
}

Network::Error Network::Initialize(void)
{
    uint8_t Found;
    WiFiAccessPoint AP[5];

    // Test if WiFi credentials are available
    if(!WiFi.hasCredentials())
    {
        Network::_mLastError = NO_WIFI_CREDENTIALS;
        return NO_WIFI_CREDENTIALS;
    }

    Found = WiFi.getCredentials(AP, 5);
    Serial.printlnf("[INFO] Found %i WiFi credentials", Found);
    for(int i = 0; i < Found; i++)
    {
        Serial.printlnf("   SSID: %s", AP[i].ssid);
        Serial.printlnf("   Security: %i", AP[i].security);
        Serial.printlnf("   Cipher: %i", AP[i].cipher);
    }

    // Load the settings from the EEPROM
    EEPROM.get(NETWORK_IP_LOC, Network::_mServerAddress);

    // Configure the MQTT client
    Network::_mClient.SetBroker(Network::_mServerAddress);
    Network::_mClient.SetCallback(Network::_callback);

    Network::_mLastError = NO_ERROR;
    return NO_ERROR;
}

Network::Error Network::Connect(uint32_t Timeout)
{
    if(!Network::_mClient.isConnected())
    {
        WiFi.on();
        WiFi.connect();
        if(!waitFor(WiFi.ready, Timeout))
        {
            Network::_mLastError = TIMEOUT;
            return TIMEOUT;
        }

        if(Network::_mClient.Connect("SensorHub", false))
        {
            Network::_mLastError = CONNECTION_ERROR;
            return CONNECTION_ERROR;
        }
    }

    Network::_mLastError = NO_ERROR;
    return NO_ERROR;
}

Network::Error Network::Publish(const char* Topic, String Message)
{
    if(Network::_mClient.Publish(Topic, Message.c_str(), Message.length()))
    {
        Network::_mLastError = CONNECTION_ERROR;
        return CONNECTION_ERROR;
    }

    Network::_mLastError = NO_ERROR;
    return NO_ERROR;
}

Network::Error Network::Publish(const char* Topic, char* Buffer, uint8_t Length)
{
    if(Network::_mClient.Publish(Topic, Buffer, Length))
    {
        Network::_mLastError = CONNECTION_ERROR;
        return CONNECTION_ERROR;
    }

    Network::_mLastError = NO_ERROR;
    return NO_ERROR;
}

Network::Error Network::Setup(uint32_t Timeout)
{
    RGB.control(true);

    RGB.color(0xFF, 0xFF, 0xFF);
    Serial.println("[INFO] Enter setup mode...");

    Network::_mBluetoothAdvertise.appendServiceUUID(BleUuid(ServiceUUID));
    BLE.addCharacteristic(_mServerIPCharacteristic);
    BLE.advertise(&Network::_mBluetoothAdvertise);

    // Wait for a bluetooth connection
    RGB.color(0xFF, 0x00, 0x00);
    while(!BLE.connected());

    // Wait for a valid configuration
    while(BLE.connected())
    { 
        RGB.color(0x00, 0xFF, 0x00);
        delay(500);
        RGB.color(0x00, 0x00, 0x00);
        delay(500);
    }

    // Test the WiFi connection
    WiFi.on();
    WiFi.connect();
    if(!waitFor(WiFi.ready, Timeout))
    {
        Network::_mLastError = TIMEOUT;
        return TIMEOUT;
    }

    // Ping the address to test if the IP adress is valid
    if(!WiFi.ping(Network::_mServerAddress, 0x04))
    {
        Serial.println("[ERROR] Can not ping MQTT broker!");
        Network::_mLastError = NO_VALID_IP;
        return NO_VALID_IP;
    }

    WiFi.disconnect();
    WiFi.off();

    Serial.println("[INFO] Leave setup mode...");

    RGB.control(false);

    Network::_mLastError = NO_ERROR;
    return NO_ERROR;
}

void Network::Disconnect(void)
{
    if(Network::_mClient.isConnected())
    {
        Network::_mClient.Disonnect();
    }

    WiFi.disconnect();
    WiFi.off();
}
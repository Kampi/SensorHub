/*
 * SensorHub.ino
 *
 *  Copyright (C) Daniel Kampert, 2020
 *	Website: www.kampis-elektroecke.de
 *  File info: Application software for the SensorHub.

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

#include "Network/Network.h"
#include "Sensors/Sensors.h"
#include "ErrorClass/ErrorClass.h"

#define FIRMWARE_MAJOR                  1
#define FIRMWARE_MINOR                  0
#define FIRMWARE_REVISION               0

#define TIMEOUT                         60000

char Buffer[200];

SystemSleepConfiguration SleepConfig;

SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

void setup()
{
    delay(3000);

    Serial.begin(9600);

    Serial.printlnf("--- SensorHub %i.%i.%i ---", FIRMWARE_MAJOR, FIRMWARE_MINOR, FIRMWARE_REVISION);
    Serial.printlnf("[INFO] Device OS version: %s", System.version().c_str());

    if(waitFor(System.buttonPushed, 10000))
    {
        if(Network::Setup(TIMEOUT) != Network::NO_ERROR)
        {
            ErrorClass::DisplayError(ErrorClass::ERROR_NETWORK, Network::lastError());
        }
    }

    Serial.println("[INFO] Initialize system...");
    if((Network::Initialize() != Network::NO_ERROR) || (Sensors::Initialize() != Sensors::NO_ERROR))
    {
        Serial.println("[ERROR] Unable to initialize system!");

        while(1)
        {
            if(Network::lastError() != Network::NO_ERROR)
            {
                ErrorClass::DisplayError(ErrorClass::ERROR_NETWORK, Network::lastError());
            }

            if(Sensors::lastError() != Sensors::NO_ERROR)
            {
                ErrorClass::DisplayError(ErrorClass::ERROR_SENSORS, Sensors::lastError());
            }

            delay(10000);
        }
    }

    SleepConfig.mode(SystemSleepMode::STOP).duration(180s);

    Serial.println("[INFO] Initialization successful! Starting...");
    delay(3000);
}

void loop()
{
    //System.sleep(SleepConfig);

    if(Network::Connect(TIMEOUT) == Network::NO_ERROR)
    {
        Sensors::SensorData Data;
        JSONBufferWriter Writer(Buffer, sizeof(Buffer));

        if(Sensors::UpdateData(&Data) == Sensors::NO_ERROR)
        {
            memset(Buffer, 0x00, sizeof(Buffer));
            Writer.beginObject();
                Writer.name("Temperature").value(Data.Temperature);
                Writer.name("Ambient light").value(Data.AmbientLight);
                Writer.name("UV").value(Data.UV);
                Writer.name("Pressure").value(Data.Environment.Pressure);
                Writer.name("Humidity").value(Data.Environment.Humidity);
                Writer.name("Gas resistance").value(Data.Environment.GasResistance);
                Writer.name("Gas valid").value(Data.Environment.GasValid);
                Writer.name("IAQ").value(Data.IAQ.Value);
                Writer.name("IAQ valid").value(Data.IAQ.Valid);
            Writer.endObject();

            Network::Publish("sensorhub/weather", Buffer, sizeof(Buffer));
        }
        else
        {
            ErrorClass::DisplayError(ErrorClass::ERROR_SENSORS, Sensors::lastError());
            String Message(Sensors::lastError());
            Network::Publish("sensorhub/errors", Message);
        }

        Network::Disconnect();
    }
    else
    {
        ErrorClass::DisplayError(ErrorClass::ERROR_NETWORK, Network::lastError());
    }

    delay(1000);
}
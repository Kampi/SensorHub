/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "g:/Dropbox/Git/SensorHub/software/SensorHub/src/Sensorhub.ino"
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

void setup();
void loop();
#line 29 "g:/Dropbox/Git/SensorHub/software/SensorHub/src/Sensorhub.ino"
#define FIRMWARE_MAJOR                  1
#define FIRMWARE_MINOR                  2
#define FIRMWARE_REVISION               0

#define TIMEOUT                         60000

char SensorBuffer[200];
char SystemBuffer[200];

SystemSleepConfiguration SleepConfig;

SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

void setup()
{
    delay(3000);

    // Disable LED D7
    pinMode(D7, OUTPUT);
    digitalWrite(D7, LOW);

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

    Serial.printlnf("[INFO] Sleeping for %i seconds", Network::sleepTime() / 1000);
    Serial.println("[INFO] Initialization successful! Starting...");
    SleepConfig.mode(SystemSleepMode::STOP).duration(Network::sleepTime());
    delay(3000);
}

void loop()
{
    System.sleep(SleepConfig);

    if(Network::Connect(TIMEOUT) == Network::NO_ERROR)
    {
        Sensors::SensorData SensorData;
        Sensors::SystemData SystemData;

        if(Sensors::UpdateData(&SensorData, &SystemData) == Sensors::NO_ERROR)
        {
            JSONBufferWriter SensorWriter(SensorBuffer, sizeof(SensorBuffer));
            JSONBufferWriter SystemWriter(SystemBuffer, sizeof(SystemBuffer));

            memset(SensorBuffer, 0x00, sizeof(SensorBuffer));
            memset(SystemBuffer, 0x00, sizeof(SystemBuffer));
            SensorWriter.beginObject();
                SensorWriter.name("Temperature").value(SensorData.Temperature);
                SensorWriter.name("Ambient light").value(SensorData.AmbientLight);
                SensorWriter.name("UV").value(SensorData.UV);
                SensorWriter.name("Pressure").value(SensorData.Environment.Pressure);
                SensorWriter.name("Humidity").value(SensorData.Environment.Humidity);
                SensorWriter.name("Gas resistance").value(SensorData.Environment.GasResistance);
                SensorWriter.name("Gas valid").value(SensorData.Environment.GasValid);
                SensorWriter.name("IAQ").value(SensorData.IAQ.Value);
                SensorWriter.name("IAQ valid").value(SensorData.IAQ.Valid);
            SensorWriter.endObject();

            SystemWriter.beginObject();
                SystemWriter.name("Solar").value(SystemData.SolarVoltage);
                SystemWriter.name("Battery").value(SystemData.BatteryVoltage);
            SystemWriter.endObject();

            Network::Publish("sensorhub/weather", SensorBuffer, sizeof(SensorBuffer));
            Network::Publish("sensorhub/system", SystemBuffer, sizeof(SystemBuffer));
        }
        else
        {
            ErrorClass::DisplayError(ErrorClass::ERROR_SENSORS, Sensors::lastError());
            String Message(Sensors::lastError());
            Network::Publish("sensorhub/errors", Message);
        }
    }
    else
    {
        ErrorClass::DisplayError(ErrorClass::ERROR_NETWORK, Network::lastError());
    }

    Network::Disconnect();
}
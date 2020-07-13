/*
 * ErrorClass.cpp
 *
 *  Copyright (C) Daniel Kampert, 2020
 *	Website: www.kampis-elektroecke.de
 *  File info: Error handling for the SensorHub.

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

#include "ErrorClass.h"

void ErrorClass::DisplayError(ErrorClass::Group Group, uint8_t Error)
{
    delay(1000);

    RGB.control(true);

    switch(Group)
    {
        case ERROR_DEVICE:
        {
            RGB.color(0xFF, 0x00, 0x00);
            Serial.printlnf("[ERROR] Device error: %i", Error);
            break;
        }
        case ERROR_NETWORK:
        {
            RGB.color(0x00, 0xFF, 0x00);
            Serial.printlnf("[ERROR] Network error: %i", Error);
            break;
        }
        case ERROR_SENSORS:
        {
            RGB.color(0x00, 0x00, 0xFF);
            Serial.printlnf("[ERROR] Sensor error: %i", Error);
            break;
        }
    }

    delay(3000);
    for(uint8_t i = 0x00; i < Error; i++)
    {
        RGB.color(0xFF, 0xFF, 0xFF);
        delay(500);
        RGB.color(0x00, 0x00, 0x00);
        delay(500);
    }

    RGB.color(0x00, 0x00, 0x00);
    delay(3000);

    RGB.control(false);
}
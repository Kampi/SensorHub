#include "weatherdata.h"

WeatherData::WeatherData() : _mTemperature(0.0),
                             _mPressure(0.0),
                             _mHumidity(0.0),
                             _mAmbientLight(0.0),
                             _mIAQ(0.0),
                             _mGas(0.0),
                             _mSolar(0.0),
                             _mBattery(0.0),
                             _mUV(0.0),
                             _mIAQValid(false),
                             _mGasValid(false)
{
}

WeatherData::WeatherData(double Temperature,
                         double Pressure,
                         double Humidity,
                         double AmbientLight,
                         int UV,
                         double IAQ,
                         bool IAQValid,
                         double Gas,
                         bool GasValid,
                         double Solar,
                         double Battery) :
                        _mTemperature(Temperature),
                        _mPressure(Pressure),
                        _mHumidity(Humidity),
                        _mAmbientLight(AmbientLight),
                        _mIAQ(IAQ),
                        _mGas(Gas),
                        _mSolar(Solar),
                        _mBattery(Battery),
                        _mUV(UV),
                        _mIAQValid(IAQValid),
                        _mGasValid(GasValid)
{
}

QString WeatherData::toCSV(void) const
{
    return QString("%1;%2;%3;%4;%5;%6;%7;%8;%9;%10;%11").arg(this->_mTemperature) \
                                                        .arg(this->_mPressure) \
                                                        .arg(this->_mHumidity) \
                                                        .arg(this->_mAmbientLight) \
                                                        .arg(this->_mUV) \
                                                        .arg(this->_mIAQ) \
                                                        .arg(this->_mIAQValid) \
                                                        .arg(this->_mGas) \
                                                        .arg(this->_mGasValid) \
                                                        .arg(this->_mSolar) \
                                                        .arg(this->_mBattery);
}

double WeatherData::temperature(void) const
{
    return this->_mTemperature;
}

double WeatherData::pressure(void) const
{
    return this->_mPressure;
}

double WeatherData::humidity(void) const
{
    return this->_mHumidity;
}

double WeatherData::ambientLight(void) const
{
    return this->_mAmbientLight;
}

double WeatherData::iaq(void) const
{
    return this->_mIAQ;
}

double WeatherData::gas(void) const
{
    return this->_mGas;
}

double WeatherData::solar(void) const
{
    return this->_mSolar;
}

double WeatherData::battery(void) const
{
    return this->_mBattery;
}

int WeatherData::uv(void) const
{
    return this->_mUV;
}

bool WeatherData::gasValid(void) const
{
    return this->_mGasValid;
}

bool WeatherData::iaqValid(void) const
{
    return this->_mIAQValid;
}

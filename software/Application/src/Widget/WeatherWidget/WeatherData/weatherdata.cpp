#include "weatherdata.h"

WeatherData::WeatherData() : _mTemperature(0.0),
                             _mPressure(0.0),
                             _mHumidity(0.0),
                             _mAmbientLight(0.0),
                             _mUV(0),
                             _mIAQ(0)
{
}

WeatherData::WeatherData(double Temperature, double Pressure, double Humidity, double AmbientLight, int UV, double IAQ) : _mTemperature(Temperature),
                                                                                                                          _mPressure(Pressure),
                                                                                                                          _mHumidity(Humidity),
                                                                                                                          _mAmbientLight(AmbientLight),
                                                                                                                          _mUV(UV),
                                                                                                                          _mIAQ(IAQ)
{
}

QString WeatherData::toCSV(void) const
{
    return QString("%1;%2;%3;%4;%5;%6").arg(this->_mTemperature) \
                                       .arg(this->_mPressure) \
                                       .arg(this->_mHumidity) \
                                       .arg(this->_mAmbientLight) \
                                       .arg(this->_mUV) \
                                       .arg(this->_mIAQ);
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

int WeatherData::uv(void) const
{
    return this->_mUV;
}

double WeatherData::iaq(void) const
{
    return this->_mIAQ;
}

#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include <QString>

class WeatherData
{
    public:
        WeatherData();
        WeatherData(double Temperature, double Pressure, double Humidity, double AmbientLight, int UV, double IAQ);

        QString toCSV(void) const;
        double temperature(void) const;
        double pressure(void) const;
        double humidity(void) const;
        double ambientLight(void) const;
        int uv(void) const;
        double iaq(void) const;

    private:
        double _mTemperature;
        double _mPressure;
        double _mHumidity;
        double _mAmbientLight;
        int _mUV;
        double _mIAQ;
};

#endif // WEATHERDATA_H

#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include <QString>

class WeatherData
{
    public:
        WeatherData();
        WeatherData(double Temperature, double Pressure, double Humidity, double AmbientLight, int UV, double IAQ, bool IAQValid, double Gas, bool GasValid, double Solar, double Battery);

        QString toCSV(void) const;
        double temperature(void) const;
        double pressure(void) const;
        double humidity(void) const;
        double ambientLight(void) const;
        double iaq(void) const;
        double gas(void) const;
        double solar(void) const;
        double battery(void) const;
        int uv(void) const;
        bool gasValid(void) const;
        bool iaqValid(void) const;

    private:
        double _mTemperature;
        double _mPressure;
        double _mHumidity;
        double _mAmbientLight;
        double _mIAQ;
        double _mGas;
        double _mSolar;
        double _mBattery;
        int _mUV;
        bool _mIAQValid;
        bool _mGasValid;
};

#endif // WEATHERDATA_H

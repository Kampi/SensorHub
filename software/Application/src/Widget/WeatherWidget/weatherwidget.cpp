#include "weatherwidget.h"
#include "ui_weatherwidget.h"

WeatherWidget::WeatherWidget(QWidget* parent) : QWidget(parent),
                                                _mUi(new Ui::WeatherWidget),
                                                _mTemperatureTile(new WeatherTile(QPixmap(":/Icons/Ressources/Icons/icons8-thermometer-96.png"), "°C", this)),
                                                _mHumidityTile(new WeatherTile(QPixmap(":/Icons/Ressources/Icons/icons8-hygrometer-96.png"), "%RH", this)),
                                                _mPressureTile(new WeatherTile(QPixmap(":/Icons/Ressources/Icons/icons8-pressure-gauge-96.png"), "hPa", this)),
                                                _mAmbientLightTile(new WeatherTile(QPixmap(":/Icons/Ressources/Icons/icons8-sun-96.png"), "lux", this)),
                                                _mUVTile(new WeatherTile(QPixmap(":/Icons/Ressources/Icons/icons8-sunlight-96.png"), "", this)),
                                                _mAirQualityTile(new WeatherTile(QPixmap(":/Icons/Ressources/Icons/icons8-air-96.png"), "%", this))
{
    _mUi->setupUi(this);
    this->_mUi->gridLayout->addWidget(this->_mTemperatureTile, 0, 1, 1, 1);
    this->_mUi->gridLayout->addWidget(this->_mHumidityTile, 1, 1, 1, 1);
    this->_mUi->gridLayout->addWidget(this->_mPressureTile, 0, 2, 1, 1);
    this->_mUi->gridLayout->addWidget(this->_mAirQualityTile, 1, 2, 1, 1);
    this->_mUi->gridLayout->addWidget(this->_mAmbientLightTile, 0, 3, 1, 1);
    this->_mUi->gridLayout->addWidget(this->_mUVTile, 1, 3, 1, 1);
}

WeatherWidget::~WeatherWidget()
{
    delete _mUi;
}

void WeatherWidget::update(const WeatherData& arg)
{
    this->_mTemperatureTile->update(arg.temperature());
    this->_mHumidityTile->update(arg.humidity());
    this->_mPressureTile->update(arg.pressure());
    this->_mAmbientLightTile->update(arg.ambientLight());
    this->_mUVTile->update(arg.uv());
    this->_mAirQualityTile->update(arg.iaq());
}

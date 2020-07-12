#ifndef WEATHERWIDGET_H
#define WEATHERWIDGET_H

#include <QDebug>
#include <QWidget>
#include <QPixmap>

#include "WeatherTile/weathertile.h"
#include "WeatherData/weatherdata.h"

namespace Ui
{
    class WeatherWidget;
}

class WeatherWidget : public QWidget
{
    Q_OBJECT

    public slots:
        void update(const WeatherData& arg);

    public:
        explicit WeatherWidget(QWidget* parent = nullptr);
        ~WeatherWidget();

    private:
        Ui::WeatherWidget* _mUi;

        int _mAirQuality;

        WeatherTile* _mTemperatureTile;
        WeatherTile* _mHumidityTile;
        WeatherTile* _mPressureTile;
        WeatherTile* _mAmbientLightTile;
        WeatherTile* _mUVTile;
        WeatherTile* _mAirQualityTile;
};

#endif // WEATHERWIDGET_H

#ifndef WEATHERTILE_H
#define WEATHERTILE_H

#include <QFrame>
#include <QTimer>
#include <QWidget>
#include <QLocale>
#include <QPainter>
#include <QStyleOption>

namespace Ui
{
    class WeatherTile;
}

class WeatherTile : public QFrame
{
    Q_OBJECT

    protected:

    public slots:
        void update(const double& arg);

    public:
        explicit WeatherTile(const QPixmap& Image, const QString& Suffix, QWidget* parent = nullptr);
        ~WeatherTile();

        void suffix(const QString& Suffix);
        void image(const QPixmap& Image);

    private slots:
        void _timer(void);

    private:
        Ui::WeatherTile* _mUi;
        QTimer* _mTimer;

        QString _mSuffix;
        QPixmap _mImage;

        int _mValueIndex;

        double _mValue;
        double _mMax;
        double _mMin;
};

#endif // WEATHERTILE_H

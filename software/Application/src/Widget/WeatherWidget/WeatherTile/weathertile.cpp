#include "weathertile.h"
#include "ui_weathertile.h"

WeatherTile::WeatherTile(const QPixmap& Image, const QString& Suffix, QWidget* parent) : QFrame(parent),
                                                                                          _mUi(new Ui::WeatherTile),
                                                                                          _mTimer(new QTimer(this)),
                                                                                          _mSuffix(Suffix),
                                                                                          _mValueIndex(0),
                                                                                          _mValue(0.0),
                                                                                          _mMax(0.0),
                                                                                          _mMin(0.0)
{
    this->_mUi->setupUi(this);
    this->_mUi->label_Image->setPixmap(Image);

    setAttribute(Qt::WA_StyledBackground, true);

    connect(this->_mTimer, &QTimer::timeout, this, &WeatherTile::_timer);
    this->_mTimer->start(3000);
    this->_timer();
}

WeatherTile::~WeatherTile()
{
    delete _mUi;
}

void WeatherTile::suffix(const QString& Suffix)
{
    this->_mSuffix = Suffix;
}

void WeatherTile::image(const QPixmap& Image)
{
    this->_mUi->label_Image->setPixmap(Image);
}

void WeatherTile::update(const double& arg)
{
    if((this->_mMax == 0.0) && (this->_mMin == 0.0))
    {
        this->_mMax = arg;
        this->_mMin = arg;
    }
    else if(arg > this->_mMax)
    {
        this->_mMax = arg;
    }
    else if(arg < this->_mMin)
    {
        this->_mMin = arg;
    }

    this->_mValue = arg;
}

void WeatherTile::_timer(void)
{
    double CurrentValue = 0.0;

    switch(this->_mValueIndex)
    {
        case 0:
        {
            this->_mUi->label_MinMax->setText("Now");
            CurrentValue = this->_mValue;
            break;
        }
        case 1:
        {
            this->_mUi->label_MinMax->setText("Min");
            CurrentValue = this->_mMin;
            break;
        }
        case 2:
        {
            this->_mUi->label_MinMax->setText("Max");
            CurrentValue = this->_mMax;
            break;
        }
    }

    if(this->_mValueIndex < 2)
    {
        this->_mValueIndex++;
    }
    else
    {
        this->_mValueIndex = 0x00;
    }

    this->_mUi->label_Value->setText(QLocale::system().toString(CurrentValue, 'g', 4) + " " + this->_mSuffix);
}

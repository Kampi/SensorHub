#include "chartwidget.h"
#include "ui_chartwidget.h"

ChartWidget::ChartWidget(QString Title, QWidget* parent) : QChartView(parent),
                                                           _mUi(new Ui::ChartWidget),
                                                           _mXAxis(new QDateTimeAxis(this)),
                                                           _mChart(new QChart()),
                                                           _mYAxis(new QValueAxis(this)),
                                                           _mSeries(new QSplineSeries(this)),
                                                           _mColor(QColor(Qt::red)),
                                                           _mPrevious(QDateTime::currentDateTime().toSecsSinceEpoch()),
                                                           _mTime(24 * 60 *60)
{
    this->_mUi->setupUi(this);

    this->_mXAxis->setTickCount(10);
    this->_mXAxis->setFormat("hh:mm");
    this->_mXAxis->setTitleText("Time");
    this->_mXAxis->setMin(QDateTime::currentDateTime().addSecs(-this->_mTime));
    this->_mXAxis->setMax(QDateTime::currentDateTime().addSecs(5));

    this->_mYAxis->setTitleText(Title);

    this->_mChart->addSeries(this->_mSeries);
    this->_mChart->addAxis(this->_mYAxis, Qt::AlignLeft);
    this->_mChart->addAxis(this->_mXAxis, Qt::AlignBottom);
    this->_mChart->legend()->setVisible(false);

    this->_mSeries->attachAxis(this->_mXAxis);
    this->_mSeries->attachAxis(this->_mYAxis);

    connect(this->_mSeries, &QSplineSeries::pointAdded, [=](int index)
    {
        qreal y = this->_mSeries->at(index).y();

        if((y < this->_mMin) || (y > this->_mMax))
        {
            if(y < this->_mMin)
            {
                this->_mMin = y;
            }
            if(y > this->_mMax)
            {
                this->_mMax = y;
            }

            this->_mYAxis->setRange(this->_mMin * 0.90, this->_mMax * 1.1);
        }
    });

    setChart(this->_mChart);
}

ChartWidget::~ChartWidget()
{
    delete this->_mUi;
}

void ChartWidget::zoomIn(void)
{
    this->_mChart->zoomIn();
}

void ChartWidget::zoomOut(void)
{
    this->_mChart->zoomOut();
}

void ChartWidget::zoomReset(void)
{
    this->_mChart->zoomReset();
}

void ChartWidget::setColor(QColor& Color)
{
    this->_mSeries->setColor(Color);
}

QColor ChartWidget::color(void) const
{
    return this->_mSeries->color();
}

void ChartWidget::setPen(QPen& Pen)
{
    this->_mSeries->setPen(Pen);
}

void ChartWidget::exportChart(void)
{
    QString FileName = QFileDialog::getSaveFileName(this, tr("Export chart"), "", "Portable Network Graphic (*.png)");

    if(FileName.length())
    {
        QPixmap Pixmap = grab();
        Pixmap.save(FileName);
    }
}

void ChartWidget::AddDataPoint(double Value)
{
    QDateTime Now = QDateTime::currentDateTime();

    qreal dx = this->_mChart->plotArea().width() / this->_mTime;
    this->_mSeries->append(Now.toMSecsSinceEpoch(), Value);
    this->_mChart->scroll(dx * (Now.toSecsSinceEpoch() - this->_mPrevious), 0);

    if(_mSeries->count() > 600)
    {
        this->_mSeries->removePoints(0, this->_mSeries->count() - 600);
    }

    this->_mPrevious = Now.toSecsSinceEpoch();
}

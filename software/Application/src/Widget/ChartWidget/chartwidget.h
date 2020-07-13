#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QtCharts>

#include <QDebug>

namespace Ui
{
    class ChartWidget;
}

class ChartWidget : public QChartView
{
    Q_OBJECT

    public:
        explicit ChartWidget(QString Title, QWidget* parent = nullptr);
        ~ChartWidget();

        void zoomIn(void);
        void zoomOut(void);
        void zoomReset(void);
        void setColor(QColor& Color);
        QColor color(void) const;
        void setPen(QPen& Pen);
        void exportChart(void);
        void AddDataPoint(double Value);

    private:
        Ui::ChartWidget* _mUi;

        QDateTimeAxis* _mXAxis;
        QChart* _mChart;
        QValueAxis* _mYAxis;
        QSplineSeries* _mSeries;

        QColor _mColor;

        qreal _mMin;
        qreal _mMax;

        qint64 _mPrevious;

        double _mTime;
};

#endif // CHARTWIDGET_H

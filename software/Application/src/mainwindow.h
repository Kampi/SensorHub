#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QString>
#include <QtCharts>
#include <QMainWindow>
#include <QMessageBox>
#include <QtMqtt/QtMqtt>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

#include "chartwidget.h"
#include "weatherwidget.h"
#include "aboutdialog.h"
#include "settingsdialog.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    protected slots:
        void languageChange(bool checked);

    protected:
        void closeEvent(QCloseEvent* event);

    public:
        explicit MainWindow(QWidget* parent = nullptr);
        ~MainWindow();

    private slots:
        void Settings_finished(int result);
        void MQTT_Connected(void);
        void MQTT_Received(const QByteArray& message, const QMqttTopicName& topic);
        void on_action_Connect_triggered();
        void on_action_Disconnect_triggered();
        void on_action_Settings_triggered();
        void on_action_About_triggered();
        void on_action_AboutQt_triggered();
        void on_action_Exit_triggered();
        void on_action_Color_triggered();
        void on_action_StopRecord_triggered();
        void on_action_StartRecord_triggered();
        void on_tabWidget_Plots_currentChanged(int index);

    private:
        Ui::MainWindow* _mUi;
        QMqttClient* _mClient;
        QLabel* _mConnectionState;
        QTimer* _mUpdateTimer;
        WeatherWidget* _mWeatherWidget;
        ChartWidget* _mTemperatureWidget;
        ChartWidget* _mHumidityWidget;
        ChartWidget* _mPressureWidget;
        ChartWidget* _mAmbientLightWidget;
        ChartWidget* _mIAQWidget;

        QTranslator _mTranslator;
        QString _mIp;
        QString _mLog;

        int _mPort;

        bool _mLoggingActive;

        void _disconnect(void);
        void _createLog(QString Path);
        void _appendLog(QString Message);
        void _setLanguageMenu(void);
        void _switchLanguage(QString Language);
        void _update(void);
};

#endif // MAINWINDOW_H

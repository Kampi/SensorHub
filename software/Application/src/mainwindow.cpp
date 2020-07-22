#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
                                          _mUi(new Ui::MainWindow),
                                          _mClient(new QMqttClient(this)),
                                          _mConnectionState(new QLabel(this)),
                                          _mUpdateTimer(new QTimer(this)),
                                          _mWeatherWidget(new WeatherWidget(this)),
                                          _mTemperatureWidget(new ChartWidget(tr("Temperature") + " [°C]", this)),
                                          _mHumidityWidget(new ChartWidget(tr("Humidity") + " [% r.H.]", this)),
                                          _mPressureWidget(new ChartWidget(tr("Pressure") + " [hPa]", this)),
                                          _mAmbientLightWidget(new ChartWidget(tr("Ambient light") + " [Lux]", this)),
                                          _mIAQWidget(new ChartWidget(tr("Air quality") + " [%]", this)),
                                          _mIp("127.0.0.1"),
                                          _mPort(1883),
                                          _mLoggingActive(false)
{
    this->_mUi->setupUi(this);

    connect(this->_mClient, &QMqttClient::messageReceived, this, &MainWindow::MQTT_Received);
    connect(this->_mClient, &QMqttClient::connected, this, &MainWindow::MQTT_Connected);
    connect(this->_mUpdateTimer, &QTimer::timeout, this, &MainWindow::_update);

    this->_mUi->statusBar->showMessage(tr("Disconnected"));
    this->_mUi->statusBar->addPermanentWidget(_mConnectionState);

    this->showFullScreen();
    this->setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    this->statusBar()->setSizeGripEnabled(false);
    this->_setLanguageMenu();

    this->_mUi->tabWidget_Plots->addTab(this->_mWeatherWidget, tr("Current"));
    this->_mUi->tabWidget_Plots->addTab(this->_mTemperatureWidget, tr("Temperature"));
    this->_mUi->tabWidget_Plots->addTab(this->_mHumidityWidget, tr("Humidity"));
    this->_mUi->tabWidget_Plots->addTab(this->_mPressureWidget, tr("Pressure"));
    this->_mUi->tabWidget_Plots->addTab(this->_mAmbientLightWidget, tr("Ambient light"));
    this->_mUi->tabWidget_Plots->addTab(this->_mIAQWidget, tr("Air quality"));

    this->_mUpdateTimer->start(100);
}

MainWindow::~MainWindow()
{
    delete this->_mUi;
}

void MainWindow::Settings_finished(int result)
{
    if(result == 1)
    {
        SettingsDialog* Dialog = qobject_cast<SettingsDialog*>(sender());

        this->_mIp = Dialog->ip();
        this->_mPort = Dialog->port();
    }
}

void MainWindow::MQTT_Connected(void)
{
    if(!this->_mClient->subscribe(QString("sensorhub/weather")))
    {
        this->_mUi->statusBar->showMessage(tr("Could not subscribe to topic sensorhub/weather"));

        return;
    }

    this->_mUi->statusBar->showMessage(tr("Subscribe to topic sensorhub/weather"));
}

void MainWindow::MQTT_Received(const QByteArray& message, const QMqttTopicName&)
{
    // Convert the message into a QString to remove all additional bytes that the microcontroller sends.
    // Unfortunately the JSON library from Particle will transmit the complete buffer and not only the JSON
    // string. This will cause some issues to the Qt JSON parser.
    QJsonDocument doc = QJsonDocument::fromJson(QString(message).toUtf8());

    #ifdef QT_DEBUG
        qDebug() << doc;
    #endif

    WeatherData Data(doc.object().value("Temperature").toDouble(),
                     doc.object().value("Pressure").toDouble(),
                     doc.object().value("Humidity").toDouble(),
                     doc.object().value("Ambient light").toDouble(),
                     doc.object().value("UV").toInt(),
                     doc.object().value("IAQ").toDouble(),
                     doc.object().value("IAQ valid").toBool(),
                     doc.object().value("Gas").toDouble(),
                     doc.object().value("Gas valid").toBool(),
                     doc.object().value("Solar").toDouble(),
                     doc.object().value("Battery").toDouble()
                     );

    if(this->_mLoggingActive)
    {
        QString Message = QString("%1;" + Data.toCSV()).arg(QDateTime::currentDateTime().toSecsSinceEpoch());

        #ifdef QT_DEBUG
            qDebug() << "Logging: " << Message;
        #endif

        this->_appendLog(Message);
    }

    this->_mTemperatureWidget->AddDataPoint(Data.temperature());
    this->_mHumidityWidget->AddDataPoint(Data.humidity());
    this->_mPressureWidget->AddDataPoint(Data.pressure());
    this->_mAmbientLightWidget->AddDataPoint(Data.ambientLight());
    this->_mIAQWidget->AddDataPoint(Data.iaq());
    this->_mWeatherWidget->update(Data);

    this->_mUi->statusBar->showMessage(tr("Update: ") + QDateTime::currentDateTime().toString("hh:mm:ss"));
}

void MainWindow::on_action_Connect_triggered()
{
    this->_connect();
}

void MainWindow::on_action_Disconnect_triggered()
{
    this->_disconnect();
}

void MainWindow::on_action_Settings_triggered()
{
    SettingsDialog* Dialog = new SettingsDialog(this->_mIp, this->_mPort, this);
    connect(Dialog, &QDialog::finished, this, &MainWindow::Settings_finished);
    Dialog->show();
}

void MainWindow::on_action_About_triggered()
{
    AboutDialog* Dialog = new AboutDialog(this);
    Dialog->show();
}

void MainWindow::on_action_AboutQt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_action_Exit_triggered()
{
    this->close();
}

void MainWindow::on_action_Color_triggered()
{
    ChartWidget* Chart = qobject_cast<ChartWidget*>(this->_mUi->tabWidget_Plots->currentWidget());

    QColor Color = QColorDialog::getColor(Chart->color(), this, tr("Choose color"));
    if(Color.isValid())
    {
        Chart->setColor(Color);
    }
}

void MainWindow::on_action_StopRecord_triggered()
{
    this->_mUi->action_StartRecord->setEnabled(true);
    this->_mUi->action_StopRecord->setEnabled(false);
    this->_mLoggingActive = false;
}

void MainWindow::on_action_StartRecord_triggered()
{
    QString FileName = QFileDialog::getSaveFileName(this, tr("Save log"), "Log", "Comma-seperated values (*.csv)");

    if(FileName.length())
    {
        this->_mLog = FileName + ".csv";
        this->_createLog(this->_mLog);
        this->_mUi->action_StartRecord->setEnabled(false);
        this->_mUi->action_StopRecord->setEnabled(true);
        this->_mLoggingActive = true;
    }
}

void MainWindow::on_tabWidget_Plots_currentChanged(int index)
{
    if(index == 0x00)
    {
        this->_mUi->menu_Plot->setEnabled(false);
    }
    else
    {
        this->_mUi->menu_Plot->setEnabled(true);
    }
}

void MainWindow::languageChange(bool)
{
    QAction* Action = qobject_cast<QAction*>(sender());
    Action->setChecked(true);
    this->_switchLanguage(Action->iconText());
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    this->_disconnect();

    QMainWindow::closeEvent(event);
}

void MainWindow::_connect(void)
{
    if(this->_mClient->state() == QMqttClient::Disconnected)
    {
        this->_mUi->action_Connect->setEnabled(false);
        this->_mUi->action_Settings->setEnabled(false);
        this->_mUi->action_Disconnect->setEnabled(true);
        this->_mUi->action_StartRecord->setEnabled(true);
        this->_mClient->setHostname(this->_mIp);
        this->_mClient->setPort(this->_mPort);
        this->_mClient->connectToHost();
    }
}

void MainWindow::_disconnect(void)
{
    if(this->_mClient->state() == QMqttClient::Connected)
    {
        this->_mClient->disconnectFromHost();
    }

    this->_mUi->action_Disconnect->setEnabled(false);
    this->_mUi->action_StartRecord->setEnabled(false);
    this->_mUi->action_Connect->setEnabled(true);
    this->_mUi->action_Settings->setEnabled(true);
    this->_mUi->statusBar->showMessage(tr("Disconnected"));
}

void MainWindow::_createLog(QString Path)
{
    QFile CSV(Path);
    CSV.open(QIODevice::WriteOnly);
    QTextStream Out(&CSV);
    Out << "Date;Temperature;Pressure;Humidity;Ambient light;UV;IAQ;IAQ valid;Gas;Gas valid;Solar;Battery";
    endl(Out);
    CSV.close();
}

void MainWindow::_appendLog(QString Message)
{
    QFile CSV(this->_mLog);
    CSV.open(QIODevice::Append);
    QTextStream Out(&CSV);
    Out << Message;
    endl(Out);
    CSV.close();
}

void MainWindow::_setLanguageMenu(void)
{
    QActionGroup* actionGroup = new QActionGroup(this);
    actionGroup->addAction(this->_mUi->action_EnglishUS);
    actionGroup->setExclusive(true);

    connect(this->_mUi->action_EnglishUS, &QAction::triggered, this, &MainWindow::languageChange);

    // Use english as default language
    this->_mUi->action_EnglishUS->triggered(true);
}

void MainWindow::_switchLanguage(QString Language)
{
    QLocale::setDefault(QLocale(Language));

    qApp->removeTranslator(&this->_mTranslator);

    if(this->_mTranslator.load(QString(QApplication::applicationDirPath().append(QString("/Languages/FoodDatabase_%1.qm").arg(Language)))))
    {
        qApp->installTranslator(&this->_mTranslator);
    }
}

void MainWindow::_update(void)
{
    QString FileName;

    if(this->_mClient->state() == QMqttClient::Connected)
    {
        FileName = ":/Icons/Ressources/Icons/icons8-connected-24.png";
    }
    else
    {
        FileName = ":/Icons/Ressources/Icons/icons8-disconnected-24.png";
    }

    QPixmap Pixmap;
    Pixmap.load(FileName);
    _mConnectionState->setPixmap(Pixmap);
}

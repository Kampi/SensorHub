VERSION_MAJOR = 1
VERSION_MINOR = 0
VERSION_BUILD = 0

QT       += core gui mqtt charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SensorHub
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
            "VERSION_MAJOR=$$VERSION_MAJOR" \
            "VERSION_MINOR=$$VERSION_MINOR" \
            "VERSION_BUILD=$$VERSION_BUILD" \

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11 file_copies

COPIES += languageFiles packageSetup configSetup

RESOURCES += \
    Ressources.qrc

SOURCES += \
        Dialog/AboutDialog/aboutdialog.cpp \
        Dialog/SettingsDialog/settingsdialog.cpp \
        Widget/ChartWidget/chartwidget.cpp \
        Widget/WeatherWidget/WeatherData/weatherdata.cpp \
        Widget/WeatherWidget/WeatherTile/weathertile.cpp \
        Widget/WeatherWidget/weatherwidget.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        Dialog/AboutDialog/aboutdialog.h \
        Dialog/SettingsDialog/settingsdialog.h \
        Widget/ChartWidget/chartwidget.h \
        Widget/WeatherWidget/WeatherData/weatherdata.h \
        Widget/WeatherWidget/WeatherTile/weathertile.h \
        Widget/WeatherWidget/weatherwidget.h \
        mainwindow.h

FORMS += \
        Dialog/AboutDialog/aboutdialog.ui \
        Dialog/SettingsDialog/settingsdialog.ui \
        Widget/ChartWidget/chartwidget.ui \
        Widget/WeatherWidget/WeatherTile/weathertile.ui \
        Widget/WeatherWidget/weatherwidget.ui \
        mainwindow.ui

INCLUDEPATH += \
        Dialog/SettingsDialog \
        Dialog/AboutDialog \
        Widget/ChartWidget \
        Widget/WeatherWidget \

TRANSLATIONS += \
    Languages/SensorHub_de_DE.ts \
    Languages/SensorHub_en_US.ts \

# Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}

# Set the build directories
CONFIG(debug, debug|release) {
    DESTDIR = debug
} else {
    DESTDIR = ../packages/com.kampis-elektroecke.SensorHub/data
}

# Copy language files
languageFiles.files = $$files($${PWD}/Languages/*.qm)
CONFIG(debug, debug|release) {
    languageFiles.path = $${OUT_PWD}/debug/Languages
} else {
    languageFiles.path = $${DESTDIR}/Languages
}

# Copy package and configuration files
CONFIG(debug, debug|release) {
    packageSetup.path = $${OUT_PWD}/debug
    configSetup.path = $${OUT_PWD}/debug
} else {
    packageSetup.path = $${DESTDIR}
    configSetup.path = ../config
}

# Deployment rule
CONFIG(debug, debug|release) {
    target.path = /home/pi/$${TARGET}/bin
    INSTALLS += target
} else {
    target.path = /home/pi/$${TARGET}/bin
    INSTALLS += target
}

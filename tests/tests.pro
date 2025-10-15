# tests/tests.pro
QT += testlib core
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle
CONFIG += c++17

TEMPLATE = app
TARGET = tst_weathercachemanager

# Chemin vers le code source
INCLUDEPATH += ../src

# Fichier de test principal
SOURCES += \
    tst_weathercachemanager.cpp

# Code source à tester
# IMPORTANT : Ne pas inclure main.cpp ni mainwindow (dépendent de QApplication)
SOURCES += \
    ../src/weathercachemanager.cpp \
    ../src/configloader.cpp \
    ../src/weatherservice.cpp

HEADERS += \
    ../src/weathercachemanager.h \
    ../src/ICacheManager.h \
    ../src/WeatherData.h \
    ../src/weathererrors.h \
    ../src/configloader.h \
    ../src/weatherservice.h

# Définir les mêmes deprecated warnings
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# Sortie dans un dossier séparé
DESTDIR = $$OUT_PWD/bin

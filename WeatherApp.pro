# WeatherApp.pro (racine - projet SUBDIRS)
TEMPLATE = subdirs

SUBDIRS += \
    src \
    tests

# Les tests dépendent du code source
tests.depends = src

CONFIG += ordered

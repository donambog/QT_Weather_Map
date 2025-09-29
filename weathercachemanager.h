#ifndef WEATHERCACHEMANAGER_H
#define WEATHERCACHEMANAGER_H
#include "WeatherData.h"
#include <QObject>
#include <qstring.h>
#include <Qlist>
#include <QMap>

using WeatherCache = QMap<QString, CachedWeatherData>;  // cityName → données+métadata
using ForecastCache = QMap<QString, CachedForecastData> ;
class weathercachemanager:public QObject
{
    Q_OBJECT
private:
    //cache
    WeatherCache m_weatherCache;
    ForecastCache m_forecastCache;
    int clear();

public:
    weathercachemanager();

    // Gestion cache
    void signalCacheCleared();
    int cleanExpiredCache();
    bool isValid(const QString& cityName, const QString& dataType) const;
    void storeCachedWeather(const QString& cityName, const CurrentWeatherData& data);
    void storeCachedForecast(const QString& cityName, const ForecastData& data);
    CurrentWeatherData getCityweatherInCache(const QString& cityName) const;

signals:
    /**
     * Émis lors du nettoyage du cache
     *
     * @param removedCount Nombre d'entrées supprimées
     */
    void cacheCleanedUp(int removedCount);
};

#endif // WEATHERCACHEMANAGER_H

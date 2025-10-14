#ifndef WEATHERCACHEMANAGER_H
#define WEATHERCACHEMANAGER_H
#include "WeatherData.h"
#include "ICacheManager.h"
#include <QObject>
#include <qstring.h>
#include <Qlist>
#include <QMap>

using WeatherCache = QMap<QString, CachedWeatherData>;  // cityName → données+métadata
using ForecastCache = QMap<QString, CachedForecastData> ;
class weathercachemanager:public QObject, ICacheManager
{
    Q_OBJECT
private:
    //save the weather
    WeatherCache m_weatherCache;
    //save the forecast
    ForecastCache m_forecastCache;
    int clear();

public:
    weathercachemanager();

    /**
     * Send signal that cache have been cleared
     */
    virtual void signalCacheCleared() override;
    /**
     * Clean the cache
     */
    int cleanExpiredCache() override;
    /**
     * check the cache validity
     * param @cityName : name of the city
     * param @dataType : weather or forecast
     */
    bool isValid(const QString& cityName, const QString& dataType) const;
    /**
     * store the weater
     * @param cityName
     * @param CurrentWeatherData
     */
    void storeCachedWeather(const QString& cityName, const CurrentWeatherData& data) override;
    void storeCachedForecast(const QString& cityName, const ForecastData& data) override;
    /**
     * return the weather in cache
     * @param cityName
     * @param returned weather.
     */
    CurrentWeatherData getCityweatherInCache(const QString& cityName) const override;

signals:
    /**
     * Émis lors du nettoyage du cache
     * @param removedCount Nombre d'entrées supprimées
     */
    void cacheCleanedUp(int removedCount);
};

#endif // WEATHERCACHEMANAGER_H

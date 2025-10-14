#ifndef ICACHEMANAGER_H
#define ICACHEMANAGER_H
#include "WeatherData.h"
#include <QObject>
#include <qstring.h>
#include <Qlist>
#include <QMap>

class ICacheManager
{
public:
    virtual ~ICacheManager()=default;

    virtual void signalCacheCleared() = 0;
    virtual int cleanExpiredCache() = 0;
    //virtual bool isValid(const QString& cityName, const QString& dataType) const = 0;

    virtual void storeCachedWeather(const QString& cityName, const CurrentWeatherData& data) = 0;
    virtual void storeCachedForecast(const QString& cityName, const ForecastData& data) = 0;
    virtual CurrentWeatherData getCityweatherInCache(const QString& cityName) const = 0;


};
#endif // ICACHEMANAGER_H

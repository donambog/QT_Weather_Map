#include "weathercachemanager.h"

weathercachemanager::weathercachemanager(WeatherCache weatherCache, ForecastCache forcastCache):
    m_weatherCache(weatherCache),m_forecastCache(forcastCache)
{

}

int weathercachemanager::clear()
{
    int count = m_weatherCache.size() + m_forecastCache.size();
    m_weatherCache.clear();
    m_forecastCache.clear();
    qDebug() << "Cache cleared -" << count << "entries removed";
    return count;
}

void weathercachemanager::signalCacheCleared(){
    emit cacheCleanedUp(clear());
}

int weathercachemanager::cleanExpiredCache()
{
    int removed = 0;
    // Nettoyage cache météo
    auto weatherIt = m_weatherCache.begin();
    while (weatherIt != m_weatherCache.end()) {
        if (!weatherIt.value().cacheInfo.isValid()) {
            weatherIt = m_weatherCache.erase(weatherIt);
            removed++;
        } else {
            ++weatherIt;
        }
    }

    // Nettoyage cache prévisions
    auto forecastIt = m_forecastCache.begin();
    while (forecastIt != m_forecastCache.end()) {
        if (!forecastIt.value().cacheInfo.isValid()) {
            forecastIt = m_forecastCache.erase(forecastIt);
            removed++;
        } else {
            ++forecastIt;
        }
    }
    return removed;
}

bool weathercachemanager::isValid(const QString& cityName, const QString& dataType) const
{
    if (dataType == "weather") {
        return m_weatherCache.contains(cityName) &&
               m_weatherCache[cityName].cacheInfo.isValid();
    } else if (dataType == "forecast") {
        return m_forecastCache.contains(cityName) &&
               m_forecastCache[cityName].cacheInfo.isValid();
    }
    return false;
}

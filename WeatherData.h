#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include <QString>
#include <QDateTime>
#include <QList>
#include <QMetaType>

/**
 * Structure pour les données météorologiques actuelles
 * Correspond à la réponse de l'API /weather
 */
struct CurrentWeatherData {
    // Identification ville
    QString cityName;           // "Paris"
    QString countryCode;        // "FR"
    qint64 cityId;             // ID unique OpenWeatherMap

    // Position géographique
    double latitude;            // 48.8534
    double longitude;           // 2.3488

    // Données météo principales
    double temperature;         // 15.5 (°C)
    double feelsLike;          // 14.2 (°C ressenti)
    double temperatureMin;      // 12.1 (min du jour)
    double temperatureMax;      // 18.3 (max du jour)

    // Conditions atmosphériques
    QString mainCondition;      // "Clear", "Clouds", "Rain"
    QString description;        // "ciel dégagé", "peu nuageux"
    QString iconCode;          // "01d", "02n", etc.
    int conditionId;           // 800, 801, etc.

    // Paramètres météo
    double humidity;           // 65 (%)
    double pressure;           // 1013 (hPa)
    double windSpeed;          // 3.2 (m/s)
    int windDirection;         // 230 (degrés)
    double visibility;         // 10000 (mètres)
    int cloudiness;           // 15 (% couverture nuageuse)

    // Informations temporelles
    QDateTime timestamp;       // Moment des données
    QDateTime sunrise;         // Lever soleil
    QDateTime sunset;          // Coucher soleil
    int timezone;             // Décalage UTC en secondes

    // Constructeur par défaut
    CurrentWeatherData()
        : cityId(0), latitude(0.0), longitude(0.0), temperature(0.0)
        , feelsLike(0.0), temperatureMin(0.0), temperatureMax(0.0)
        , conditionId(0), humidity(0.0), pressure(0.0), windSpeed(0.0)
        , windDirection(0), visibility(0.0), cloudiness(0), timezone(0) {}

    // Méthode utilitaire - Données valides ?
    bool isValid() const {
        return !cityName.isEmpty() && cityId > 0;
    }

    // Méthode utilitaire - Âge des données
    int ageInMinutes() const {
        return QDateTime::currentDateTime().secsTo(timestamp) / 60;
    }
};

/**
 * Structure pour une entrée de prévision (créneau 3h)
 * Correspond à un élément du tableau "list" de /forecast
 */
struct ForecastEntry {
    QDateTime dateTime;         // "2025-09-23 12:00:00"

    // Données météo pour ce créneau
    double temperature;         // Température prévue
    double feelsLike;          // Ressenti prévu
    double humidity;           // Humidité prévue (%)
    double pressure;           // Pression prévue (hPa)

    // Conditions prévues
    QString mainCondition;      // "Clouds", "Rain", etc.
    QString description;        // "partiellement nuageux"
    QString iconCode;          // "03d", "10n", etc.
    int conditionId;           // Code numérique condition

    // Vent et précipitations
    double windSpeed;          // Vitesse vent (m/s)
    int windDirection;         // Direction vent (degrés)
    double windGust;           // Rafales (m/s)
    int cloudiness;           // Couverture nuageuse (%)
    double precipitationProbability; // Probabilité pluie (0-100%)

    // Constructeur par défaut
    ForecastEntry()
        : temperature(0.0), feelsLike(0.0), humidity(0.0), pressure(0.0)
        , conditionId(0), windSpeed(0.0), windDirection(0), windGust(0.0)
        , cloudiness(0), precipitationProbability(0.0) {}
};

/**
 * Structure pour les prévisions complètes 5 jours
 * Correspond à la réponse de l'API /forecast
 */
struct ForecastData {
    QString cityName;           // Nom de la ville
    double latitude;            // Position pour cohérence
    double longitude;

    QList<ForecastEntry> entries;  // 40 créneaux (8 par jour × 5 jours)
    QDateTime retrievedAt;         // Moment de récupération

    // Constructeur par défaut
    ForecastData() : latitude(0.0), longitude(0.0) {}

    // Méthodes utilitaires
    bool isValid() const {
        return !cityName.isEmpty() && !entries.isEmpty();
    }

    int totalEntries() const {
        return entries.size();
    }

    int ageInMinutes() const {
        return QDateTime::currentDateTime().secsTo(retrievedAt) / 60;
    }

    // Obtenir les prévisions d'un jour spécifique (0-4)
    QList<ForecastEntry> getEntriesForDay(int dayIndex) const {
        QList<ForecastEntry> dayEntries;
        int startIndex = dayIndex * 8;  // 8 créneaux par jour
        int endIndex = qMin(startIndex + 8, entries.size());

        for (int i = startIndex; i < endIndex; ++i) {
            dayEntries.append(entries[i]);
        }
        return dayEntries;
    }

    // Résumé quotidien (min/max température du jour)
    struct DailySummary {
        QDate date;
        double minTemp;
        double maxTemp;
        QString dominantCondition;
        QString iconCode;
    };

    QList<DailySummary> getDailySummaries() const {
        QList<DailySummary> summaries;

        for (int day = 0; day < 5; ++day) {
            auto dayEntries = getEntriesForDay(day);
            if (dayEntries.isEmpty()) continue;

            DailySummary summary;
            summary.date = dayEntries.first().dateTime.date();

            // Calcul min/max température
            summary.minTemp = dayEntries.first().temperature;
            summary.maxTemp = dayEntries.first().temperature;

            for (const auto& entry : dayEntries) {
                summary.minTemp = qMin(summary.minTemp, entry.temperature);
                summary.maxTemp = qMax(summary.maxTemp, entry.temperature);
            }

            // Condition dominante (celle de midi si disponible)
            int noonIndex = dayEntries.size() > 4 ? 4 : dayEntries.size() / 2;
            summary.dominantCondition = dayEntries[noonIndex].mainCondition;
            summary.iconCode = dayEntries[noonIndex].iconCode;

            summaries.append(summary);
        }

        return summaries;
    }
};

/**
 * Structure pour les informations de cache
 */
struct CacheInfo {
    QDateTime cachedAt;         // Moment de mise en cache
    int validityMinutes;        // Durée validité (15min weather, 120min forecast)

    bool isValid() const {
        int ageMinutes = QDateTime::currentDateTime().secsTo(cachedAt) / 60;
        return qAbs(ageMinutes) <= validityMinutes;
    }

    int ageMinutes() const {
        return QDateTime::currentDateTime().secsTo(cachedAt) / 60;
    }
};

/**
 * Données météo avec informations de cache
 */
struct CachedWeatherData {
    CurrentWeatherData weatherData;
    CacheInfo cacheInfo;

    CachedWeatherData() {
        cacheInfo.validityMinutes = 15;  // 15 minutes par défaut
    }
};

struct CachedForecastData {
    ForecastData forecastData;
    CacheInfo cacheInfo;

    CachedForecastData() {
        cacheInfo.validityMinutes = 120;  // 2 heures par défaut
    }
};

// Déclarations pour utilisation dans signals/slots Qt
Q_DECLARE_METATYPE(CurrentWeatherData)
Q_DECLARE_METATYPE(ForecastData)
Q_DECLARE_METATYPE(ForecastEntry)
Q_DECLARE_METATYPE(CachedWeatherData)
Q_DECLARE_METATYPE(CachedForecastData)

#endif // WEATHERDATA_H

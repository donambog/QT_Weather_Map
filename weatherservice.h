#ifndef WEATHERSERVICE_H
#define WEATHERSERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QUrlQuery>
#include "WeatherData.h"

/**
 * Service principal pour la gestion des données météorologiques
 *
 * Responsabilités :
 * - Appels API OpenWeatherMap
 * - Gestion du cache intelligent (validité/expiration)
 * - Parsing des réponses JSON → structures typées
 * - Émission de signals pour notifier l'interface
 * - Gestion des erreurs réseau
 *
 * Communication avec l'UI via signals/slots uniquement
 * Service testable indépendamment de l'interface
 */
class WeatherService : public QObject
{
    Q_OBJECT

public:
    explicit WeatherService(QObject* parent = nullptr);
    ~WeatherService();

    // Configuration API
    void setApiKey(const QString& apiKey);
    void setRequestTimeout(int timeoutMs);

    // État du service
    bool isApiKeyValid() const;
    bool hasValidCache(const QString& cityName) const;
    int getCacheAge(const QString& cityName) const;
    QStringList getCachedCities() const;

    // Gestion cache
    void clearCache();
    void clearCacheForCity(const QString& cityName);
    void cleanExpiredCache();

public slots:
    /**
     * Requête météo actuelle pour une ville
     *
     * @param cityName Nom de la ville ("Paris", "London", "New York")
     *
     * Comportement :
     * 1. Vérifie le cache d'abord
     * 2. Si cache valide → émet currentWeatherReady() immédiatement
     * 3. Si cache expiré/absent → appel API + émet loadingStarted()
     * 4. Succès API → émet currentWeatherReady() + mise en cache
     * 5. Erreur API → émet errorOccurred()
     */
    void requestCurrentWeather(const QString& cityName);

    /**
     * Requête prévisions 5 jours pour une ville
     *
     * @param cityName Nom de la ville
     *
     * Comportement similaire à requestCurrentWeather mais pour /forecast
     */
    void requestForecast(const QString& cityName);

    /**
     * Force le rafraîchissement (ignore le cache)
     *
     * @param cityName Ville à rafraîchir
     */
   // void refreshWeatherData(const QString& cityName);

signals:
    // === SIGNAUX DONNÉES MÉTÉO ===

    /**
     * Émis quand les données météo actuelles sont disponibles
     * (depuis cache ou API)
     *
     * @param cityName Ville concernée
     * @param weatherData Données météo complètes et validées
     */
    void currentWeatherReady(const QString& cityName, const CurrentWeatherData& weatherData);

    /**
     * Émis quand les prévisions 5 jours sont disponibles
     *
     * @param cityName Ville concernée
     * @param forecastData Prévisions complètes (40 créneaux)
     */
    void forecastReady(const QString& cityName, const ForecastData& forecastData);

    // === SIGNAUX ÉTAT/PROGRESS ===

    /**
     * Émis au début d'une requête API
     *
     * @param cityName Ville en cours de chargement
     * @param requestType "weather" ou "forecast"
     */
    void loadingStarted(const QString& cityName, const QString& requestType);

    /**
     * Émis en cas d'erreur (réseau, API, parsing, validation)
     *
     * @param cityName Ville concernée
     * @param errorMessage Message d'erreur lisible pour l'utilisateur
     * @param errorType Type d'erreur pour traitement spécialisé
     */
    void errorOccurred(const QString& cityName, const QString& errorMessage, const QString& errorType = "");

    // === SIGNAUX CACHE ===

    /**
     * Émis quand le cache est mis à jour
     *
     * @param cityName Ville mise en cache
     * @param dataType "weather" ou "forecast"
     */
    void cacheUpdated(const QString& cityName, const QString& dataType);

    /**
     * Émis lors du nettoyage du cache
     *
     * @param removedCount Nombre d'entrées supprimées
     */
    void cacheCleanedUp(int removedCount);

private slots:
    // Réception réponses réseau
    void onCurrentWeatherReceived();
    void onForecastReceived();
    void onNetworkError(QNetworkReply::NetworkError error);
    //void onSslErrors(const QList<QSslError>& errors);

    // Timer pour nettoyage cache automatique
    void onCacheCleanupTimer();

private:
    // === CONFIGURATION ===
    QString m_apiKey;
    QString m_baseUrl;                    // "https://api.openweathermap.org/data/2.5"
    int m_requestTimeoutMs;               // Timeout requêtes (défaut: 10s)

    // === RÉSEAU ===
    QNetworkAccessManager* m_networkManager;
    QMap<QNetworkReply*, QString> m_pendingRequests;  // Reply → cityName
    QMap<QNetworkReply*, QString> m_requestTypes;     // Reply → "weather"/"forecast"

    // === CACHE ===
    QMap<QString, CachedWeatherData> m_weatherCache;  // cityName → données+métadata
    QMap<QString, CachedForecastData> m_forecastCache;
    QTimer* m_cacheCleanupTimer;                      // Nettoyage automatique toutes les heures

    // === MÉTHODES PRIVÉES ===

    // Construction URLs API
    QUrl buildWeatherUrl(const QString& cityName) const;
    QUrl buildForecastUrl(const QString& cityName) const;

    // Gestion cache
    bool isCacheValid(const QString& cityName, const QString& dataType) const;
    void storeCachedWeather(const QString& cityName, const CurrentWeatherData& data);
    void storeCachedForecast(const QString& cityName, const ForecastData& data);

    // Parsing JSON → structures typées
    CurrentWeatherData parseCurrentWeatherJson(const QJsonObject& json) const;
    ForecastData parseForecastJson(const QJsonObject& json) const;
    ForecastEntry parseForecastEntry(const QJsonObject& entryJson) const;

    // Validation et gestion erreurs
    bool validateApiResponse(const QJsonObject& json, const QString& expectedType) const;
    QString getErrorMessage(QNetworkReply::NetworkError error) const;
    QString getApiErrorMessage(const QJsonObject& json) const;

    // Utilitaires
    void cleanupRequest(QNetworkReply* reply);
    void emitErrorSafely(const QString& cityName, const QString& message, const QString& type = "");
};

#endif // WEATHERSERVICE_H

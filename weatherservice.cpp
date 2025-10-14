#include "WeatherService.h"
#include "weathererrors.h"
#include <QDebug>
#include <QJsonValue>
#include <QNetworkRequest>
#include <QTimer>

WeatherService::WeatherService(QObject* parent)
    : QObject(parent)
    , m_baseUrl("https://api.openweathermap.org/data/2.5")
    , m_requestTimeoutMs(10000)
    , m_networkManager(nullptr)
    , m_cacheCleanupTimer(nullptr)
{
    // Initialisation du gestionnaire réseau
    m_networkManager = new QNetworkAccessManager(this);

    // Timer de nettoyage cache toutes les heures
    m_cacheCleanupTimer = new QTimer(this);
    m_cacheCleanupTimer->setInterval(60 * 60 * 1000); // 1 heure
    connect(m_cacheCleanupTimer, &QTimer::timeout, this, &WeatherService::onCacheCleanupTimer);
    m_cacheCleanupTimer->start();

    qDebug() << "WeatherService initialized";
}

WeatherService::~WeatherService()
{
    // Nettoyage des requêtes en cours
    for (auto it = m_pendingRequests.begin(); it != m_pendingRequests.end(); ++it) {
        it.key()->deleteLater();
    }
    m_pendingRequests.clear();
}

void WeatherService::setApiKey(const QString& apiKey)
{
    m_apiKey = apiKey;
    qDebug() << "API Key set";
}

bool WeatherService::isApiKeyValid() const
{
    return !m_apiKey.isEmpty() && m_apiKey.length() > 20;
}

void WeatherService::requestCurrentWeather(const QString& cityName)
{
    if (cityName.trimmed().isEmpty()) {
        emitErrorSafely(cityName, "Nom de ville vide", "validation");
        return;
    }

    if (!isApiKeyValid()) {
        emitErrorSafely(cityName, "Clé API manquante ou invalide", "configuration");
        return;
    }
    qDebug()<<"Nom de la ville333 : "<<cityName<<"\n";
    // Vérification cache d'abord
    if (cacheMgrPtr->isValid(cityName, "weather")) {
        qDebug() << "Cache hit for" << cityName;
        emit currentWeatherReady(cityName, m_weatherCache[cityName].weatherData);
        return;
    }

    // Cache manquant/expiré → appel API
    qDebug() << "Cache miss for" << cityName << "- calling API";
    emit loadingStarted(cityName, "weather");

    QUrl url = buildWeatherUrl(cityName);
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "WeatherApp/1.0");
    request.setTransferTimeout(m_requestTimeoutMs);

    QNetworkReply* reply = m_networkManager->get(request);

    // Enregistrement de la requête
    m_pendingRequests[reply] = cityName;
    m_requestTypes[reply] = "weather";

    // Connexions pour cette requête
    connect(reply, &QNetworkReply::finished, this, &WeatherService::onCurrentWeatherReceived);
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &WeatherService::onNetworkError);
}

void WeatherService::requestForecast(const QString& cityName)
{
    if (cityName.trimmed().isEmpty()) {
        emitErrorSafely(cityName, "Nom de ville vide", "validation");
        return;
    }

    if (!isApiKeyValid()) {
        emitErrorSafely(cityName, "Clé API manquante ou invalide", "configuration");
        return;
    }

    // Vérification cache forecast
    if (isCacheValid(cityName, "forecast")) {
        qDebug() << "Forecast cache hit for" << cityName;
        emit forecastReady(cityName, m_forecastCache[cityName].forecastData);
        return;
    }

    qDebug() << "Forecast cache miss for" << cityName << "- calling API";
    emit loadingStarted(cityName, "forecast");

    QUrl url = buildForecastUrl(cityName);
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "WeatherApp/1.0");
    request.setTransferTimeout(m_requestTimeoutMs);

    QNetworkReply* reply = m_networkManager->get(request);

    m_pendingRequests[reply] = cityName;
    m_requestTypes[reply] = "forecast";

    connect(reply, &QNetworkReply::finished, this, &WeatherService::onForecastReceived);
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &WeatherService::onNetworkError);
}

bool WeatherService::hasValidCache(const QString& cityName) const
{
    return isCacheValid(cityName, "weather");
}

void WeatherService::clearCache()
{
    int count = m_weatherCache.size() + m_forecastCache.size();
    m_weatherCache.clear();
    m_forecastCache.clear();
    emit cacheCleanedUp(count);
    qDebug() << "Cache cleared -" << count << "entries removed";
}

void WeatherService::onCurrentWeatherReceived()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply || !m_pendingRequests.contains(reply)) {
        return;
    }

    QString cityName = m_pendingRequests[reply];

    if (reply->error() != QNetworkReply::NoError) {
        emitErrorSafely(cityName, getErrorMessage(reply->error()), "network");
        cleanupRequest(reply);
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isNull()) {
        emitErrorSafely(cityName, "Réponse API invalide", "parsing");
        cleanupRequest(reply);
        return;
    }

    QJsonObject json = doc.object();

    // Vérification erreur API
    if (json.contains("cod") && json["cod"].toInt() != 200) {
        QString apiError = getApiErrorMessage(json);
        emitErrorSafely(cityName, apiError, "api");
        cleanupRequest(reply);
        return;
    }

    // Parsing des données
    CurrentWeatherData weatherData = parseCurrentWeatherJson(json);

    if (!weatherData.isValid()) {
        emitErrorSafely(cityName, "Données météo invalides", "validation");
        cleanupRequest(reply);
        return;
    }

    // Mise en cache et émission signal
    cacheMgrPtr->storeCachedWeather(cityName, weatherData);
    emit currentWeatherReady(cityName, weatherData);
    emit cacheUpdated(cityName, "weather");

    qDebug() << "Weather data received and cached for" << cityName;
    cleanupRequest(reply);
}

void WeatherService::onForecastReceived()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply || !m_pendingRequests.contains(reply)) {
        return;
    }

    QString cityName = m_pendingRequests[reply];

    if (reply->error() != QNetworkReply::NoError) {
        emitErrorSafely(cityName, getErrorMessage(reply->error()), "network");
        cleanupRequest(reply);
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isNull()) {
        emitErrorSafely(cityName, "Réponse API forecast invalide", "parsing");
        cleanupRequest(reply);
        return;
    }

    QJsonObject json = doc.object();

    ForecastData forecastData = parseForecastJson(json);

    if (!forecastData.isValid()) {
        emitErrorSafely(cityName, "Données prévisions invalides", "validation");
        cleanupRequest(reply);
        return;
    }

    cacheMgrPtr->storeCachedForecast(cityName, forecastData);
    emit forecastReady(cityName, forecastData);
    emit cacheUpdated(cityName, "forecast");

    qDebug() << "Forecast data received and cached for" << cityName;
    cleanupRequest(reply);
}

void WeatherService::onNetworkError(QNetworkReply::NetworkError error)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    QString cityName = m_pendingRequests.value(reply, "Unknown");
    emitErrorSafely(cityName, getErrorMessage(error), "network");
    cleanupRequest(reply);
}

QUrl WeatherService::buildWeatherUrl(const QString& cityName) const
{
    QUrl url(m_baseUrl + "/weather");
    QUrlQuery query;
    query.addQueryItem("q", cityName);
    query.addQueryItem("appid", m_apiKey);
    query.addQueryItem("units", "metric");
    query.addQueryItem("lang", "fr");
    url.setQuery(query);
    return url;
}

QUrl WeatherService::buildForecastUrl(const QString& cityName) const
{
    QUrl url(m_baseUrl + "/forecast");
    QUrlQuery query;
    query.addQueryItem("q", cityName);
    query.addQueryItem("appid", m_apiKey);
    query.addQueryItem("units", "metric");
    query.addQueryItem("lang", "fr");
    url.setQuery(query);
    return url;
}

bool WeatherService::isCacheValid(const QString& cityName, const QString& dataType) const
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

void WeatherService::storeCachedWeather(const QString& cityName, const CurrentWeatherData& data)
{
    CachedWeatherData cached;
    cached.weatherData = data;
    cached.cacheInfo.cachedAt = QDateTime::currentDateTime();
    cached.cacheInfo.validityMinutes = 15; // 15 minutes

    m_weatherCache[cityName] = cached;
}

void WeatherService::storeCachedForecast(const QString& cityName, const ForecastData& data)
{
    CachedForecastData cached;
    cached.forecastData = data;
    cached.cacheInfo.cachedAt = QDateTime::currentDateTime();
    cached.cacheInfo.validityMinutes = 120; // 2 heures

    m_forecastCache[cityName] = cached;
}

CurrentWeatherData WeatherService::parseCurrentWeatherJson(const QJsonObject& json) const
{
    CurrentWeatherData data;

    // Informations ville
    data.cityName = json["name"].toString();
    data.cityId = json["id"].toInteger();

    if (json.contains("sys")) {
        QJsonObject sys = json["sys"].toObject();
        data.countryCode = sys["country"].toString();
    }

    // Coordonnées
    if (json.contains("coord")) {
        QJsonObject coord = json["coord"].toObject();
        data.latitude = coord["lat"].toDouble();
        data.longitude = coord["lon"].toDouble();
    }

    // Données principales
    if (json.contains("main")) {
        QJsonObject main = json["main"].toObject();
        data.temperature = main["temp"].toDouble();
        data.feelsLike = main["feels_like"].toDouble();
        data.temperatureMin = main["temp_min"].toDouble();
        data.temperatureMax = main["temp_max"].toDouble();
        data.humidity = main["humidity"].toDouble();
        data.pressure = main["pressure"].toDouble();
    }

    // Conditions météo
    if (json.contains("weather") && json["weather"].isArray()) {
        QJsonArray weather = json["weather"].toArray();
        if (!weather.isEmpty()) {
            QJsonObject weatherObj = weather[0].toObject();
            data.mainCondition = weatherObj["main"].toString();
            data.description = weatherObj["description"].toString();
            data.iconCode = weatherObj["icon"].toString();
            data.conditionId = weatherObj["id"].toInt();
        }
    }

    // Vent
    if (json.contains("wind")) {
        QJsonObject wind = json["wind"].toObject();
        data.windSpeed = wind["speed"].toDouble();
        data.windDirection = wind["deg"].toInt();
    }

    // Autres
    data.visibility = json["visibility"].toDouble();
    if (json.contains("clouds")) {
        data.cloudiness = json["clouds"].toObject()["all"].toInt();
    }

    // Timestamp
    data.timestamp = QDateTime::fromSecsSinceEpoch(json["dt"].toInteger());

    return data;
}

ForecastData WeatherService::parseForecastJson(const QJsonObject& json) const
{
    ForecastData data;

    // Informations ville
    if (json.contains("city")) {
        QJsonObject city = json["city"].toObject();
        data.cityName = city["name"].toString();

        if (city.contains("coord")) {
            QJsonObject coord = city["coord"].toObject();
            data.latitude = coord["lat"].toDouble();
            data.longitude = coord["lon"].toDouble();
        }
    }

    // Parsing des entrées
    if (json.contains("list") && json["list"].isArray()) {
        QJsonArray list = json["list"].toArray();

        for (const QJsonValue& value : list) {
            QJsonObject entryJson = value.toObject();
            ForecastEntry entry = parseForecastEntry(entryJson);
            data.entries.append(entry);
        }
    }

    data.retrievedAt = QDateTime::currentDateTime();
    return data;
}

ForecastEntry WeatherService::parseForecastEntry(const QJsonObject& entryJson) const
{
    ForecastEntry entry;

    // Timestamp
    entry.dateTime = QDateTime::fromString(entryJson["dt_txt"].toString(),
                                           "yyyy-MM-dd hh:mm:ss");

    // Données principales
    if (entryJson.contains("main")) {
        QJsonObject main = entryJson["main"].toObject();
        entry.temperature = main["temp"].toDouble();
        entry.feelsLike = main["feels_like"].toDouble();
        entry.humidity = main["humidity"].toDouble();
        entry.pressure = main["pressure"].toDouble();
    }

    // Conditions météo
    if (entryJson.contains("weather") && entryJson["weather"].isArray()) {
        QJsonArray weather = entryJson["weather"].toArray();
        if (!weather.isEmpty()) {
            QJsonObject weatherObj = weather[0].toObject();
            entry.mainCondition = weatherObj["main"].toString();
            entry.description = weatherObj["description"].toString();
            entry.iconCode = weatherObj["icon"].toString();
            entry.conditionId = weatherObj["id"].toInt();
        }
    }

    // Vent
    if (entryJson.contains("wind")) {
        QJsonObject wind = entryJson["wind"].toObject();
        entry.windSpeed = wind["speed"].toDouble();
        entry.windDirection = wind["deg"].toInt();
        if (wind.contains("gust")) {
            entry.windGust = wind["gust"].toDouble();
        }
    }

    // Nuages
    if (entryJson.contains("clouds")) {
        entry.cloudiness = entryJson["clouds"].toObject()["all"].toInt();
    }

    // Probabilité précipitations
    entry.precipitationProbability = entryJson["pop"].toDouble() * 100; // 0.0-1.0 → 0-100%

    return entry;
}

QString WeatherService::getErrorMessage(QNetworkReply::NetworkError error) const
{
    switch (error) {
    case QNetworkReply::ConnectionRefusedError:
        return WeatherErrors::CONNECTION_REFUSED;
    case QNetworkReply::RemoteHostClosedError:
        return WeatherErrors::REMOTE_HOST_CLOSED;
    case QNetworkReply::HostNotFoundError:
        return WeatherErrors::HOST_NOT_FOUND;
    case QNetworkReply::TimeoutError:
        return WeatherErrors::TIMEOUT;
    case QNetworkReply::SslHandshakeFailedError:
        return WeatherErrors::SSL_HANDSHAKE;
    case QNetworkReply::AuthenticationRequiredError:
        return WeatherErrors::AUTHENTICATION;
    default:
        return WeatherErrors::NETWORK_UNKNOWN;
    }
}

QString WeatherService::getApiErrorMessage(const QJsonObject& json) const
{
    using namespace WeatherErrors;

    int cod = json["cod"].toInt();
    QString message = json["message"].toString();

    switch (cod) {
    case ApiCodes::UNAUTHORIZED:     return ApiMessages::UNAUTHORIZED;
    case ApiCodes::NOT_FOUND:        return ApiMessages::NOT_FOUND;
    case ApiCodes::TOO_MANY_REQUESTS: return ApiMessages::TOO_MANY_REQUESTS;
    case ApiCodes::INTERNAL_ERROR:   return ApiMessages::INTERNAL_ERROR;
    case ApiCodes::BAD_REQUEST:      return ApiMessages::BAD_REQUEST;
    case ApiCodes::SERVICE_UNAVAILABLE: return ApiMessages::SERVICE_UNAVAILABLE;
    default:
        return message.isEmpty() ? ApiMessages::UNKNOWN : message;
    }
}

void WeatherService::cleanupRequest(QNetworkReply* reply)
{
    if (!reply) return;

    m_pendingRequests.remove(reply);
    m_requestTypes.remove(reply);
    reply->deleteLater();
}

void WeatherService::emitErrorSafely(const QString& cityName, const QString& message, const QString& type)
{
    qWarning() << "WeatherService error for" << cityName << ":" << message;
    emit errorOccurred(cityName, message, type);
}

void WeatherService::onCacheCleanupTimer()
{
    cacheMgrPtr->cleanExpiredCache();
}



#include <QtTest>
#include <QSignalSpy>
#include "../src/weathercachemanager.h"
#include "../src/WeatherData.h"

class TestWeatherCacheManager : public QObject
{
    Q_OBJECT

private:
    weathercachemanager * m_cache;

    // Méthode utilitaire pour créer des données de test
    CurrentWeatherData createTestWeather(const QString& city, double temp = 20.0) {
        CurrentWeatherData data;
        data.cityName = city;
        data.countryCode = "FR";
        data.temperature = temp;
        data.feelsLike = temp - 1.0;
        data.humidity = 65.0;
        data.pressure = 1013.0;
        data.windSpeed = 5.5;
        data.description = "Clear sky";
        data.timestamp = QDateTime::currentDateTime();
        data.latitude = 48.8566;
        data.longitude = 2.3522;
        return data;
    }

    ForecastData createTestForecast(const QString& city) {
        ForecastData data;
        data.cityName = city;
        //data.countryCode = "FR";
        
        for (int i = 0; i < 5; ++i) {
            ForecastEntry entry;
            entry.dateTime = QDateTime::currentDateTime().addSecs(i * 10800);
            entry.temperature = 18.0 + i;
            entry.description = QString("Forecast %1").arg(i);
            entry.humidity = 60.0;
            entry.windSpeed = 4.0;
            entry.precipitationProbability = 10.0;
            data.entries.append(entry);
        }
        
        return data;
    }

private slots:
    void initTestCase() {
        qDebug() << "=== Début des tests WeatherCacheManager ===";
    }

    void init() {
        m_cache = new weathercachemanager();
    }

    void cleanup() {
        delete m_cache;
        m_cache = nullptr;
    }

    void cleanupTestCase() {
        qDebug() << "=== Fin des tests ===";
    }

    // ========================================
    // TESTS DE STOCKAGE
    // ========================================

    void testStoreAndRetrieveWeather() {
        // ARRANGE
        QString city = "Paris";
        CurrentWeatherData testData = createTestWeather(city, 22.5);

        // ACT
        m_cache->storeCachedWeather(city, testData);
        CurrentWeatherData retrieved = m_cache->getCityweatherInCache(city);

        // ASSERT
        QCOMPARE(retrieved.cityName, city);
        QCOMPARE(retrieved.temperature, 22.5);
        QCOMPARE(retrieved.countryCode, QString("FR"));
        QVERIFY(m_cache->isValid(city, "weather"));
    }

    void testStoreAndRetrieveForecast() {
        // ARRANGE
        QString city = "London";
        ForecastData testData = createTestForecast(city);

        // ACT
        m_cache->storeCachedForecast(city, testData);

        // ASSERT
        QVERIFY(m_cache->isValid(city, "forecast"));
    }

    void testStoreMultipleCities() {
        // ARRANGE
        QStringList cities = {"Paris", "London", "Tokyo"};

        // ACT
        for (const QString& city : cities) {
            m_cache->storeCachedWeather(city, createTestWeather(city));
        }

        // ASSERT
        for (const QString& city : cities) {
            QVERIFY2(m_cache->isValid(city, "weather"), 
                     qPrintable(QString("Cache invalide pour %1").arg(city)));
        }
    }

    void testOverwriteCache() {
        // ARRANGE
        QString city = "Paris";
        m_cache->storeCachedWeather(city, createTestWeather(city, 15.0));
        
        // ACT
        m_cache->storeCachedWeather(city, createTestWeather(city, 25.0));
        CurrentWeatherData retrieved = m_cache->getCityweatherInCache(city);

        // ASSERT
        QCOMPARE(retrieved.temperature, 25.0);
    }

    // ========================================
    // TESTS DE VALIDATION
    // ========================================

    void testIsValid_ValidCache() {
        // ARRANGE
        m_cache->storeCachedWeather("Paris", createTestWeather("Paris"));

        // ACT & ASSERT
        QVERIFY(m_cache->isValid("Paris", "weather"));
    }

    void testIsValid_NonExistent() {
        // ACT & ASSERT
        QVERIFY(!m_cache->isValid("NonExistentCity", "weather"));
    }

    void testIsValid_WrongDataType() {
        // ARRANGE
        m_cache->storeCachedWeather("Paris", createTestWeather("Paris"));

        // ACT & ASSERT
        QVERIFY(m_cache->isValid("Paris", "weather"));
        QVERIFY(!m_cache->isValid("Paris", "forecast"));
    }

    // ========================================
    // TESTS DE NETTOYAGE
    // ========================================

    void testClearCache() {
        // ARRANGE
        m_cache->storeCachedWeather("Paris", createTestWeather("Paris"));
        m_cache->storeCachedWeather("London", createTestWeather("London"));

        // ACT
        m_cache->signalCacheCleared();

        // ASSERT
        QVERIFY(!m_cache->isValid("Paris", "weather"));
        QVERIFY(!m_cache->isValid("London", "weather"));
    }

    void testClearCacheSignal() {
        // ARRANGE
        QSignalSpy spy(m_cache, &weathercachemanager::cacheCleanedUp);
        QVERIFY(spy.isValid());
        
        m_cache->storeCachedWeather("Paris", createTestWeather("Paris"));

        // ACT
        m_cache->signalCacheCleared();

        // ASSERT
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toInt(), 1); // 1 entrée supprimée
    }

    void testCleanExpiredCache() {
        // ARRANGE
        m_cache->storeCachedWeather("Paris", createTestWeather("Paris"));
        
        // ACT
        int removed = m_cache->cleanExpiredCache();

        // ASSERT
        QCOMPARE(removed, 0); // Rien d'expiré
        QVERIFY(m_cache->isValid("Paris", "weather"));
    }

    // ========================================
    // TESTS DE CAS LIMITES
    // ========================================

    void testEmptyCityName() {
        // ARRANGE
        CurrentWeatherData data = createTestWeather("");

        // ACT
        m_cache->storeCachedWeather("", data);

        // ASSERT
        QVERIFY(m_cache->isValid("", "weather"));
    }

    void testSpecialCharacters() {
        // ARRANGE
        QString specialCity = "Saint-Étienne";

        // ACT
        m_cache->storeCachedWeather(specialCity, createTestWeather(specialCity));

        // ASSERT
        QVERIFY(m_cache->isValid(specialCity, "weather"));
    }
};

QTEST_APPLESS_MAIN(TestWeatherCacheManager)
#include "tst_weathercachemanager.moc"

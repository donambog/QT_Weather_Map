#include "configloader.h"
#include "MainWindow.h"
#include "weatherchartwidget.h"
#include <QApplication>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_weatherService(nullptr)
    , m_isLoading(false)
{
    setupUI();
    setupStatusBar();

    // Initialisation du service météo
    m_weatherService = new WeatherService(this);

    // Set the API key
    //m_weatherService->setApiKey("d9e4708f4f732c05626ac37291b86059");
    // Charge le cle par json
    ConfigLoader config;
    if (config.loadConfig()) {
        // Configuration OK
        m_weatherService->setApiKey(config.getApiKey());
        m_logDisplay->append("Clé API chargée depuis config.json");
    } else {
        //Erreur de configuration
        m_logDisplay->append("Erreur de configuration:");
        m_logDisplay->append(config.getErrorMessage());

        QMessageBox::critical(this, "Configuration", config.getErrorMessage());
    }

    setupConnections();

    setWindowTitle("Application Météo - Test Architecture");
    resize(800, 600);

    // Message d'accueil
    m_logDisplay->append("=== Application Météo Démarrée ===");
    m_logDisplay->append("Entrez un nom de ville et cliquez Rechercher");
    m_logDisplay->append("Architecture: WeatherService + Cache + Signals/Slots");
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    m_mainLayout = new QVBoxLayout(m_centralWidget);

    // === SECTION RECHERCHE ===
    m_searchGroup = new QGroupBox("Recherche Ville", this);
    m_searchLayout = new QHBoxLayout(m_searchGroup);

    m_cityInput = new QLineEdit(this);
    m_cityInput->setPlaceholderText("Entrez un nom de ville (ex: Paris, London, Tokyo)");
    m_cityInput->setMinimumWidth(300);

    m_searchButton = new QPushButton("Rechercher Météo", this);
    m_searchButton->setMinimumWidth(150);

    m_clearCacheButton = new QPushButton("Vider Cache", this);

    m_searchLayout->addWidget(m_cityInput);
    m_searchLayout->addWidget(m_searchButton);
    m_searchLayout->addWidget(m_clearCacheButton);
    m_mainLayout->addWidget(m_searchGroup);

    // === SECTION MÉTÉO ACTUELLE ===
    m_currentWeatherGroup = new QGroupBox("Météo Actuelle", this);
    m_currentWeatherLayout = new QGridLayout(m_currentWeatherGroup);

    // Labels pour affichage
    m_cityNameLabel = new QLabel("Ville: -", this);
    m_cityNameLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

    m_temperatureLabel = new QLabel("Température: -", this);
    m_temperatureLabel->setStyleSheet("font-size: 16px; color: #2196F3;");

    m_descriptionLabel = new QLabel("Conditions: -", this);
    m_feelsLikeLabel = new QLabel("Ressenti: -", this);
    m_humidityLabel = new QLabel("Humidité: -", this);
    m_pressureLabel = new QLabel("Pression: -", this);
    m_windLabel = new QLabel("Vent: -", this);
    m_timestampLabel = new QLabel("Dernière mise à jour: -", this);
    m_timestampLabel->setStyleSheet("font-size: 10px; color: gray;");

    // Organisation en grille
    m_currentWeatherLayout->addWidget(m_cityNameLabel, 0, 0, 1, 2);
    m_currentWeatherLayout->addWidget(m_temperatureLabel, 1, 0);
    m_currentWeatherLayout->addWidget(m_feelsLikeLabel, 1, 1);
    m_currentWeatherLayout->addWidget(m_descriptionLabel, 2, 0);
    m_currentWeatherLayout->addWidget(m_humidityLabel, 2, 1);
    m_currentWeatherLayout->addWidget(m_pressureLabel, 3, 0);
    m_currentWeatherLayout->addWidget(m_windLabel, 3, 1);
    m_currentWeatherLayout->addWidget(m_timestampLabel, 4, 0, 1, 2);

    m_mainLayout->addWidget(m_currentWeatherGroup);

    // === SECTION PRÉVISIONS ===
    m_forecastGroup = new QGroupBox("Prévisions 5 Jours (Résumé)", this);
    m_forecastLayout = new QVBoxLayout(m_forecastGroup);

    m_forecastDisplay = new QTextEdit(this);
    m_forecastDisplay->setMaximumHeight(150);
    m_forecastDisplay->setPlaceholderText("Les prévisions apparaîtront ici...");
    m_forecastLayout->addWidget(m_forecastDisplay);

    m_mainLayout->addWidget(m_forecastGroup);
    // === SECTION GRAPHIQUE ===
    m_chartGroup = new QGroupBox("Graphique Évolution 5 Jours", this);
    m_chartLayout = new QVBoxLayout(m_chartGroup);

    m_chartWidget = new WeatherChartWidget(this);
    m_chartLayout->addWidget(m_chartWidget);

    m_mainLayout->addWidget(m_chartGroup);
    // === SECTION ÉTAT/DEBUG ===
    m_statusGroup = new QGroupBox("État & Logs", this);
    m_statusLayout = new QVBoxLayout(m_statusGroup);

    m_loadingBar = new QProgressBar(this);
    m_loadingBar->setRange(0, 0); // Animation infinie
    m_loadingBar->setVisible(false);

    m_logDisplay = new QTextEdit(this);
    m_logDisplay->setMaximumHeight(120);
    m_logDisplay->setStyleSheet("font-family: monospace; font-size: 9px;");

    m_statusLayout->addWidget(m_loadingBar);
    m_statusLayout->addWidget(m_logDisplay);
    m_mainLayout->addWidget(m_statusGroup);
}

void MainWindow::setupConnections()
{
    // === CONNEXIONS UI → ACTIONS ===
    connect(m_searchButton, &QPushButton::clicked,
            this, &MainWindow::onSearchButtonClicked);

    connect(m_clearCacheButton, &QPushButton::clicked,
            this, &MainWindow::onClearCacheClicked);

    connect(m_cityInput, &QLineEdit::returnPressed,
            this, &MainWindow::onCityInputReturnPressed);

    // === CONNEXIONS WEATHERSERVICE → UI ===
    connect(m_weatherService, &WeatherService::currentWeatherReady,
            this, &MainWindow::onCurrentWeatherReady);

    connect(m_weatherService, &WeatherService::forecastReady,
            this, &MainWindow::onForecastReady);

    connect(m_weatherService, &WeatherService::loadingStarted,
            this, &MainWindow::onLoadingStarted);

    connect(m_weatherService, &WeatherService::errorOccurred,
            this, &MainWindow::onErrorOccurred);

    connect(m_weatherService, &WeatherService::cacheUpdated,
            this, &MainWindow::onCacheUpdated);

    connect(m_weatherService, &WeatherService::forecastReady,
            m_chartWidget, &WeatherChartWidget::onForecastDataReceived);
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("Prêt - Entrez une ville pour commencer");
}

void MainWindow::onSearchButtonClicked()
{
    QString city = m_cityInput->text().trimmed();

    if (city.isEmpty()) {
        QMessageBox::warning(this, "Attention", "Veuillez entrer un nom de ville");
        return;
    }

    m_logDisplay->append(QString("=== Recherche pour: %1 ===").arg(city));

    // Demander météo actuelle ET prévisions
    m_weatherService->requestCurrentWeather(city);
    m_weatherService->requestForecast(city);

    m_currentCity = city;
}

void MainWindow::onClearCacheClicked()
{
    m_weatherService->clearCache();
    m_logDisplay->append("Cache vidé manuellement");
}

void MainWindow::onCityInputReturnPressed()
{
    onSearchButtonClicked();
}

void MainWindow::onCurrentWeatherReady(const QString& cityName, const CurrentWeatherData& data)
{
    m_logDisplay->append(QString("✓ Météo actuelle reçue pour %1").arg(cityName));
    displayCurrentWeather(data);

    // Cacher loading si visible
    if (m_isLoading) {
        m_loadingBar->setVisible(false);
        m_isLoading = false;
    }

    statusBar()->showMessage(QString("Météo de %1 mise à jour").arg(cityName), 5000);
}

void MainWindow::onForecastReady(const QString& cityName, const ForecastData& data)
{
    m_logDisplay->append(QString("✓ Prévisions reçues pour %1 (%2 créneaux)")
                             .arg(cityName)
                             .arg(data.entries.size()));

    displayForecastSummary(data);
}

void MainWindow::onLoadingStarted(const QString& cityName, const QString& requestType)
{
    m_logDisplay->append(QString("⏳ Chargement %1 pour %2...")
                             .arg(requestType == "weather" ? "météo" : "prévisions")
                             .arg(cityName));

    if (!m_isLoading) {
        m_loadingBar->setVisible(true);
        m_isLoading = true;
    }

    statusBar()->showMessage(QString("Chargement %1...").arg(cityName));
}

void MainWindow::onErrorOccurred(const QString& cityName, const QString& errorMessage, const QString& errorType)
{
    m_logDisplay->append(QString("Erreur pour %1: %2 (type: %3)")
                             .arg(cityName)
                             .arg(errorMessage)
                             .arg(errorType));

    showErrorState(errorMessage);

    // Cacher loading
    if (m_isLoading) {
        m_loadingBar->setVisible(false);
        m_isLoading = false;
    }

    statusBar()->showMessage(QString("Erreur: %1").arg(errorMessage), 10000);
}

void MainWindow::onCacheUpdated(const QString& cityName, const QString& dataType)
{
    m_logDisplay->append(QString("💾 Cache mis à jour: %1 (%2)")
                             .arg(cityName)
                             .arg(dataType));
}

void MainWindow::displayCurrentWeather(const CurrentWeatherData& data)
{
    m_cityNameLabel->setText(QString("Ville: %1, %2")
                                 .arg(data.cityName)
                                 .arg(data.countryCode));

    m_temperatureLabel->setText(formatTemperature(data.temperature));

    m_descriptionLabel->setText(QString("Conditions: %1").arg(data.description));

    m_feelsLikeLabel->setText(QString("Ressenti: %1").arg(formatTemperature(data.feelsLike)));

    m_humidityLabel->setText(QString("Humidité: %1%").arg(data.humidity, 0, 'f', 0));

    m_pressureLabel->setText(QString("Pression: %1 hPa").arg(data.pressure, 0, 'f', 0));

    m_windLabel->setText(formatWindSpeed(data.windSpeed));

    m_timestampLabel->setText(QString("Mis à jour: %1").arg(formatTime(data.timestamp)));

    // Style selon température
    QString tempColor = data.temperature >= 25 ? "#FF5722" :
                            data.temperature >= 15 ? "#FF9800" :
                            data.temperature >= 5 ? "#2196F3" : "#9C27B0";

    m_temperatureLabel->setStyleSheet(QString("font-size: 18px; font-weight: bold; color: %1;")
                                          .arg(tempColor));
}

void MainWindow::displayForecastSummary(const ForecastData& data)
{
    m_forecastDisplay->clear();

    // Générer résumés quotidiens
    auto dailySummaries = data.getDailySummaries();

    m_forecastDisplay->append(QString("=== Prévisions pour %1 ===\n").arg(data.cityName));

    for (int i = 0; i < dailySummaries.size(); ++i) {
        const auto& summary = dailySummaries[i];

        QString dayName;
        if (i == 0) dayName = "Aujourd'hui";
        else if (i == 1) dayName = "Demain";
        else dayName = summary.date.toString("dddd dd/MM");

        m_forecastDisplay->append(
            QString("%1: %2 à %3 - %4")
                .arg(dayName)
                .arg(formatTemperature(summary.minTemp))
                .arg(formatTemperature(summary.maxTemp))
                .arg(summary.dominantCondition)
            );
    }

    // Afficher quelques créneaux détaillés du premier jour
    m_forecastDisplay->append(QString("\n=== Détail Aujourd'hui ==="));
    auto todayEntries = data.getEntriesForDay(0);

    for (int i = 0; i < qMin(6, todayEntries.size()); ++i) {
        const auto& entry = todayEntries[i];
        m_forecastDisplay->append(
            QString("%1: %2 - %3 (pluie: %4%)")
                .arg(entry.dateTime.toString("hh:mm"))
                .arg(formatTemperature(entry.temperature))
                .arg(entry.description)
                .arg(entry.precipitationProbability, 0, 'f', 0)
            );
    }
}

void MainWindow::showErrorState(const QString& message)
{
    // Afficher erreur dans interface
    m_cityNameLabel->setText("Erreur");
    m_temperatureLabel->setText("--°C");
    m_temperatureLabel->setStyleSheet("font-size: 16px; color: red;");
    m_descriptionLabel->setText(QString("Erreur: %1").arg(message));

    // Réinitialiser autres champs
    m_feelsLikeLabel->setText("Ressenti: -");
    m_humidityLabel->setText("Humidité: -");
    m_pressureLabel->setText("Pression: -");
    m_windLabel->setText("Vent: -");
    m_timestampLabel->setText("-");

    m_forecastDisplay->setText("Prévisions indisponibles suite à l'erreur");
}

QString MainWindow::formatTemperature(double temp) const
{
    return QString("%1°C").arg(temp, 0, 'f', 1);
}

QString MainWindow::formatWindSpeed(double speed) const
{
    double kmh = speed * 3.6; // m/s → km/h
    return QString("Vent: %1 km/h").arg(kmh, 0, 'f', 1);
}

QString MainWindow::formatTime(const QDateTime& dateTime) const
{
    return dateTime.toString("dd/MM/yyyy hh:mm");
}

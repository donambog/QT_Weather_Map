#include "configloader.h"
#include "MainWindow.h"
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
    // Charge la cle par json
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

    setWindowTitle("Application Météo ");
    resize(1200, 900);

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

    // Layout principal horizontal qu on va diviser en 2
    QHBoxLayout* mainHorizontalLayout = new QHBoxLayout(m_centralWidget);

    // === PARTIE GAUCHE ===
    QWidget* leftWidget = new QWidget();
    m_mainLeftLayout = new QVBoxLayout(leftWidget);
    setupLeftPanel();

    // === PARTIE DROITE ===
    QWidget* rightWidget = new QWidget();
    m_mainRightLayout = new QVBoxLayout(rightWidget);
    setupRightPanel();

    // === ASSEMBLAGE FINAL ===
    mainHorizontalLayout->addWidget(leftWidget, 2);  // 2/3 de la largeur
    mainHorizontalLayout->addWidget(rightWidget, 1); // 1/3 de la largeur
}

void MainWindow::setupLeftPanel()
{
    setupSearchSection();
    setupCurrentWeatherSection();
    setupForecastSection();
    setupChartSection();
    setupStatusSection();
}

void MainWindow::setupRightPanel()
{
    setupHistorySection();
    setupMapSection();
    // Espaceur pour centrer le contenu
    //m_mainRightLayout->addStretch();
}

void MainWindow::setupMapSection()
{
    // Groupe principal pour la carte
    QGroupBox* mapGroup = new QGroupBox("Carte Météo", this);
    QVBoxLayout* mapGroupLayout = new QVBoxLayout(mapGroup);

    // Placeholder pour la carte
    QLabel* mapPlaceholder = new QLabel("Zone Carte\nLa carte sera intégrée ici", this);
    mapPlaceholder->setAlignment(Qt::AlignCenter);
    mapPlaceholder->setMinimumHeight(250);
    mapPlaceholder->setStyleSheet(
        "border: 2px dashed #95a5a6;"
        "color: #7f8c8d;"
        "font-size: 14px;"
        "padding: 40px;"
        "border-radius: 8px;"
        "background: #ecf0f1;"
        );

    mapGroupLayout->addWidget(mapPlaceholder);
    m_mainRightLayout->addWidget(mapGroup,2);
}

void MainWindow::setupHistorySection()
{
    // Groupe principal pour l'historique
    QGroupBox* historyGroup = new QGroupBox("Historique des recherches", this);
    QVBoxLayout* historyGroupLayout = new QVBoxLayout(historyGroup);

    // Widget à onglets
    m_historyTabWidget = new QTabWidget(this);

    // === ONGLET RÉCENT ===
    QWidget* recentTab = new QWidget();
    QVBoxLayout* recentLayout = new QVBoxLayout(recentTab);

    // Liste des recherches récentes
    m_recentSearchList = new QListWidget(this);
    m_recentSearchList->setMaximumHeight(200);
    m_recentSearchList->setStyleSheet(
        "QListWidget { border: 1px solid #ccc; border-radius: 4px; }"
        "QListWidget::item { padding: 8px; border-bottom: 1px solid #eee; }"
        "QListWidget::item:hover { background-color: #f0f8ff; }"
        "QListWidget::item:selected { background-color: #3498db; color: white; }"
        );

    // Label statistiques récent
    m_recentStatsLabel = new QLabel("Aucune recherche récente", this);
    m_recentStatsLabel->setStyleSheet("color: #666; font-size: 11px; margin-top: 5px;");
    m_recentStatsLabel->setAlignment(Qt::AlignCenter);

    recentLayout->addWidget(m_recentSearchList);
    recentLayout->addWidget(m_recentStatsLabel);

    // === ONGLET FAVORIS ===
    QWidget* favoritesTab = new QWidget();
    QVBoxLayout* favoritesLayout = new QVBoxLayout(favoritesTab);

    // Liste des favoris
    m_favoritesSearchList = new QListWidget(this);
    m_favoritesSearchList->setMaximumHeight(150);
    m_favoritesSearchList->setStyleSheet(
        "QListWidget { border: 1px solid #ccc; border-radius: 4px; }"
        "QListWidget::item { padding: 8px; border-bottom: 1px solid #eee; }"
        "QListWidget::item:hover { background-color: #fff3cd; }"
        "QListWidget::item:selected { background-color: #f39c12; color: white; }"
        );

    // Boutons de gestion des favoris
    QHBoxLayout* favoritesButtonLayout = new QHBoxLayout();

    m_addFavoriteButton = new QPushButton("⭐ Ajouter aux favoris", this);
    m_addFavoriteButton->setStyleSheet(
        "QPushButton { background-color: #f39c12; color: white; border: none; padding: 6px 12px; border-radius: 4px; }"
        "QPushButton:hover { background-color: #e67e22; }"
        "QPushButton:disabled { background-color: #bdc3c7; }"
        );
    m_addFavoriteButton->setEnabled(false); // Désactivé par défaut

    m_removeFavoriteButton = new QPushButton("🗑️ Supprimer", this);
    m_removeFavoriteButton->setStyleSheet(
        "QPushButton { background-color: #e74c3c; color: white; border: none; padding: 6px 12px; border-radius: 4px; }"
        "QPushButton:hover { background-color: #c0392b; }"
        "QPushButton:disabled { background-color: #bdc3c7; }"
        );
    m_removeFavoriteButton->setEnabled(false);

    favoritesButtonLayout->addWidget(m_addFavoriteButton);
    favoritesButtonLayout->addWidget(m_removeFavoriteButton);
    favoritesButtonLayout->addStretch();

    // Label statistiques favoris
    m_favoritesStatsLabel = new QLabel("Aucun favori", this);
    m_favoritesStatsLabel->setStyleSheet("color: #666; font-size: 11px; margin-top: 5px;");
    m_favoritesStatsLabel->setAlignment(Qt::AlignCenter);

    favoritesLayout->addWidget(m_favoritesSearchList);
    favoritesLayout->addLayout(favoritesButtonLayout);
    favoritesLayout->addWidget(m_favoritesStatsLabel);

    // === AJOUT DES ONGLETS ===
    m_historyTabWidget->addTab(recentTab, "Récent");
    m_historyTabWidget->addTab(favoritesTab, "⭐ Favoris");
    m_historyTabWidget->setCurrentIndex(0); // Onglet récent par défaut

    // === CONTRÔLES GÉNÉRAUX ===
    QHBoxLayout* controlsLayout = new QHBoxLayout();

    m_clearHistoryButton = new QPushButton("Vider historique", this);
    m_clearHistoryButton->setStyleSheet(
        "QPushButton { background-color: #95a5a6; color: white; border: none; padding: 6px 12px; border-radius: 4px; font-size: 10px; }"
        "QPushButton:hover { background-color: #7f8c8d; }"
        );

    controlsLayout->addStretch();
    controlsLayout->addWidget(m_clearHistoryButton);

    // === ASSEMBLAGE FINAL ===
    historyGroupLayout->addWidget(m_historyTabWidget);
    historyGroupLayout->addLayout(controlsLayout);

    m_mainRightLayout->addWidget(historyGroup,1);

    // === CONNEXIONS DES SIGNAUX ===
    //setupHistoryConnections();

    // === INITIALISATION DES DONNÉES ===
    //refreshHistoryDisplay();
}
void MainWindow::setupSearchSection()
{
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

    m_mainLeftLayout->addWidget(m_searchGroup);
}

void MainWindow::setupCurrentWeatherSection()
{
    m_currentWeatherGroup = new QGroupBox("Météo Actuelle", this);
    m_currentWeatherLayout = new QGridLayout(m_currentWeatherGroup);

    // Création des labels
    createWeatherLabels();

    // Organisation en grille
    arrangeWeatherLabels();

    m_mainLeftLayout->addWidget(m_currentWeatherGroup);
}

void MainWindow::createWeatherLabels()
{
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
}

void MainWindow::arrangeWeatherLabels()
{
    m_currentWeatherLayout->addWidget(m_cityNameLabel, 0, 0, 1, 2);
    m_currentWeatherLayout->addWidget(m_temperatureLabel, 1, 0);
    m_currentWeatherLayout->addWidget(m_feelsLikeLabel, 1, 1);
    m_currentWeatherLayout->addWidget(m_descriptionLabel, 2, 0);
    m_currentWeatherLayout->addWidget(m_humidityLabel, 2, 1);
    m_currentWeatherLayout->addWidget(m_pressureLabel, 3, 0);
    m_currentWeatherLayout->addWidget(m_windLabel, 3, 1);
    m_currentWeatherLayout->addWidget(m_timestampLabel, 4, 0, 1, 2);
}

void MainWindow::setupForecastSection()
{
    m_forecastGroup = new QGroupBox("Prévisions 5 Jours (Résumé)", this);
    m_forecastLayout = new QVBoxLayout(m_forecastGroup);

    m_forecastDisplay = new QTextEdit(this);
    m_forecastDisplay->setMaximumHeight(150);
    m_forecastDisplay->setPlaceholderText("Les prévisions apparaîtront ici...");

    m_forecastLayout->addWidget(m_forecastDisplay);
    m_mainLeftLayout->addWidget(m_forecastGroup);
}

void MainWindow::setupChartSection()
{
    m_chartGroup = new QGroupBox("Graphique Évolution 5 Jours et Maps", this);
    m_chartLayout = new QHBoxLayout(m_chartGroup);

    // Widget graphique
    m_chartWidget = new WeatherChartWidget(this);
    m_chartLayout->addWidget(m_chartWidget);

    m_mainLeftLayout->addWidget(m_chartGroup);
}

void MainWindow::setupStatusSection()
{
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
    m_mainLeftLayout->addWidget(m_statusGroup);
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

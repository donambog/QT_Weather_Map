#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QTabWidget>
#include <QListWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QGroupBox>
#include <QGridLayout>
#include <QStatusBar>
#include "WeatherService.h"
#include "WeatherChartWidget.h"
#include "simplemapwidget.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Interactions utilisateur
    void onSearchButtonClicked();
    void onClearCacheClicked();
    void onCityInputReturnPressed();

    // Réception des données WeatherService
    void onCurrentWeatherReady(const QString& cityName, const CurrentWeatherData& data);
    void onForecastReady(const QString& cityName, const ForecastData& data);
    void onLoadingStarted(const QString& cityName, const QString& requestType);
    void onErrorOccurred(const QString& cityName, const QString& errorMessage, const QString& errorType);
    void onCacheUpdated(const QString& cityName, const QString& dataType);

private:
    // Interface utilisateur
    void setupUI();
    void setupConnections();
    void setupStatusBar();

    // Affichage des données
    void displayCurrentWeather(const CurrentWeatherData& data);
    void displayForecastSummary(const ForecastData& data);
    void showLoadingState(const QString& cityName, const QString& requestType);
    void showErrorState(const QString& message);
    void resetDisplay();

    // Utilitaires UI
    QString formatTemperature(double temp) const;
    QString formatWindSpeed(double speed) const;
    QString formatTime(const QDateTime& dateTime) const;

    // Méthodes de configuration UI
    void setupLeftPanel();
    void setupRightPanel();
    void setupSearchSection();
    void setupCurrentWeatherSection();
    void setupForecastSection();
    void setupChartSection();
    void setupStatusSection();
    void setupHistorySection();
    void setupMapSection();

    // Méthodes utilitaires UI
    void createWeatherLabels();
    void arrangeWeatherLabels();

    // Widgets principaux
    QWidget* m_centralWidget;
    QVBoxLayout* m_mainLeftLayout;
    QVBoxLayout* m_mainRightLayout;

    // Section recherche
    QGroupBox* m_searchGroup;
    QHBoxLayout* m_searchLayout;
    QLineEdit* m_cityInput;
    QPushButton* m_clearCacheButton, * m_searchButton, *m_clearHistoryButton;

    // Section météo actuelle
    QGroupBox* m_currentWeatherGroup;
    QGridLayout* m_currentWeatherLayout;
    QLabel* m_cityNameLabel;
    QLabel* m_temperatureLabel;
    QLabel* m_descriptionLabel;
    QLabel* m_feelsLikeLabel;
    QLabel* m_humidityLabel;
    QLabel* m_pressureLabel;
    QLabel* m_windLabel;
    QLabel* m_timestampLabel;

    // Section prévisions résumé
    QGroupBox* m_forecastGroup;
    QVBoxLayout* m_forecastLayout;
    QTextEdit* m_forecastDisplay;

    //Section
    // === SECTION GRAPHIQUE ===
    QGroupBox* m_chartGroup;
    QHBoxLayout* m_chartLayout;

    // section Favoris
    QTabWidget* m_historyTabWidget;
    QListWidget* m_recentSearchList, *m_favoritesSearchList;
    QPushButton* m_addFavoriteButton, *m_removeFavoriteButton;
    QLabel* m_recentStatsLabel, *m_favoritesStatsLabel;


    // Section état/debug
    QGroupBox* m_statusGroup;
    QVBoxLayout* m_statusLayout;
    QProgressBar* m_loadingBar;
    QTextEdit* m_logDisplay;

    // Service météo
    WeatherService* m_weatherService;
    // chart
    WeatherChartWidget* m_chartWidget;
    //SimpleMapWidget* m_mapWidget;


    // État
    QString m_currentCity;
    bool m_isLoading;
};

#endif // MAINWINDOW_H

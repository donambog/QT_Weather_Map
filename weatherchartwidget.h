#ifndef WEATHERCHARTWIDGET_H
#define WEATHERCHARTWIDGET_H

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLegend>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>
#include "WeatherData.h"

QT_CHARTS_USE_NAMESPACE

    /**
 * Widget affichant un graphique de l'évolution de la température
 * et de l'humidité sur 5 jours
 *
 * Fonctionnalités :
 * - Double courbe : température (°C) et humidité (%)
 * - Axes séparés pour les différentes unités
 * - Timeline sur 5 jours avec les 40 créneaux
 * - Légende et couleurs différenciées
 * - Responsive et intégré dans l'interface
 */
    class WeatherChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WeatherChartWidget(QWidget* parent = nullptr);
    ~WeatherChartWidget();

    // Affichage des données
    void displayForecastData(const ForecastData& forecastData);
    void clearChart();

    // Configuration
    void setChartTitle(const QString& title);
    void setTemperatureColor(const QColor& color);
    void setHumidityColor(const QColor& color);

public slots:
    void onForecastDataReceived(const QString& cityName, const ForecastData& data);

private:
    // Interface
    QVBoxLayout* m_layout;
    QLabel* m_titleLabel;
    QChartView* m_chartView;
    QChart* m_chart;

    // Séries de données
    QSplineSeries* m_temperatureSeries;
    QSplineSeries* m_humiditySeries;

    // Axes
    QDateTimeAxis* m_axisX;          // Axe temps commun
    QValueAxis* m_axisTemperature;   // Axe température (gauche)
    QValueAxis* m_axisHumidity;      // Axe humidité (droite)

    // Configuration
    QString m_cityName;
    QColor m_temperatureColor;
    QColor m_humidityColor;

    // Méthodes privées
    void setupChart();
    void setupAxes();
    void setupSeries();
    void configureAppearance();
    void updateAxisRanges(const ForecastData& data);

    // Utilitaires données
    QPair<double, double> getTemperatureRange(const ForecastData& data) const;
    QPair<double, double> getHumidityRange(const ForecastData& data) const;
    QPair<QDateTime, QDateTime> getTimeRange(const ForecastData& data) const;

    // Formatage
    QString formatTooltip(const QDateTime& time, double temp, double humidity) const;
};

#endif // WEATHERCHARTWIDGET_H

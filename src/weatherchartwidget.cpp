#include "WeatherChartWidget.h"
#include <QDebug>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>

WeatherChartWidget::WeatherChartWidget(QWidget* parent)
    : QWidget(parent)
    , m_layout(nullptr)
    , m_titleLabel(nullptr)
    , m_chartView(nullptr)
    , m_chart(nullptr)
    , m_temperatureSeries(nullptr)
    , m_humiditySeries(nullptr)
    , m_axisX(nullptr)
    , m_axisTemperature(nullptr)
    , m_axisHumidity(nullptr)
    , m_temperatureColor(Qt::red)
    , m_humidityColor(Qt::blue)
{
    setupChart();
    qDebug() << "WeatherChartWidget initialized";
}

WeatherChartWidget::~WeatherChartWidget()
{
    // Les objets Qt sont détruits automatiquement par le parent
}

void WeatherChartWidget::setupChart()
{
    // Layout principal
    m_layout = new QVBoxLayout(this);

    // Titre
    m_titleLabel = new QLabel("Évolution Température et Humidité - 5 jours", this);
    m_titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #333;");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(m_titleLabel);

    // Créer le graphique
    m_chart = new QChart();
    m_chart->setTitle("Prévisions Météorologiques");
    m_chart->setAnimationOptions(QChart::AllAnimations);

    // Vue du graphique
    m_chartView = new QChartView(m_chart, this);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setMinimumHeight(300);

    m_layout->addWidget(m_chartView);

    // Configuration initiale
    setupSeries();
    setupAxes();
    configureAppearance();
}

void WeatherChartWidget::setupSeries()
{
    // Série température (courbe lisse rouge)
    m_temperatureSeries = new QSplineSeries();
    m_temperatureSeries->setName("Température (°C)");
    m_temperatureSeries->setColor(m_temperatureColor);
    // Qt 6 : utiliser QPen pour définir l'épaisseur
    QPen tempPen(m_temperatureColor);
    tempPen.setWidth(3);
    m_temperatureSeries->setPen(tempPen);

    // Série humidité (courbe lisse bleue)
    m_humiditySeries = new QSplineSeries();
    m_humiditySeries->setName("Humidité (%)");
    m_humiditySeries->setColor(m_humidityColor);
    // Qt 6 : utiliser QPen pour définir l'épaisseur
    QPen humidityPen(m_humidityColor);
    humidityPen.setWidth(3);
    m_humiditySeries->setPen(humidityPen);

    // Ajouter les séries au graphique
    m_chart->addSeries(m_temperatureSeries);
    m_chart->addSeries(m_humiditySeries);
}

void WeatherChartWidget::setupAxes()
{
    // Axe X : Temps (commun aux deux courbes)
    m_axisX = new QDateTimeAxis();
    m_axisX->setTitleText("Temps");
    m_axisX->setFormat("dd/MM hh:mm");
    m_axisX->setLabelsAngle(-45); // Labels inclinés pour lisibilité
    m_axisX->setTickCount(5);     // 6 points de temps visibles

    // Axe Y gauche : Température
    m_axisTemperature = new QValueAxis();
    m_axisTemperature->setTitleText("Température (°C)");
    m_axisTemperature->setLabelFormat("%.1f°C");
    m_axisTemperature->setLabelsColor(m_temperatureColor);

    // Axe Y droite : Humidité
    m_axisHumidity = new QValueAxis();
    m_axisHumidity->setTitleText("Humidité (%)");
    m_axisHumidity->setLabelFormat("%.0f%%");
    m_axisHumidity->setLabelsColor(m_humidityColor);

    // Associer les axes aux séries
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisTemperature, Qt::AlignLeft);
    m_chart->addAxis(m_axisHumidity, Qt::AlignRight);

    // Lier séries et axes
    m_temperatureSeries->attachAxis(m_axisX);
    m_temperatureSeries->attachAxis(m_axisTemperature);

    m_humiditySeries->attachAxis(m_axisX);
    m_humiditySeries->attachAxis(m_axisHumidity);
}

void WeatherChartWidget::configureAppearance()
{
    // Style du graphique
    m_chart->setBackgroundBrush(QBrush(QColor(250, 250, 250)));
    m_chart->setTitleFont(QFont("Arial", 12, QFont::Bold));

    // Légende
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignTop);
    m_chart->legend()->setBackgroundVisible(true);

    // Grille
    m_axisX->setGridLineVisible(true);
    m_axisTemperature->setGridLineVisible(true);
    m_axisHumidity->setGridLineVisible(false); // Éviter surcharge visuelle

    // Marges
    m_chart->setMargins(QMargins(10, 10, 10, 10));
}

void WeatherChartWidget::displayForecastData(const ForecastData& forecastData)
{
    if (!forecastData.isValid() || forecastData.entries.isEmpty()) {
        qWarning() << "Invalid forecast data for chart display";
        return;
    }

    // Vider les séries existantes
    m_temperatureSeries->clear();
    m_humiditySeries->clear();

    // Stocker le nom de la ville
    m_cityName = forecastData.cityName;

    qDebug() << "Displaying chart for" << m_cityName << "with" << forecastData.entries.size() << "entries";

    // Ajouter les points de données
    for (const ForecastEntry& entry : forecastData.entries) {
        if (!entry.dateTime.isValid()) {
            continue; // Ignorer entrées invalides
        }

        // Convertir QDateTime en timestamp pour QtCharts
        qint64 timestamp = entry.dateTime.toMSecsSinceEpoch();

        // Ajouter points aux séries
        m_temperatureSeries->append(timestamp, entry.temperature);
        m_humiditySeries->append(timestamp, entry.humidity);
    }

    // Mettre à jour les plages d'axes
    updateAxisRanges(forecastData);

    // Mettre à jour le titre
    setChartTitle(QString("Prévisions pour %1 - %2 créneaux")
                      .arg(m_cityName)
                      .arg(forecastData.entries.size()));

    qDebug() << "Chart updated successfully";
}

void WeatherChartWidget::updateAxisRanges(const ForecastData& data)
{
    if (data.entries.isEmpty()) return;

    // Plage temporelle
    auto timeRange = getTimeRange(data);
    m_axisX->setRange(timeRange.first, timeRange.second);

    // Plage température avec marge
    auto tempRange = getTemperatureRange(data);
    double tempMargin = (tempRange.second - tempRange.first) * 0.1; // 10% de marge
    m_axisTemperature->setRange(tempRange.first - tempMargin,
                                tempRange.second + tempMargin);

    // Plage humidité (toujours 0-100% mais centré sur les données)
    auto humidityRange = getHumidityRange(data);
    double humidityMin = qMax(0.0, humidityRange.first - 10);
    double humidityMax = qMin(100.0, humidityRange.second + 10);
    m_axisHumidity->setRange(humidityMin, humidityMax);

    qDebug() << "Axis ranges - Time:" << timeRange.first << "to" << timeRange.second;
    qDebug() << "Temperature:" << tempRange.first << "to" << tempRange.second;
    qDebug() << "Humidity:" << humidityRange.first << "to" << humidityRange.second;
}

QPair<double, double> WeatherChartWidget::getTemperatureRange(const ForecastData& data) const
{
    if (data.entries.isEmpty()) return {0, 30};

    double minTemp = data.entries.first().temperature;
    double maxTemp = data.entries.first().temperature;

    for (const ForecastEntry& entry : data.entries) {
        minTemp = qMin(minTemp, entry.temperature);
        maxTemp = qMax(maxTemp, entry.temperature);
    }

    return {minTemp, maxTemp};
}

QPair<double, double> WeatherChartWidget::getHumidityRange(const ForecastData& data) const
{
    if (data.entries.isEmpty()) return {0, 100};

    double minHumidity = data.entries.first().humidity;
    double maxHumidity = data.entries.first().humidity;

    for (const ForecastEntry& entry : data.entries) {
        minHumidity = qMin(minHumidity, entry.humidity);
        maxHumidity = qMax(maxHumidity, entry.humidity);
    }

    return {minHumidity, maxHumidity};
}

QPair<QDateTime, QDateTime> WeatherChartWidget::getTimeRange(const ForecastData& data) const
{
    if (data.entries.isEmpty()) {
        QDateTime now = QDateTime::currentDateTime();
        return {now, now.addDays(5)};
    }

    QDateTime start = data.entries.first().dateTime;
    QDateTime end = data.entries.last().dateTime;

    return {start, end};
}

void WeatherChartWidget::onForecastDataReceived(const QString& cityName, const ForecastData& data)
{
    Q_UNUSED(cityName)
    displayForecastData(data);
}

void WeatherChartWidget::clearChart()
{
    if (m_temperatureSeries) {
        m_temperatureSeries->clear();
    }
    if (m_humiditySeries) {
        m_humiditySeries->clear();
    }

    setChartTitle("Aucune donnée de prévision");
    qDebug() << "Chart cleared";
}

void WeatherChartWidget::setChartTitle(const QString& title)
{
    if (m_chart) {
        m_chart->setTitle(title);
    }
    if (m_titleLabel) {
        m_titleLabel->setText(title);
    }
}

void WeatherChartWidget::setTemperatureColor(const QColor& color)
{
    m_temperatureColor = color;
    if (m_temperatureSeries) {
        m_temperatureSeries->setColor(color);
    }
    if (m_axisTemperature) {
        m_axisTemperature->setLabelsColor(color);
    }
}

void WeatherChartWidget::setHumidityColor(const QColor& color)
{
    m_humidityColor = color;
    if (m_humiditySeries) {
        m_humiditySeries->setColor(color);
    }
    if (m_axisHumidity) {
        m_axisHumidity->setLabelsColor(color);
    }
}

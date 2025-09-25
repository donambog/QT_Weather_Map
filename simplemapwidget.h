#ifndef SIMPLEMAPWIDGET_H
#define SIMPLEMAPWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include "WeatherData.h"

/**
 * Widget simple pour afficher la localisation d'une ville
 * Affiche le nom de la ville et ses coordonnées GPS
 */
class SimpleMapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SimpleMapWidget(QWidget* parent = nullptr);

    // Affichage manuel des informations
    void showLocation(const QString& cityName, const QString& country,
                      double latitude, double longitude);
    void clearLocation();

public slots:
    // Slot connecté aux signaux WeatherService
    void onWeatherDataReceived(const QString& cityName, const CurrentWeatherData& data);

private:
    // Interface
    QVBoxLayout* m_layout;
    QLabel* m_titleLabel;
    QLabel* m_locationLabel;
    QLabel* m_coordsLabel;
    QLabel* m_infoLabel;

    QString m_currentCity;
    double m_latitude;
    double m_longitude;

    // Méthodes privées
    void setupUI();
    void updateLocationDisplay();
    QString formatCoordinates(double latitude, double longitude) const;
};

#endif // SIMPLEMAPWIDGET_H

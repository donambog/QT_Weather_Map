#include "SimpleMapWidget.h"
#include <QDebug>

SimpleMapWidget::SimpleMapWidget(QWidget* parent)
    : QWidget(parent)
    , m_layout(nullptr)
    , m_titleLabel(nullptr)
    , m_locationLabel(nullptr)
    , m_coordsLabel(nullptr)
    , m_infoLabel(nullptr)
    , m_latitude(0.0)
    , m_longitude(0.0)
{
    setupUI();
    qDebug() << "SimpleMapWidget initialized";
}

void SimpleMapWidget::setupUI()
{
    // Layout principal
    m_layout = new QVBoxLayout(this);

    // Titre
    m_titleLabel = new QLabel("Localisation", this);
    m_titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    m_titleLabel->setAlignment(Qt::AlignCenter);

    // Ville et pays
    m_locationLabel = new QLabel("Aucune ville sélectionnée", this);
    m_locationLabel->setAlignment(Qt::AlignCenter);
    m_locationLabel->setStyleSheet("font-size: 12px; color: #333;");

    // Coordonnées GPS
    m_coordsLabel = new QLabel("Coordonnées: -", this);
    m_coordsLabel->setAlignment(Qt::AlignCenter);
    m_coordsLabel->setStyleSheet("font-size: 11px; color: #666;");

    // Information additionnelle
    m_infoLabel = new QLabel("Position sera affichée ici", this);
    m_infoLabel->setAlignment(Qt::AlignCenter);
    m_infoLabel->setStyleSheet("font-size: 10px; color: #888; margin-top: 10px;");

    // Ajouter au layout
    m_layout->addWidget(m_titleLabel);
    m_layout->addWidget(m_locationLabel);
    m_layout->addWidget(m_coordsLabel);
    m_layout->addStretch(); // Espace flexible
    m_layout->addWidget(m_infoLabel);

    // Style du widget
    setMinimumWidth(300);
    setMinimumHeight(200);
    setStyleSheet("SimpleMapWidget { border: 1px solid gray; background: #F0F8FF; border-radius: 5px; }");
}

void SimpleMapWidget::onWeatherDataReceived(const QString& cityName, const CurrentWeatherData& data)
{
    Q_UNUSED(cityName)

    if (!data.isValid()) {
        qWarning() << "Invalid weather data received in SimpleMapWidget";
        return;
    }

    // Stocker les données
    m_currentCity = data.cityName;
    m_latitude = data.latitude;
    m_longitude = data.longitude;

    // Mettre à jour l'affichage
    updateLocationDisplay();

    qDebug() << "SimpleMapWidget updated for" << m_currentCity
             << "at" << m_latitude << "," << m_longitude;
}

void SimpleMapWidget::showLocation(const QString& cityName, const QString& country,
                                   double latitude, double longitude)
{
    m_currentCity = cityName;
    m_latitude = latitude;
    m_longitude = longitude;

    m_locationLabel->setText(QString("%1, %2").arg(cityName).arg(country));
    m_coordsLabel->setText(formatCoordinates(latitude, longitude));

    // Information contextuelle
    m_infoLabel->setText("Position géographique");
}

void SimpleMapWidget::clearLocation()
{
    m_currentCity.clear();
    m_latitude = 0.0;
    m_longitude = 0.0;

    m_locationLabel->setText("Aucune ville sélectionnée");
    m_coordsLabel->setText("Coordonnées: -");
    m_infoLabel->setText("Position sera affichée ici");
}

void SimpleMapWidget::updateLocationDisplay()
{
    // Affichage du nom et pays
    m_locationLabel->setText(QString("%1, %2").arg(m_currentCity).arg("--"));
    // Coordonnées formatées
    m_coordsLabel->setText(formatCoordinates(m_latitude, m_longitude));

    // Information contextuelle selon la position
    QString hemisphere;
    if (m_latitude > 0) hemisphere += "Nord";
    else hemisphere += "Sud";

    if (m_longitude > 0) hemisphere += "-Est";
    else hemisphere += "-Ouest";

    m_infoLabel->setText(QString("Hémisphère %1").arg(hemisphere));

    // Style selon la latitude (couleur indicative)
    if (m_latitude > 60) {
        // Région arctique
        setStyleSheet("SimpleMapWidget { border: 1px solid gray; background: #E6F3FF; border-radius: 5px; }");
    } else if (m_latitude > 30) {
        // Région tempérée
        setStyleSheet("SimpleMapWidget { border: 1px solid gray; background: #F0F8FF; border-radius: 5px; }");
    } else if (m_latitude > -30) {
        // Région tropicale
        setStyleSheet("SimpleMapWidget { border: 1px solid gray; background: #FFF8E1; border-radius: 5px; }");
    } else {
        // Région australe
        setStyleSheet("SimpleMapWidget { border: 1px solid gray; background: #F0F8FF; border-radius: 5px; }");
    }
}

QString SimpleMapWidget::formatCoordinates(double latitude, double longitude) const
{
    QString latDir = latitude >= 0 ? "N" : "S";
    QString lonDir = longitude >= 0 ? "E" : "O";

    return QString("Lat: %1° %2, Lon: %3° %4")
        .arg(qAbs(latitude), 0, 'f', 4)
        .arg(latDir)
        .arg(qAbs(longitude), 0, 'f', 4)
        .arg(lonDir);
}

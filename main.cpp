#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QMessageBox>
#include "MainWindow.h"
#include "WeatherData.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Configuration application
    app.setApplicationName("WeatherApp");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("TheraSonic");
    app.setOrganizationDomain("therasonic.com");

    // Enregistrement des types personnalisés pour les signals/slots
    qRegisterMetaType<CurrentWeatherData>("CurrentWeatherData");
    qRegisterMetaType<ForecastData>("ForecastData");
    qRegisterMetaType<ForecastEntry>("ForecastEntry");

    // Créer dossier cache si nécessaire
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir().mkpath(cacheDir);
    qDebug() << "Cache directory:" << cacheDir;

    // Message de bienvenue
    qDebug() << "=== WeatherApp Starting ===";
    qDebug() << "Qt Version:" << QT_VERSION_STR;
    qDebug() << "Architecture: WeatherService + Cache + Signals/Slots";

    // Vérification réseau basique
    qDebug() << "Network accessible: Available";

    // Créer et afficher la fenêtre principale
    MainWindow window;

    // Message important pour la configuration API
    QMessageBox::information(&window, "Configuration API",
                             "Configurez votre cle API !\n\n"
                             "Créez le fichier config.json dans le dossier :\n"
                             "WeatherApp\\build\\Desktop_Qt_6_9_2_MinGW_64_bit-Debug\\\n\n"
                             "Contenu du fichier :\n"
                             "{\n"
                             "  \"api_key\": \"cle_api_32_caracteres\"\n"
                             "}\n\n"
                             "Obtenez votre clé sur: https://openweathermap.org/api");

    window.show();

    qDebug() << "MainWindow displayed - Ready for user interaction";

    return app.exec();
}

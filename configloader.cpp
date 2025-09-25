#include "ConfigLoader.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

ConfigLoader::ConfigLoader()
    : m_isValid(false)
{
}

bool ConfigLoader::loadConfig()
{
    // Réinitialiser l'état
    m_isValid = false;
    m_apiKey.clear();
    m_errorMessage.clear();

    // Ouvrir le fichier config.json
    QFile configFile("config.json");

    if (!configFile.exists()) {
        m_errorMessage = "Fichier config.json introuvable !\n"
                         "Créez le fichier avec votre clé API :\n"
                         "{\n  \"api_key\": \"votre_cle_ici\"\n}";
        return false;
    }

    if (!configFile.open(QIODevice::ReadOnly)) {
        m_errorMessage = QString("Impossible d'ouvrir config.json %1")
        .arg(configFile.errorString());
        return false;
    }

    // Lire et parser le JSON
    QByteArray jsonData = configFile.readAll();

    if (jsonData.isEmpty()) {
        m_errorMessage = "Le fichier config.json est vide !";
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    if (doc.isNull()) {
        m_errorMessage = QString("JSON invalide dans config.json :\n%1")
        .arg(parseError.errorString());
        return false;
    }

    if (!doc.isObject()) {
        m_errorMessage = "config.json doit contenir un objet JSON !";
        return false;
    }

    // Extraire la clé API
    QJsonObject config = doc.object();

    if (!config.contains("api_key")) {
        m_errorMessage = "Clé 'api_key' manquante dans config.json !\n"
                         "Format attendu :\n"
                         "{\n  \"api_key\": \"votre_cle_ici\"\n}";
        return false;
    }

    m_apiKey = config["api_key"].toString();

    if (m_apiKey.isEmpty()) {
        m_errorMessage = "La clé 'api_key' est vide dans config.json !";
        return false;
    }

    // Valider la clé API
    if (!isValidApiKey(m_apiKey)) {
        m_errorMessage = QString("Clé API invalide : \n"
                                 "Les clés OpenWeatherMap font 32 caractères (lettres et chiffres)");
        return false;
    }

    // Tout est OK !
    m_isValid = true;
    qDebug() << " Configuration chargée avec succès";
    qDebug() << "   Clé API:" << m_apiKey.left(4) + "..." + m_apiKey.right(4);

    return true;
}

QString ConfigLoader::getApiKey() const
{
    return m_apiKey;
}

bool ConfigLoader::isValid() const
{
    return m_isValid;
}

QString ConfigLoader::getErrorMessage() const
{
    return m_errorMessage;
}

bool ConfigLoader::isValidApiKey(const QString& key) const
{
    // Validation OpenWeatherMap :
    // - 32 caractères exactement
    // - Seulement lettres (a-f) et chiffres (0-9)
    // - Format hexadécimal

    if (key.length() != 32) {
        return false;
    }

    // Vérifier que tous les caractères sont hexadécimaux
    for (const QChar& c : key) {
        if (!c.isDigit() && !(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F')) {
            return false;
        }
    }

    return true;
}

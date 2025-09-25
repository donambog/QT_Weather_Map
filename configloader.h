#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <QString>

/**
 * Classe simple pour charger la configuration depuis config.json
 * - Lit le fichier config.json à la racine du projet
 * - Valide la clé API
 * - Gère les erreurs proprement
 */
class ConfigLoader
{
public:
    ConfigLoader();

    // Chargement configuration
    bool loadConfig();

    // Accès aux données
    QString getApiKey() const;
    bool isValid() const;

    // Gestion des erreurs
    QString getErrorMessage() const;

private:
    // Validation
    bool isValidApiKey(const QString& key) const;

    // Données
    QString m_apiKey;
    QString m_errorMessage;
    bool m_isValid;
};

#endif // CONFIGLOADER_H


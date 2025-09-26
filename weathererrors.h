#ifndef WEATHERERRORS_H
#define WEATHERERRORS_H

namespace WeatherErrors {

// =====================================================
// ERREURS RÉSEAU
// =====================================================
constexpr char CONNECTION_REFUSED[] = "Connexion refusée - Vérifiez votre connexion internet";
constexpr char REMOTE_HOST_CLOSED[] = "Serveur météo indisponible";
constexpr char HOST_NOT_FOUND[] = "Serveur météo introuvable";
constexpr char TIMEOUT[] = "Délai d'attente dépassé";
constexpr char SSL_HANDSHAKE[] = "Erreur de sécurité SSL";
constexpr char AUTHENTICATION[] = "Clé API invalide";
constexpr char NETWORK_UNKNOWN[] = "Erreur réseau inconnue";

// =====================================================
// CODES D'ERREUR API OPENWEATHERMAP
// =====================================================
namespace ApiCodes {
// Codes de succès
constexpr int SUCCESS = 200;

// Codes d'erreur client (4xx)
constexpr int BAD_REQUEST = 400;        // Requête malformée
constexpr int UNAUTHORIZED = 401;       // Clé API invalide
constexpr int FORBIDDEN = 403;          // Accès refusé
constexpr int NOT_FOUND = 404;          // Ville introuvable
constexpr int METHOD_NOT_ALLOWED = 405; // Méthode HTTP incorrecte
constexpr int TOO_MANY_REQUESTS = 429;  // Quota dépassé

// Codes d'erreur serveur (5xx)
constexpr int INTERNAL_ERROR = 500;     // Erreur interne serveur
constexpr int BAD_GATEWAY = 502;        // Passerelle défaillante
constexpr int SERVICE_UNAVAILABLE = 503;// Service indisponible
constexpr int GATEWAY_TIMEOUT = 504;    // Timeout passerelle
}

// =====================================================
// MESSAGES D'ERREUR API
// =====================================================
namespace ApiMessages {
// Messages pour codes 4xx (erreurs client)
constexpr char BAD_REQUEST[] = "Requête invalide - Vérifiez les paramètres";
constexpr char UNAUTHORIZED[] = "Clé API invalide ou expirée";
constexpr char FORBIDDEN[] = "Accès refusé - Vérifiez vos permissions API";
constexpr char NOT_FOUND[] = "Ville introuvable - Vérifiez l'orthographe";
constexpr char METHOD_NOT_ALLOWED[] = "Méthode de requête incorrecte";
constexpr char TOO_MANY_REQUESTS[] = "Quota d'API dépassé - Réessayez plus tard";

// Messages pour codes 5xx (erreurs serveur)
constexpr char INTERNAL_ERROR[] = "Erreur interne du serveur météo";
constexpr char BAD_GATEWAY[] = "Problème de passerelle - Réessayez";
constexpr char SERVICE_UNAVAILABLE[] = "Service météo temporairement indisponible";
constexpr char GATEWAY_TIMEOUT[] = "Timeout du serveur - Réessayez";

// Messages génériques
constexpr char UNKNOWN[] = "Erreur API inconnue";
constexpr char NO_MESSAGE[] = "Aucun détail d'erreur fourni";
}

// =====================================================
// MESSAGES DE VALIDATION
// =====================================================
constexpr char EMPTY_CITY_NAME[] = "Nom de ville vide";
constexpr char INVALID_API_KEY[] = "Clé API manquante ou invalide";
constexpr char INVALID_API_RESPONSE[] = "Réponse API invalide";
constexpr char INVALID_WEATHER_DATA[] = "Données météo invalides";
constexpr char INVALID_FORECAST_DATA[] = "Données prévisions invalides";
constexpr char PARSING_JSON[] = "Erreur de parsing JSON";

} // namespace WeatherErrors

#endif // WEATHERERRORS_H

// SearchHistory.h - Nouvelle classe pour gérer l'historique
#ifndef SEARCHHISTORY_H
#define SEARCHHISTORY_H

#include <QObject>
#include <QStringList>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QTimer>
#include <QMap>

/**
 * Structure pour une entrée d'historique
 */
struct SearchHistoryEntry {
    QString cityName;           // Nom de la ville recherchée
    QDateTime searchTime;       // Moment de la recherche
    int searchCount;           // Nombre de fois recherchée
    QDateTime lastAccess;      // Dernier accès
    bool isFavorite;          // Marquée comme favorite

    SearchHistoryEntry()
        : searchCount(1), isFavorite(false)
    {
        searchTime = QDateTime::currentDateTime();
        lastAccess = searchTime;
    }

    SearchHistoryEntry(const QString& city)
        : cityName(city), searchCount(1), isFavorite(false)
    {
        searchTime = QDateTime::currentDateTime();
        lastAccess = searchTime;
    }

    // Pour le tri par fréquence/récence
    bool operator<(const SearchHistoryEntry& other) const {
        // Priorité aux favoris
        if (isFavorite != other.isFavorite) {
            return isFavorite > other.isFavorite;
        }
        // Puis par nombre de recherches
        if (searchCount != other.searchCount) {
            return searchCount > other.searchCount;
        }
        // Puis par récence
        return lastAccess > other.lastAccess;
    }
};

/**
 * Gestionnaire d'historique des recherches
 *
 * Fonctionnalités :
 * - Sauvegarde automatique des recherches
 * - Tri intelligent (favoris > fréquence > récence)
 * - Persistance sur disque
 * - Suggestions de villes
 * - Gestion des favoris
 */
class SearchHistory : public QObject
{
    Q_OBJECT

public:
    explicit SearchHistory(QObject* parent = nullptr);
    ~SearchHistory();

    // Gestion des recherches
    void addSearch(const QString& cityName);
    void removeSearch(const QString& cityName);
    void clearHistory();

    // Accès aux données
    QStringList getRecentSearches(int maxCount = 10) const;
    QStringList getFrequentSearches(int maxCount = 5) const;
    QStringList getFavorites() const;
    QStringList getSuggestions(const QString& prefix, int maxCount = 5) const;

    // Gestion des favoris
    void addToFavorites(const QString& cityName);
    void removeFromFavorites(const QString& cityName);
    bool isFavorite(const QString& cityName) const;
    void toggleFavorite(const QString& cityName);

    // Statistiques
    int getTotalSearches() const;
    int getSearchCount(const QString& cityName) const;
    QDateTime getLastSearchTime(const QString& cityName) const;

    // Configuration
    void setMaxHistorySize(int maxSize);
    int getMaxHistorySize() const { return m_maxHistorySize; }

signals:
    void historyChanged();
    void favoriteChanged(const QString& cityName, bool isFavorite);

private slots:
    void saveToFile();

private:

    QMap<QString, SearchHistoryEntry> m_entries;
    int m_maxHistorySize;
    QString m_historyFilePath;
    QTimer* m_saveTimer;  // Sauvegarde différée

    // Persistance
    void loadFromFile();
    QString getHistoryFilePath() const;
    QJsonObject serializeEntry(const SearchHistoryEntry& entry) const;
    SearchHistoryEntry deserializeEntry(const QJsonObject& json) const;

    // Utilitaires
    void cleanupOldEntries();
    QString normalizeCityName(const QString& cityName) const;
};

#endif // SEARCHHISTORY_H

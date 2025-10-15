// NetworkManager.h - Abstraction réseau pour tests
#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkReply>
#include <QString>

struct NetworkRequest {
    QString url;
    QMap<QString, QString> headers;
    int timeoutMs;
};

struct NetworkResponse {
    int httpCode;
    QByteArray data;
    QString errorString;
    bool isSuccess() const { return httpCode >= 200 && httpCode < 300; }
};

class INetworkManager : public QObject
{
    Q_OBJECT
public:
    virtual ~INetworkManager() = default;
    virtual void get(const NetworkRequest& request, const QString& requestId) = 0;

signals:
    void requestFinished(const QString& requestId, const NetworkResponse& response);
    void requestError(const QString& requestId, const QString& error);
};

// Implémentation réelle
class QtNetworkManager : public INetworkManager
{
    Q_OBJECT
public:
    explicit QtNetworkManager(QObject* parent = nullptr);
    void get(const NetworkRequest& request, const QString& requestId) override;

private slots:
    void onReplyFinished();

private:
    QNetworkAccessManager* m_manager;
    QMap<QNetworkReply*, QString> m_pendingRequests;
};

// Mock pour les tests
class MockNetworkManager : public INetworkManager
{
    Q_OBJECT
public:
    explicit MockNetworkManager(QObject* parent = nullptr);
    
    void get(const NetworkRequest& request, const QString& requestId) override;
    
    // Méthodes de test
    void setMockResponse(const NetworkResponse& response);
    void setMockError(const QString& error);
    NetworkRequest getLastRequest() const { return m_lastRequest; }

private:
    NetworkRequest m_lastRequest;
    NetworkResponse m_mockResponse;
    QString m_mockError;
    bool m_shouldError = false;
};

#endif
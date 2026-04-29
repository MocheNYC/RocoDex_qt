#pragma once

#include "DexRuntime.h"
#include "PetListModel.h"

#include <QObject>
#include <QNetworkAccessManager>
#include <QSet>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

class DexController : public QObject {
    Q_OBJECT
    Q_PROPERTY(PetListModel *petModel READ petModel CONSTANT)
    Q_PROPERTY(QVariantMap selectedPet READ selectedPet NOTIFY selectedPetChanged)
    Q_PROPERTY(QVariantList attributes READ attributes NOTIFY dataChanged)
    Q_PROPERTY(QVariantList stages READ stages CONSTANT)
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY filtersChanged)
    Q_PROPERTY(QString stage READ stage WRITE setStage NOTIFY filtersChanged)
    Q_PROPERTY(QString sortKey READ sortKey WRITE setSortKey NOTIFY filtersChanged)
    Q_PROPERTY(QString attributeMode READ attributeMode WRITE setAttributeMode NOTIFY filtersChanged)
    Q_PROPERTY(QStringList selectedAttributes READ selectedAttributes NOTIFY filtersChanged)
    Q_PROPERTY(bool onlyDistributed READ onlyDistributed WRITE setOnlyDistributed NOTIFY filtersChanged)
    Q_PROPERTY(int resultCount READ resultCount NOTIFY resultCountChanged)
    Q_PROPERTY(int petCount READ petCount NOTIFY dataChanged)
    Q_PROPERTY(int skillCount READ skillCount NOTIFY dataChanged)
    Q_PROPERTY(QString dataStatus READ dataStatus NOTIFY dataChanged)
    Q_PROPERTY(QString snapshotTime READ snapshotTime NOTIFY dataChanged)
    Q_PROPERTY(QString updateMessage READ updateMessage NOTIFY updateStateChanged)
    Q_PROPERTY(bool updateBusy READ updateBusy NOTIFY updateStateChanged)
    Q_PROPERTY(QVariantList favoritePets READ favoritePets NOTIFY quickPetsChanged)
    Q_PROPERTY(QVariantList recentPets READ recentPets NOTIFY quickPetsChanged)

public:
    explicit DexController(QObject *parent = nullptr);

    PetListModel *petModel();
    QVariantMap selectedPet() const;
    QVariantList attributes() const;
    QVariantList stages() const;

    QString query() const;
    void setQuery(const QString &query);

    QString stage() const;
    void setStage(const QString &stage);

    QString sortKey() const;
    void setSortKey(const QString &sortKey);

    QString attributeMode() const;
    void setAttributeMode(const QString &attributeMode);

    QStringList selectedAttributes() const;
    bool onlyDistributed() const;
    void setOnlyDistributed(bool onlyDistributed);

    int resultCount() const;
    int petCount() const;
    int skillCount() const;
    QString dataStatus() const;
    QString snapshotTime() const;
    QString updateMessage() const;
    bool updateBusy() const;
    QVariantList favoritePets() const;
    QVariantList recentPets() const;

    Q_INVOKABLE void selectPet(const QString &key);
    Q_INVOKABLE void selectRow(int row);
    Q_INVOKABLE void selectPrevious();
    Q_INVOKABLE void selectNext();
    Q_INVOKABLE void clearFilters();
    Q_INVOKABLE void toggleAttribute(const QString &key);
    Q_INVOKABLE void toggleFavorite();
    Q_INVOKABLE void checkForUpdates();
    Q_INVOKABLE void resetToEmbeddedData();

signals:
    void selectedPetChanged();
    void filtersChanged();
    void resultCountChanged();
    void dataChanged();
    void updateStateChanged();
    void quickPetsChanged();

private:
    void loadData();
    void applyBundle(DexDataBundle bundle, const QString &sourceLabel, const QString &preferredKey = {});
    void applyFilters();
    void rememberRecent(const QString &key);
    QStringList favoriteKeys() const;
    void saveFavorites() const;
    void setUpdateMessage(const QString &message);
    void setUpdateBusy(bool busy);
    bool isRemoteNewer(const DexDataBundle &remote) const;
    qint64 dataSizeScore(const DexDataBundle &bundle) const;

    DexRuntime m_runtime;
    PetListModel m_petModel;
    QNetworkAccessManager m_network;
    QString m_selectedKey;
    QString m_query;
    QString m_stage = QStringLiteral("全部");
    QString m_sortKey = QStringLiteral("id");
    QString m_attributeMode = QStringLiteral("any");
    QSet<QString> m_selectedAttributes;
    QSet<QString> m_favorites;
    QStringList m_recent;
    bool m_onlyDistributed = false;
    QString m_dataStatus;
    QString m_updateMessage;
    bool m_updateBusy = false;
};

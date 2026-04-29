#pragma once

#include "DexRuntime.h"

#include <QAbstractListModel>
#include <QSet>

class PetListModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        KeyRole = Qt::UserRole + 1,
        IdRole,
        NameZhRole,
        NameEnRole,
        ImageRole,
        StageRole,
        RoleLabelRole,
        BaseStatsRole,
        SpeedRole,
        HealthRole,
        FavoriteRole,
        AttributesRole,
        SummaryRole,
    };

    explicit PetListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setRuntime(DexRuntime *runtime);
    void setFavorites(const QSet<QString> &favorites);
    void setFilters(
        const QString &query,
        const QSet<QString> &attributes,
        const QString &attributeMode,
        const QString &stage,
        bool onlyDistributed,
        const QString &sortKey);

    QString keyAt(int row) const;
    int rowOfKey(const QString &key) const;
    int count() const;

private:
    void rebuild();

    DexRuntime *m_runtime = nullptr;
    QVector<int> m_rows;
    QSet<QString> m_favorites;
    QString m_query;
    QSet<QString> m_attributes;
    QString m_attributeMode = QStringLiteral("any");
    QString m_stage = QStringLiteral("全部");
    bool m_onlyDistributed = false;
    QString m_sortKey = QStringLiteral("id");
};

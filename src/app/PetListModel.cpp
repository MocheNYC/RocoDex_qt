#include "PetListModel.h"

PetListModel::PetListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int PetListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_rows.size();
}

QVariant PetListModel::data(const QModelIndex &index, int role) const
{
    if (!m_runtime || !index.isValid() || index.row() < 0 || index.row() >= m_rows.size())
        return {};

    const auto &pets = m_runtime->bundle().pets;
    const auto petIndex = m_rows.at(index.row());
    if (petIndex < 0 || petIndex >= pets.size())
        return {};

    const auto &pet = pets.at(petIndex);
    const bool favorite = m_favorites.contains(pet.key);
    const QVariantMap summary = m_runtime->petSummary(pet, favorite);

    switch (role) {
    case KeyRole:
        return pet.key;
    case IdRole:
        return pet.id;
    case NameZhRole:
        return pet.nameZh;
    case NameEnRole:
        return pet.nameEn;
    case ImageRole:
        return summary.value(QStringLiteral("image"));
    case StageRole:
        return pet.stage;
    case RoleLabelRole:
        return summary.value(QStringLiteral("roleLabel"));
    case BaseStatsRole:
        return pet.stats.baseStats;
    case SpeedRole:
        return pet.stats.speed;
    case HealthRole:
        return pet.stats.health;
    case FavoriteRole:
        return favorite;
    case AttributesRole:
        return summary.value(QStringLiteral("attributes"));
    case SummaryRole:
        return summary;
    default:
        return {};
    }
}

QHash<int, QByteArray> PetListModel::roleNames() const
{
    return {
        { KeyRole, "key" },
        { IdRole, "petId" },
        { NameZhRole, "nameZh" },
        { NameEnRole, "nameEn" },
        { ImageRole, "image" },
        { StageRole, "stage" },
        { RoleLabelRole, "roleLabel" },
        { BaseStatsRole, "baseStats" },
        { SpeedRole, "speed" },
        { HealthRole, "health" },
        { FavoriteRole, "favorite" },
        { AttributesRole, "attributes" },
        { SummaryRole, "summary" },
    };
}

void PetListModel::setRuntime(DexRuntime *runtime)
{
    beginResetModel();
    m_runtime = runtime;
    m_rows = runtime ? runtime->allIndexes() : QVector<int> {};
    endResetModel();
    rebuild();
}

void PetListModel::setFavorites(const QSet<QString> &favorites)
{
    m_favorites = favorites;
    if (!m_rows.isEmpty())
        emit dataChanged(index(0, 0), index(m_rows.size() - 1, 0), { FavoriteRole, SummaryRole });
}

void PetListModel::setFilters(
    const QString &query,
    const QSet<QString> &attributes,
    const QString &attributeMode,
    const QString &stage,
    bool onlyDistributed,
    const QString &sortKey)
{
    m_query = query;
    m_attributes = attributes;
    m_attributeMode = attributeMode;
    m_stage = stage;
    m_onlyDistributed = onlyDistributed;
    m_sortKey = sortKey;
    rebuild();
}

QString PetListModel::keyAt(int row) const
{
    if (!m_runtime || row < 0 || row >= m_rows.size())
        return {};
    return m_runtime->bundle().pets.at(m_rows.at(row)).key;
}

int PetListModel::rowOfKey(const QString &key) const
{
    if (!m_runtime)
        return -1;

    const auto &pets = m_runtime->bundle().pets;
    for (int row = 0; row < m_rows.size(); ++row) {
        if (pets.at(m_rows.at(row)).key == key)
            return row;
    }
    return -1;
}

int PetListModel::count() const
{
    return m_rows.size();
}

void PetListModel::rebuild()
{
    if (!m_runtime)
        return;

    beginResetModel();
    m_rows = m_runtime->filterPets(m_query, m_attributes, m_attributeMode, m_stage, m_onlyDistributed, m_sortKey);
    endResetModel();
}

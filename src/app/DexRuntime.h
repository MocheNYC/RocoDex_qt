#pragma once

#include "../models/DexTypes.h"

#include <QSet>
#include <QVariantList>
#include <QVariantMap>

class DexRuntime {
public:
    void setBundle(DexDataBundle bundle);

    const DexDataBundle &bundle() const;
    const QVector<int> &allIndexes() const;

    QVector<int> filterPets(
        const QString &query,
        const QSet<QString> &attributes,
        const QString &attributeMode,
        const QString &stage,
        bool onlyDistributed,
        const QString &sortKey) const;

    const Pet *petByKey(const QString &key) const;
    const SkillInfo *skillByName(const QString &name) const;
    const AttributeMeta *attributeByKey(const QString &key) const;

    QString attributeLabel(const QString &key) const;
    QString roleLabel(const Pet &pet) const;
    QString evolutionRequirementLabel(const Pet &pet) const;
    QVector<const Pet *> evolutionChain(const Pet &pet) const;
    QVector<MatchupEntry> offensiveCoverage(const Pet &pet) const;
    DefensiveProfile defensiveProfile(const Pet &pet) const;

    QVariantMap petSummary(const Pet &pet, bool favorite) const;
    QVariantMap petDetail(const Pet &pet, bool favorite) const;
    QVariantList attributeOptions() const;
    QVariantList stageOptions() const;

private:
    QString normalizedQuery(const QString &value) const;
    QString chineseInitials(const QString &value) const;
    QString searchText(const Pet &pet) const;
    int comparePets(const Pet &left, const Pet &right, const QString &sortKey) const;

    DexDataBundle m_bundle;
    QVector<int> m_allIndexes;
    PetStats m_maxStats;
    QHash<QString, int> m_petIndexByKey;
    QHash<QString, int> m_skillIndexByName;
    QHash<QString, int> m_attributeIndexByKey;
};

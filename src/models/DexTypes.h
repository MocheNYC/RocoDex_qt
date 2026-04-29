#pragma once

#include <QHash>
#include <QString>
#include <QVector>

struct PetStats {
    int health = 0;
    int physicalAttack = 0;
    int magicAttack = 0;
    int physicalDefense = 0;
    int magicDefense = 0;
    int speed = 0;
    int baseStats = 0;
};

struct EvolutionInfo {
    QString previous;
    QString next;
    int level = 0;
    QString condition;
};

struct PetSkill {
    QString name;
    int level = 0;
};

struct Pet {
    QString key;
    QString title;
    QString href;
    QString id;
    QString nameZh;
    QString nameEn;
    QString image;
    QVector<QString> attributes;
    QString formName;
    QString initialName;
    QString petType;
    bool hasShiny = false;
    QString introductionZh;
    QString introductionEn;
    QString traitName;
    QString traitDescription;
    QString height;
    QString weight;
    QString distributionZh;
    QString distributionEn;
    EvolutionInfo evolution;
    QString stage;
    PetStats stats;
    QVector<QString> dexTasks;
    QVector<QString> taskSkillStones;
    QVector<PetSkill> skills;
    QVector<QString> bloodlineSkills;
    QVector<QString> learnableSkillStones;
    QString updateVersion;
    QString pageUrl;
    QString sourceKey;
};

struct AttributeMeta {
    QString key;
    QString nameZh;
    QString nameEn;
    QString color;
    QString textColor;
    QString descriptionZh;
    QHash<QString, QVector<QString>> offense;
    QHash<QString, QVector<QString>> defense;
};

struct SkillInfo {
    QString name;
    QString attribute;
    QString category;
    int energy = -1;
    int power = -1;
    QString effect;
    QString description;
    QString version;
    QString pageUrl;
};

struct DataSnapshot {
    QString importedAt;
    int petCount = 0;
    int detailCount = 0;
    int evolutionDetailCount = 0;
    int skillCount = 0;
    int skillDetailCount = 0;
    int attributeCount = 0;
    QString sourceUrl;
    QString skillSourceUrl;
    qint64 updatePackageVersion = 0;
};

struct DexDataBundle {
    DataSnapshot snapshot;
    QVector<Pet> pets;
    QVector<AttributeMeta> attributes;
    QVector<SkillInfo> skills;
};

struct MatchupEntry {
    QString key;
    QString label;
    double multiplier = 1.0;
};

struct DefensiveProfile {
    QVector<MatchupEntry> weak;
    QVector<MatchupEntry> resistant;
    QVector<MatchupEntry> neutral;
};

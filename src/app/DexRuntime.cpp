#include "DexRuntime.h"

#include "DexRepository.h"

#include <QCollator>
#include <QVariant>
#include <algorithm>

namespace {

QVariantMap statRow(const QString &key, const QString &label, int value, int maxValue)
{
    QVariantMap row;
    row.insert(QStringLiteral("key"), key);
    row.insert(QStringLiteral("label"), label);
    row.insert(QStringLiteral("value"), value);
    row.insert(QStringLiteral("percent"), maxValue > 0 ? qRound((value * 100.0) / maxValue) : 0);
    return row;
}

QVariantMap matchupMap(const MatchupEntry &entry)
{
    QVariantMap map;
    map.insert(QStringLiteral("key"), entry.key);
    map.insert(QStringLiteral("label"), entry.label);
    map.insert(QStringLiteral("multiplier"), entry.multiplier);
    map.insert(QStringLiteral("text"), QStringLiteral("%1 x%2").arg(entry.label).arg(entry.multiplier));
    return map;
}

QVariantList matchupList(const QVector<MatchupEntry> &entries)
{
    QVariantList list;
    list.reserve(entries.size());
    for (const auto &entry : entries)
        list.push_back(matchupMap(entry));
    return list;
}

bool containsKey(const QVector<QString> &items, const QString &key)
{
    return std::find(items.cbegin(), items.cend(), key) != items.cend();
}

} // namespace

void DexRuntime::setBundle(DexDataBundle bundle)
{
    m_bundle = std::move(bundle);
    m_allIndexes.clear();
    m_petIndexByKey.clear();
    m_skillIndexByName.clear();
    m_attributeIndexByKey.clear();
    m_maxStats = {};

    m_allIndexes.reserve(m_bundle.pets.size());
    for (int i = 0; i < m_bundle.pets.size(); ++i) {
        const auto &pet = m_bundle.pets.at(i);
        m_allIndexes.push_back(i);
        m_petIndexByKey.insert(pet.key, i);
        m_maxStats.health = std::max(m_maxStats.health, pet.stats.health);
        m_maxStats.physicalAttack = std::max(m_maxStats.physicalAttack, pet.stats.physicalAttack);
        m_maxStats.magicAttack = std::max(m_maxStats.magicAttack, pet.stats.magicAttack);
        m_maxStats.physicalDefense = std::max(m_maxStats.physicalDefense, pet.stats.physicalDefense);
        m_maxStats.magicDefense = std::max(m_maxStats.magicDefense, pet.stats.magicDefense);
        m_maxStats.speed = std::max(m_maxStats.speed, pet.stats.speed);
        m_maxStats.baseStats = std::max(m_maxStats.baseStats, pet.stats.baseStats);
    }

    for (int i = 0; i < m_bundle.skills.size(); ++i)
        m_skillIndexByName.insert(m_bundle.skills.at(i).name, i);

    for (int i = 0; i < m_bundle.attributes.size(); ++i)
        m_attributeIndexByKey.insert(m_bundle.attributes.at(i).key, i);
}

const DexDataBundle &DexRuntime::bundle() const
{
    return m_bundle;
}

const QVector<int> &DexRuntime::allIndexes() const
{
    return m_allIndexes;
}

QVector<int> DexRuntime::filterPets(
    const QString &query,
    const QSet<QString> &attributes,
    const QString &attributeMode,
    const QString &stage,
    bool onlyDistributed,
    const QString &sortKey) const
{
    const QString normalized = normalizedQuery(query);
    QVector<int> result;

    for (int i = 0; i < m_bundle.pets.size(); ++i) {
        const auto &pet = m_bundle.pets.at(i);

        const bool matchesQuery = normalized.isEmpty() || searchText(pet).contains(normalized);
        const bool matchesStage = stage.isEmpty() || stage == QStringLiteral("全部") || pet.stage == stage;
        const bool matchesDistribution = !onlyDistributed || !pet.distributionZh.isEmpty();

        bool matchesAttributes = true;
        if (!attributes.isEmpty()) {
            if (attributeMode == QStringLiteral("all")) {
                for (const auto &attribute : attributes) {
                    if (!containsKey(pet.attributes, attribute)) {
                        matchesAttributes = false;
                        break;
                    }
                }
            } else {
                matchesAttributes = false;
                for (const auto &attribute : attributes) {
                    if (containsKey(pet.attributes, attribute)) {
                        matchesAttributes = true;
                        break;
                    }
                }
            }
        }

        if (matchesQuery && matchesStage && matchesDistribution && matchesAttributes)
            result.push_back(i);
    }

    std::sort(result.begin(), result.end(), [&](int leftIndex, int rightIndex) {
        return comparePets(m_bundle.pets.at(leftIndex), m_bundle.pets.at(rightIndex), sortKey) < 0;
    });

    return result;
}

const Pet *DexRuntime::petByKey(const QString &key) const
{
    const auto index = m_petIndexByKey.value(key, -1);
    return index >= 0 ? &m_bundle.pets.at(index) : nullptr;
}

const SkillInfo *DexRuntime::skillByName(const QString &name) const
{
    const auto index = m_skillIndexByName.value(name, -1);
    return index >= 0 ? &m_bundle.skills.at(index) : nullptr;
}

const AttributeMeta *DexRuntime::attributeByKey(const QString &key) const
{
    const auto index = m_attributeIndexByKey.value(key, -1);
    return index >= 0 ? &m_bundle.attributes.at(index) : nullptr;
}

QString DexRuntime::attributeLabel(const QString &key) const
{
    const auto *attribute = attributeByKey(key);
    return attribute ? attribute->nameZh : key;
}

QString DexRuntime::roleLabel(const Pet &pet) const
{
    const auto &stats = pet.stats;
    if (stats.baseStats == 0)
        return QStringLiteral("资料待补");

    const int attack = std::max(stats.physicalAttack, stats.magicAttack);
    const int defense = std::max(stats.physicalDefense, stats.magicDefense);

    if (stats.speed >= attack && stats.speed >= defense)
        return QStringLiteral("高速压制");
    if (attack >= defense + 20)
        return stats.physicalAttack >= stats.magicAttack ? QStringLiteral("物攻输出") : QStringLiteral("魔攻输出");
    if (defense >= attack + 20 || stats.health >= 115)
        return QStringLiteral("耐久站场");
    return QStringLiteral("均衡成长");
}

QString DexRuntime::evolutionRequirementLabel(const Pet &pet) const
{
    QStringList parts;
    if (pet.evolution.level > 0)
        parts << QStringLiteral("%1级").arg(pet.evolution.level);
    if (!pet.evolution.condition.isEmpty())
        parts << pet.evolution.condition;
    return parts.isEmpty() ? QStringLiteral("进化条件待补") : parts.join(QStringLiteral(" · "));
}

QVector<const Pet *> DexRuntime::evolutionChain(const Pet &pet) const
{
    QVector<const Pet *> chain;
    QSet<QString> backtrackVisited;
    const Pet *current = &pet;

    while (current && !current->evolution.previous.isEmpty() && !backtrackVisited.contains(current->evolution.previous)) {
        backtrackVisited.insert(current->key);
        current = petByKey(current->evolution.previous);
    }

    QSet<QString> forwardVisited;
    while (current && !forwardVisited.contains(current->key)) {
        forwardVisited.insert(current->key);
        chain.push_back(current);
        current = current->evolution.next.isEmpty() ? nullptr : petByKey(current->evolution.next);
    }

    return chain;
}

QVector<MatchupEntry> DexRuntime::offensiveCoverage(const Pet &pet) const
{
    QSet<QString> targetKeys;
    for (const auto &attributeKey : pet.attributes) {
        const auto *attribute = attributeByKey(attributeKey);
        if (!attribute)
            continue;
        for (const auto &target : attribute->offense.value(QStringLiteral("2.0")))
            targetKeys.insert(target);
    }

    QVector<MatchupEntry> result;
    result.reserve(targetKeys.size());
    for (const auto &key : targetKeys)
        result.push_back({ key, attributeLabel(key), 2.0 });

    QCollator collator;
    collator.setLocale(QLocale(QStringLiteral("zh_CN")));
    std::sort(result.begin(), result.end(), [&](const MatchupEntry &left, const MatchupEntry &right) {
        return collator.compare(left.label, right.label) < 0;
    });
    return result;
}

DefensiveProfile DexRuntime::defensiveProfile(const Pet &pet) const
{
    DefensiveProfile profile;

    for (const auto &attackingAttribute : m_bundle.attributes) {
        double multiplier = 1.0;
        for (const auto &defensiveKey : pet.attributes) {
            const auto *defensive = attributeByKey(defensiveKey);
            if (!defensive)
                continue;
            if (containsKey(defensive->defense.value(QStringLiteral("2.0")), attackingAttribute.key))
                multiplier *= 2.0;
            else if (containsKey(defensive->defense.value(QStringLiteral("0.5")), attackingAttribute.key))
                multiplier *= 0.5;
        }

        MatchupEntry entry { attackingAttribute.key, attackingAttribute.nameZh, multiplier };
        if (multiplier > 1.0)
            profile.weak.push_back(entry);
        else if (multiplier < 1.0)
            profile.resistant.push_back(entry);
        else
            profile.neutral.push_back(entry);
    }

    std::sort(profile.weak.begin(), profile.weak.end(), [](const MatchupEntry &left, const MatchupEntry &right) {
        if (left.multiplier != right.multiplier)
            return left.multiplier > right.multiplier;
        return left.label < right.label;
    });
    std::sort(profile.resistant.begin(), profile.resistant.end(), [](const MatchupEntry &left, const MatchupEntry &right) {
        if (left.multiplier != right.multiplier)
            return left.multiplier < right.multiplier;
        return left.label < right.label;
    });
    std::sort(profile.neutral.begin(), profile.neutral.end(), [](const MatchupEntry &left, const MatchupEntry &right) {
        return left.label < right.label;
    });

    return profile;
}

QVariantMap DexRuntime::petSummary(const Pet &pet, bool favorite) const
{
    QVariantList attributes;
    for (const auto &key : pet.attributes) {
        const auto *attribute = attributeByKey(key);
        QVariantMap item;
        item.insert(QStringLiteral("key"), key);
        item.insert(QStringLiteral("label"), attribute ? attribute->nameZh : key);
        item.insert(QStringLiteral("color"), attribute ? attribute->color : QStringLiteral("#d8dee8"));
        item.insert(QStringLiteral("textColor"), attribute ? attribute->textColor : QStringLiteral("#18202a"));
        attributes.push_back(item);
    }

    QVariantMap map;
    map.insert(QStringLiteral("key"), pet.key);
    map.insert(QStringLiteral("id"), pet.id);
    map.insert(QStringLiteral("nameZh"), pet.nameZh);
    map.insert(QStringLiteral("nameEn"), pet.nameEn);
    map.insert(QStringLiteral("image"), DexRepository::imageSource(pet.image));
    map.insert(QStringLiteral("stage"), pet.stage);
    map.insert(QStringLiteral("roleLabel"), roleLabel(pet));
    map.insert(QStringLiteral("baseStats"), pet.stats.baseStats);
    map.insert(QStringLiteral("speed"), pet.stats.speed);
    map.insert(QStringLiteral("health"), pet.stats.health);
    map.insert(QStringLiteral("favorite"), favorite);
    map.insert(QStringLiteral("attributes"), attributes);
    return map;
}

QVariantMap DexRuntime::petDetail(const Pet &pet, bool favorite) const
{
    QVariantMap map = petSummary(pet, favorite);
    map.insert(QStringLiteral("formName"), pet.formName);
    map.insert(QStringLiteral("petType"), pet.petType);
    map.insert(QStringLiteral("hasShiny"), pet.hasShiny);
    map.insert(QStringLiteral("traitName"), pet.traitName.isEmpty() ? QStringLiteral("暂无特性记录") : pet.traitName);
    map.insert(QStringLiteral("traitDescription"), pet.traitDescription);
    map.insert(QStringLiteral("height"), pet.height.isEmpty() ? QStringLiteral("-") : pet.height);
    map.insert(QStringLiteral("weight"), pet.weight.isEmpty() ? QStringLiteral("-") : pet.weight);
    map.insert(QStringLiteral("distributionZh"), pet.distributionZh.isEmpty() ? QStringLiteral("暂无公开分布记录") : pet.distributionZh);
    map.insert(QStringLiteral("introductionZh"), pet.introductionZh.isEmpty() ? QStringLiteral("暂无图鉴介绍") : pet.introductionZh);
    map.insert(QStringLiteral("pageUrl"), pet.pageUrl);
    map.insert(QStringLiteral("sourceKey"), pet.sourceKey);

    QVariantList stats;
    stats << statRow(QStringLiteral("health"), QStringLiteral("精力"), pet.stats.health, m_maxStats.health)
          << statRow(QStringLiteral("physicalAttack"), QStringLiteral("物攻"), pet.stats.physicalAttack, m_maxStats.physicalAttack)
          << statRow(QStringLiteral("magicAttack"), QStringLiteral("魔攻"), pet.stats.magicAttack, m_maxStats.magicAttack)
          << statRow(QStringLiteral("physicalDefense"), QStringLiteral("物防"), pet.stats.physicalDefense, m_maxStats.physicalDefense)
          << statRow(QStringLiteral("magicDefense"), QStringLiteral("魔防"), pet.stats.magicDefense, m_maxStats.magicDefense)
          << statRow(QStringLiteral("speed"), QStringLiteral("速度"), pet.stats.speed, m_maxStats.speed)
          << statRow(QStringLiteral("baseStats"), QStringLiteral("总和"), pet.stats.baseStats, m_maxStats.baseStats);
    map.insert(QStringLiteral("stats"), stats);

    QVariantList chain;
    for (const auto *node : evolutionChain(pet)) {
        QVariantMap item;
        item.insert(QStringLiteral("key"), node->key);
        item.insert(QStringLiteral("name"), node->nameZh);
        item.insert(QStringLiteral("image"), DexRepository::imageSource(node->image));
        item.insert(QStringLiteral("current"), node->key == pet.key);
        chain.push_back(item);
    }
    map.insert(QStringLiteral("evolutionChain"), chain);
    map.insert(QStringLiteral("evolutionRequirement"), pet.evolution.next.isEmpty()
        ? QStringLiteral("当前没有下一形态记录。")
        : QStringLiteral("下一形态：%1").arg(evolutionRequirementLabel(pet)));

    QVariantList skills;
    auto appendSkill = [&](const QString &name, int level, const QString &group) {
        const auto *skill = skillByName(name);
        QVariantMap item;
        item.insert(QStringLiteral("name"), name);
        item.insert(QStringLiteral("level"), level);
        item.insert(QStringLiteral("group"), group);
        item.insert(QStringLiteral("energy"), skill && skill->energy >= 0 ? QVariant(skill->energy) : QVariant(QStringLiteral("-")));
        item.insert(QStringLiteral("power"), skill && skill->power >= 0 ? QVariant(skill->power) : QVariant(QStringLiteral("-")));
        item.insert(QStringLiteral("category"), skill ? skill->category : QString());
        item.insert(QStringLiteral("effect"), skill ? (skill->effect.isEmpty() ? skill->description : skill->effect) : QStringLiteral("暂无技能详情记录"));
        item.insert(QStringLiteral("attributeLabel"), skill ? attributeLabel(skill->attribute) : QString());
        const auto *attribute = skill ? attributeByKey(skill->attribute) : nullptr;
        item.insert(QStringLiteral("attributeColor"), attribute ? attribute->color : QStringLiteral("#e5e7eb"));
        item.insert(QStringLiteral("attributeTextColor"), attribute ? attribute->textColor : QStringLiteral("#111827"));
        skills.push_back(item);
    };

    for (const auto &skill : pet.skills)
        appendSkill(skill.name, skill.level, QStringLiteral("升级技能"));
    for (const auto &skill : pet.bloodlineSkills)
        appendSkill(skill, 0, QStringLiteral("血脉技能"));
    for (const auto &skill : pet.learnableSkillStones)
        appendSkill(skill, 0, QStringLiteral("可学技能石"));
    map.insert(QStringLiteral("skills"), skills);

    const auto defensive = defensiveProfile(pet);
    QVariantMap matchup;
    matchup.insert(QStringLiteral("weak"), matchupList(defensive.weak));
    matchup.insert(QStringLiteral("resistant"), matchupList(defensive.resistant));
    matchup.insert(QStringLiteral("neutral"), matchupList(defensive.neutral));
    matchup.insert(QStringLiteral("offense"), matchupList(offensiveCoverage(pet)));
    map.insert(QStringLiteral("matchup"), matchup);

    return map;
}

QVariantList DexRuntime::attributeOptions() const
{
    QVariantList list;
    for (const auto &attribute : m_bundle.attributes) {
        QVariantMap item;
        item.insert(QStringLiteral("key"), attribute.key);
        item.insert(QStringLiteral("label"), attribute.nameZh);
        item.insert(QStringLiteral("color"), attribute.color);
        item.insert(QStringLiteral("textColor"), attribute.textColor);
        list.push_back(item);
    }
    return list;
}

QVariantList DexRuntime::stageOptions() const
{
    return {
        QStringLiteral("全部"),
        QStringLiteral("Ⅰ阶"),
        QStringLiteral("Ⅱ阶"),
        QStringLiteral("最终形态"),
        QStringLiteral("独立形态"),
        QStringLiteral("未知"),
    };
}

QString DexRuntime::normalizedQuery(const QString &value) const
{
    return value.trimmed().toLower();
}

QString DexRuntime::chineseInitials(const QString &value) const
{
    static const QString initials = QStringLiteral("ABCDEFGHJKLMNOPQRSTWXYZ");
    static const QStringList boundaries {
        QStringLiteral("阿"), QStringLiteral("芭"), QStringLiteral("擦"), QStringLiteral("搭"),
        QStringLiteral("蛾"), QStringLiteral("发"), QStringLiteral("噶"), QStringLiteral("哈"),
        QStringLiteral("击"), QStringLiteral("喀"), QStringLiteral("垃"), QStringLiteral("妈"),
        QStringLiteral("拿"), QStringLiteral("哦"), QStringLiteral("啪"), QStringLiteral("期"),
        QStringLiteral("然"), QStringLiteral("撒"), QStringLiteral("塌"), QStringLiteral("挖"),
        QStringLiteral("昔"), QStringLiteral("压"), QStringLiteral("匝")
    };

    QString result;
    for (const auto character : value) {
        if (character.unicode() < 128 && character.isLetterOrNumber()) {
            result.append(character.toLower());
            continue;
        }

        const QString current(character);
        for (int i = boundaries.size() - 1; i >= 0; --i) {
            if (QString::localeAwareCompare(current, boundaries.at(i)) >= 0) {
                result.append(initials.at(i).toLower());
                break;
            }
        }
    }
    return result;
}

QString DexRuntime::searchText(const Pet &pet) const
{
    QStringList parts {
        pet.id,
        pet.key,
        pet.nameZh,
        chineseInitials(pet.nameZh),
        pet.nameEn,
        pet.distributionZh,
        pet.distributionEn,
        pet.formName,
        pet.petType,
        pet.traitName,
        pet.traitDescription,
        pet.dexTasks.join(QLatin1Char(' ')),
        pet.bloodlineSkills.join(QLatin1Char(' ')),
        pet.learnableSkillStones.join(QLatin1Char(' ')),
    };

    QStringList skillNames;
    for (const auto &skill : pet.skills)
        skillNames << skill.name;
    parts << skillNames.join(QLatin1Char(' '));

    QStringList attributeNames;
    for (const auto &attribute : pet.attributes)
        attributeNames << attributeLabel(attribute);
    parts << attributeNames.join(QLatin1Char(' '));

    return parts.join(QLatin1Char(' ')).toLower();
}

int DexRuntime::comparePets(const Pet &left, const Pet &right, const QString &sortKey) const
{
    QCollator collator;
    collator.setNumericMode(true);
    collator.setLocale(QLocale(QStringLiteral("zh_CN")));

    if (sortKey == QStringLiteral("name"))
        return collator.compare(left.nameZh, right.nameZh);
    if (sortKey == QStringLiteral("baseStats"))
        return right.stats.baseStats != left.stats.baseStats ? right.stats.baseStats - left.stats.baseStats : collator.compare(left.id, right.id);
    if (sortKey == QStringLiteral("speed"))
        return right.stats.speed != left.stats.speed ? right.stats.speed - left.stats.speed : collator.compare(left.id, right.id);
    if (sortKey == QStringLiteral("health"))
        return right.stats.health != left.stats.health ? right.stats.health - left.stats.health : collator.compare(left.id, right.id);

    return collator.compare(left.id, right.id);
}

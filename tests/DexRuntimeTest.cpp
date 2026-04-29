#include "../src/app/DexRuntime.h"
#include "../src/app/DexRepository.h"

#include <QtTest/QtTest>

class DexRuntimeTest : public QObject {
    Q_OBJECT

private slots:
    void repositoryParsesLargeSnapshotVersion();
    void filtersAndSortsPets();
    void defensiveProfileStacksMultipliers();
    void offensiveCoverageRemovesDuplicates();
};

namespace {

AttributeMeta attribute(
    const QString &key,
    const QString &label,
    QVector<QString> weakTo = {},
    QVector<QString> resistantTo = {},
    QVector<QString> hitsForDouble = {})
{
    AttributeMeta meta;
    meta.key = key;
    meta.nameZh = label;
    meta.nameEn = key;
    meta.color = QStringLiteral("#e5e7eb");
    meta.textColor = QStringLiteral("#111827");
    meta.defense.insert(QStringLiteral("2.0"), weakTo);
    meta.defense.insert(QStringLiteral("0.5"), resistantTo);
    meta.offense.insert(QStringLiteral("2.0"), hitsForDouble);
    meta.offense.insert(QStringLiteral("0.5"), {});
    return meta;
}

DexRuntime runtimeWithPet(const Pet &pet)
{
    DexDataBundle bundle;
    bundle.attributes = {
        attribute(QStringLiteral("x"), QStringLiteral("X")),
        attribute(QStringLiteral("y"), QStringLiteral("Y")),
        attribute(QStringLiteral("a"), QStringLiteral("A"), { QStringLiteral("x") }, {}, { QStringLiteral("x"), QStringLiteral("y") }),
        attribute(QStringLiteral("b"), QStringLiteral("B"), { QStringLiteral("x") }, {}, { QStringLiteral("x") }),
        attribute(QStringLiteral("c"), QStringLiteral("C"), { QStringLiteral("x") }),
        attribute(QStringLiteral("d"), QStringLiteral("D"), {}, { QStringLiteral("x") }),
        attribute(QStringLiteral("e"), QStringLiteral("E"), {}, { QStringLiteral("x") }),
        attribute(QStringLiteral("f"), QStringLiteral("F"), {}, { QStringLiteral("x") }),
    };
    bundle.pets = { pet };

    DexRuntime runtime;
    runtime.setBundle(std::move(bundle));
    return runtime;
}

Pet petWithAttributes(QVector<QString> attributes)
{
    Pet pet;
    pet.key = QStringLiteral("pet");
    pet.id = QStringLiteral("001");
    pet.nameZh = QStringLiteral("测试精灵");
    pet.attributes = std::move(attributes);
    return pet;
}

Pet namedPet(
    const QString &key,
    const QString &id,
    const QString &name,
    const QString &stage,
    int baseStats,
    int speed,
    QVector<QString> attributes,
    QVector<PetSkill> skills = {})
{
    Pet pet;
    pet.key = key;
    pet.id = id;
    pet.nameZh = name;
    pet.stage = stage;
    pet.stats.baseStats = baseStats;
    pet.stats.speed = speed;
    pet.distributionZh = key == QStringLiteral("fire") ? QStringLiteral("维苏威火山") : QString();
    pet.attributes = std::move(attributes);
    pet.skills = std::move(skills);
    return pet;
}

double multiplierFor(const QVector<MatchupEntry> &entries, const QString &key)
{
    for (const auto &entry : entries) {
        if (entry.key == key)
            return entry.multiplier;
    }
    return -1.0;
}

} // namespace

void DexRuntimeTest::repositoryParsesLargeSnapshotVersion()
{
    const QByteArray payload = R"({
        "snapshot": {
            "importedAt": "2026-04-27T16:32:19.847Z",
            "petCount": 1,
            "skillCount": 1,
            "attributeCount": 1,
            "updatePackageVersion": 1777307539847,
            "supplementPetCount": 999
        },
        "pets": [
            { "key": "p1", "id": "001", "nameZh": "Pet", "attributes": ["fire"], "stats": {} }
        ],
        "attributes": [
            {
                "key": "fire",
                "nameZh": "Fire",
                "color": "#e5e7eb",
                "textColor": "#111827",
                "offense": { "0.5": [], "2.0": [] },
                "defense": { "0.5": [], "2.0": [] }
            }
        ],
        "skills": [
            { "name": "Skill", "attribute": "fire" }
        ],
        "supplement": { "pets": [], "skills": [] }
    })";

    QString error;
    const DexDataBundle bundle = DexRepository::parseBundle(payload, &error);

    QVERIFY2(error.isEmpty(), qPrintable(error));
    QCOMPARE(bundle.pets.size(), 1);
    QCOMPARE(bundle.attributes.size(), 1);
    QCOMPARE(bundle.snapshot.updatePackageVersion, qint64(1777307539847));
}

void DexRuntimeTest::filtersAndSortsPets()
{
    DexDataBundle bundle;
    bundle.attributes = {
        attribute(QStringLiteral("fire"), QStringLiteral("火")),
        attribute(QStringLiteral("water"), QStringLiteral("水")),
    };
    bundle.pets = {
        namedPet(
            QStringLiteral("fire"),
            QStringLiteral("002"),
            QStringLiteral("火神"),
            QStringLiteral("最终形态"),
            600,
            90,
            { QStringLiteral("fire") },
            { { QStringLiteral("火焰喷发"), 50 } }),
        namedPet(
            QStringLiteral("water"),
            QStringLiteral("001"),
            QStringLiteral("水蓝蓝"),
            QStringLiteral("Ⅰ阶"),
            320,
            45,
            { QStringLiteral("water") }),
    };

    DexRuntime runtime;
    runtime.setBundle(std::move(bundle));

    auto result = runtime.filterPets(
        QStringLiteral("火焰"),
        {},
        QStringLiteral("any"),
        QStringLiteral("全部"),
        false,
        QStringLiteral("id"));
    QCOMPARE(result.size(), 1);
    QCOMPARE(runtime.bundle().pets.at(result.first()).key, QStringLiteral("fire"));

    result = runtime.filterPets(
        QString(),
        { QStringLiteral("water") },
        QStringLiteral("any"),
        QStringLiteral("Ⅰ阶"),
        false,
        QStringLiteral("id"));
    QCOMPARE(result.size(), 1);
    QCOMPARE(runtime.bundle().pets.at(result.first()).key, QStringLiteral("water"));

    result = runtime.filterPets(
        QString(),
        {},
        QStringLiteral("any"),
        QStringLiteral("全部"),
        true,
        QStringLiteral("baseStats"));
    QCOMPARE(result.size(), 1);
    QCOMPARE(runtime.bundle().pets.at(result.first()).key, QStringLiteral("fire"));

    result = runtime.filterPets(
        QString(),
        {},
        QStringLiteral("any"),
        QStringLiteral("全部"),
        false,
        QStringLiteral("baseStats"));
    QCOMPARE(result.size(), 2);
    QCOMPARE(runtime.bundle().pets.at(result.first()).key, QStringLiteral("fire"));
}

void DexRuntimeTest::defensiveProfileStacksMultipliers()
{
    {
        const Pet pet = petWithAttributes({ QStringLiteral("a"), QStringLiteral("b") });
        const DexRuntime runtime = runtimeWithPet(pet);
        const DefensiveProfile profile = runtime.defensiveProfile(pet);
        QCOMPARE(multiplierFor(profile.weak, QStringLiteral("x")), 4.0);
    }

    {
        const Pet pet = petWithAttributes({ QStringLiteral("c"), QStringLiteral("d") });
        const DexRuntime runtime = runtimeWithPet(pet);
        const DefensiveProfile profile = runtime.defensiveProfile(pet);
        QCOMPARE(multiplierFor(profile.neutral, QStringLiteral("x")), 1.0);
    }

    {
        const Pet pet = petWithAttributes({ QStringLiteral("e"), QStringLiteral("f") });
        const DexRuntime runtime = runtimeWithPet(pet);
        const DefensiveProfile profile = runtime.defensiveProfile(pet);
        QCOMPARE(multiplierFor(profile.resistant, QStringLiteral("x")), 0.25);
    }
}

void DexRuntimeTest::offensiveCoverageRemovesDuplicates()
{
    const Pet pet = petWithAttributes({ QStringLiteral("a"), QStringLiteral("b") });
    const DexRuntime runtime = runtimeWithPet(pet);
    const auto coverage = runtime.offensiveCoverage(pet);

    QSet<QString> keys;
    for (const auto &entry : coverage)
        keys.insert(entry.key);

    QCOMPARE(keys.size(), 2);
    QVERIFY(keys.contains(QStringLiteral("x")));
    QVERIFY(keys.contains(QStringLiteral("y")));
}

QTEST_MAIN(DexRuntimeTest)

#include "DexRuntimeTest.moc"

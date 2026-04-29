#include "DexRepository.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QStandardPaths>
#include <QUrl>

namespace {

QString valueString(const QJsonObject &object, const QString &key)
{
    const auto value = object.value(key);
    return value.isString() ? value.toString() : QString();
}

int valueInt(const QJsonObject &object, const QString &key, int fallback = 0)
{
    const auto value = object.value(key);
    return value.isDouble() ? value.toInt() : fallback;
}

qint64 valueInteger(const QJsonObject &object, const QString &key, qint64 fallback = 0)
{
    const auto value = object.value(key);
    return value.isDouble() ? static_cast<qint64>(value.toDouble()) : fallback;
}

QVector<QString> stringList(const QJsonValue &value)
{
    QVector<QString> result;
    if (!value.isArray())
        return result;

    const auto array = value.toArray();
    result.reserve(array.size());
    for (const auto &item : array) {
        if (item.isString())
            result.push_back(item.toString());
    }
    return result;
}

QHash<QString, QVector<QString>> multiplierTable(const QJsonObject &object)
{
    QHash<QString, QVector<QString>> result;
    result.insert(QStringLiteral("0.5"), stringList(object.value(QStringLiteral("0.5"))));
    result.insert(QStringLiteral("2.0"), stringList(object.value(QStringLiteral("2.0"))));
    return result;
}

PetStats parseStats(const QJsonObject &object)
{
    return {
        valueInt(object, QStringLiteral("health")),
        valueInt(object, QStringLiteral("physicalAttack")),
        valueInt(object, QStringLiteral("magicAttack")),
        valueInt(object, QStringLiteral("physicalDefense")),
        valueInt(object, QStringLiteral("magicDefense")),
        valueInt(object, QStringLiteral("speed")),
        valueInt(object, QStringLiteral("baseStats")),
    };
}

EvolutionInfo parseEvolution(const QJsonObject &object)
{
    return {
        valueString(object, QStringLiteral("previous")),
        valueString(object, QStringLiteral("next")),
        valueInt(object, QStringLiteral("level")),
        valueString(object, QStringLiteral("condition")),
    };
}

QVector<PetSkill> parsePetSkills(const QJsonValue &value)
{
    QVector<PetSkill> result;
    if (!value.isArray())
        return result;

    const auto array = value.toArray();
    result.reserve(array.size());
    for (const auto &item : array) {
        const auto object = item.toObject();
        result.push_back({
            valueString(object, QStringLiteral("name")),
            valueInt(object, QStringLiteral("level")),
        });
    }
    return result;
}

DataSnapshot parseSnapshot(const QJsonObject &object)
{
    return {
        valueString(object, QStringLiteral("importedAt")),
        valueInt(object, QStringLiteral("petCount")),
        valueInt(object, QStringLiteral("detailCount")),
        valueInt(object, QStringLiteral("evolutionDetailCount")),
        valueInt(object, QStringLiteral("skillCount")),
        valueInt(object, QStringLiteral("skillDetailCount")),
        valueInt(object, QStringLiteral("attributeCount")),
        valueString(object, QStringLiteral("sourceUrl")),
        valueString(object, QStringLiteral("skillSourceUrl")),
        valueInteger(object, QStringLiteral("updatePackageVersion")),
    };
}

Pet parsePet(const QJsonObject &object)
{
    return {
        valueString(object, QStringLiteral("key")),
        valueString(object, QStringLiteral("title")),
        valueString(object, QStringLiteral("href")),
        valueString(object, QStringLiteral("id")),
        valueString(object, QStringLiteral("nameZh")),
        valueString(object, QStringLiteral("nameEn")),
        valueString(object, QStringLiteral("image")),
        stringList(object.value(QStringLiteral("attributes"))),
        valueString(object, QStringLiteral("formName")),
        valueString(object, QStringLiteral("initialName")),
        valueString(object, QStringLiteral("petType")),
        object.value(QStringLiteral("hasShiny")).toBool(false),
        valueString(object, QStringLiteral("introductionZh")),
        valueString(object, QStringLiteral("introductionEn")),
        valueString(object, QStringLiteral("traitName")),
        valueString(object, QStringLiteral("traitDescription")),
        valueString(object, QStringLiteral("height")),
        valueString(object, QStringLiteral("weight")),
        valueString(object, QStringLiteral("distributionZh")),
        valueString(object, QStringLiteral("distributionEn")),
        parseEvolution(object.value(QStringLiteral("evolution")).toObject()),
        valueString(object, QStringLiteral("stage")),
        parseStats(object.value(QStringLiteral("stats")).toObject()),
        stringList(object.value(QStringLiteral("dexTasks"))),
        stringList(object.value(QStringLiteral("taskSkillStones"))),
        parsePetSkills(object.value(QStringLiteral("skills"))),
        stringList(object.value(QStringLiteral("bloodlineSkills"))),
        stringList(object.value(QStringLiteral("learnableSkillStones"))),
        valueString(object, QStringLiteral("updateVersion")),
        valueString(object, QStringLiteral("pageUrl")),
        valueString(object, QStringLiteral("sourceKey")),
    };
}

AttributeMeta parseAttribute(const QJsonObject &object)
{
    return {
        valueString(object, QStringLiteral("key")),
        valueString(object, QStringLiteral("nameZh")),
        valueString(object, QStringLiteral("nameEn")),
        valueString(object, QStringLiteral("color")),
        valueString(object, QStringLiteral("textColor")),
        valueString(object, QStringLiteral("descriptionZh")),
        multiplierTable(object.value(QStringLiteral("offense")).toObject()),
        multiplierTable(object.value(QStringLiteral("defense")).toObject()),
    };
}

SkillInfo parseSkill(const QJsonObject &object)
{
    return {
        valueString(object, QStringLiteral("name")),
        valueString(object, QStringLiteral("attribute")),
        valueString(object, QStringLiteral("category")),
        object.value(QStringLiteral("energy")).isDouble()
            ? object.value(QStringLiteral("energy")).toInt()
            : -1,
        object.value(QStringLiteral("power")).isDouble()
            ? object.value(QStringLiteral("power")).toInt()
            : -1,
        valueString(object, QStringLiteral("effect")),
        valueString(object, QStringLiteral("description")),
        valueString(object, QStringLiteral("version")),
        valueString(object, QStringLiteral("pageUrl")),
    };
}

QString resourcesRoot()
{
    const QString appRoot = QCoreApplication::applicationDirPath() + QStringLiteral("/resources");
    if (QDir(appRoot).exists())
        return appRoot;

    const QString cwdRoot = QDir::currentPath() + QStringLiteral("/resources");
    if (QDir(cwdRoot).exists())
        return cwdRoot;

    const QString embeddedRoot = QStringLiteral(":/resources");
    if (QDir(embeddedRoot).exists())
        return embeddedRoot;

    return QDir::currentPath() + QStringLiteral("/resources");
}

} // namespace

DexDataBundle DexRepository::loadDefaultBundle(QString *errorMessage)
{
    return loadBundleFromFile(defaultBundlePath(), errorMessage);
}

DexDataBundle DexRepository::loadPreferredBundle(QString *sourceLabel, QString *errorMessage)
{
    const QString userPath = userBundlePath();
    if (QFile::exists(userPath)) {
        QString userError;
        DexDataBundle userBundle = loadBundleFromFile(userPath, &userError);
        if (!userBundle.pets.isEmpty() && !userBundle.attributes.isEmpty()) {
            if (sourceLabel)
                *sourceLabel = QStringLiteral("在线更新数据");
            return userBundle;
        }
    }

    if (sourceLabel)
        *sourceLabel = QStringLiteral("内置离线数据");
    return loadDefaultBundle(errorMessage);
}

DexDataBundle DexRepository::loadBundleFromFile(const QString &filePath, QString *errorMessage)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        if (errorMessage)
            *errorMessage = QStringLiteral("Unable to open data bundle: %1").arg(filePath);
        return {};
    }

    return parseBundle(file.readAll(), errorMessage);
}

DexDataBundle DexRepository::parseBundle(const QByteArray &payload, QString *errorMessage)
{
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(payload, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        if (errorMessage)
            *errorMessage = QStringLiteral("Invalid JSON bundle: %1").arg(parseError.errorString());
        return {};
    }

    const QJsonObject root = document.object();
    DexDataBundle bundle;
    bundle.snapshot = parseSnapshot(root.value(QStringLiteral("snapshot")).toObject());

    const auto pets = root.value(QStringLiteral("pets")).toArray();
    bundle.pets.reserve(pets.size());
    for (const auto &item : pets)
        bundle.pets.push_back(parsePet(item.toObject()));

    const auto attributes = root.value(QStringLiteral("attributes")).toArray();
    bundle.attributes.reserve(attributes.size());
    for (const auto &item : attributes)
        bundle.attributes.push_back(parseAttribute(item.toObject()));

    const auto skills = root.value(QStringLiteral("skills")).toArray();
    bundle.skills.reserve(skills.size());
    for (const auto &item : skills)
        bundle.skills.push_back(parseSkill(item.toObject()));

    if (bundle.pets.isEmpty() || bundle.attributes.isEmpty()) {
        if (errorMessage)
            *errorMessage = QStringLiteral("Data bundle is missing pets or attributes");
    }

    return bundle;
}

QString DexRepository::defaultBundlePath()
{
    const QString localPath = resourcesRoot() + QStringLiteral("/data/dex-bundle.json");
    if (QFile::exists(localPath))
        return localPath;

    return QStringLiteral(":/resources/data/dex-bundle.json");
}

QString DexRepository::userBundlePath()
{
    const QString root = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return root + QStringLiteral("/dex-bundle.json");
}

bool DexRepository::saveUserBundle(const QByteArray &payload, QString *errorMessage)
{
    QString parseError;
    const DexDataBundle parsed = parseBundle(payload, &parseError);
    if (parsed.pets.isEmpty() || parsed.attributes.isEmpty()) {
        if (errorMessage)
            *errorMessage = parseError.isEmpty() ? QStringLiteral("数据包格式无效") : parseError;
        return false;
    }

    QByteArray payloadToWrite = payload;
    QJsonParseError jsonError;
    const QJsonDocument document = QJsonDocument::fromJson(payload, &jsonError);
    if (jsonError.error == QJsonParseError::NoError && document.isObject()) {
        QJsonObject root = document.object();
        root.remove(QStringLiteral("supplement"));

        QJsonObject snapshot = root.value(QStringLiteral("snapshot")).toObject();
        snapshot.remove(QStringLiteral("supplementPetCount"));
        snapshot.remove(QStringLiteral("supplementSkillCount"));
        root.insert(QStringLiteral("snapshot"), snapshot);

        payloadToWrite = QJsonDocument(root).toJson(QJsonDocument::Indented);
    }

    const QString path = userBundlePath();
    QDir().mkpath(QFileInfo(path).absolutePath());
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        if (errorMessage)
            *errorMessage = QStringLiteral("无法写入用户数据：%1").arg(path);
        return false;
    }

    file.write(payloadToWrite);
    if (!file.commit()) {
        if (errorMessage)
            *errorMessage = QStringLiteral("用户数据保存失败");
        return false;
    }

    return true;
}

void DexRepository::clearUserBundle()
{
    QFile::remove(userBundlePath());
}

QString DexRepository::imageSource(const QString &bundleImagePath)
{
    if (bundleImagePath.isEmpty())
        return {};

    if (bundleImagePath.startsWith(QStringLiteral("http://"))
        || bundleImagePath.startsWith(QStringLiteral("https://"))
        || bundleImagePath.startsWith(QStringLiteral("file:"))
        || bundleImagePath.startsWith(QStringLiteral("qrc:"))) {
        return bundleImagePath;
    }

    QString relative = bundleImagePath;
    while (relative.startsWith(QLatin1Char('/')))
        relative.remove(0, 1);

    const QString filePath = resourcesRoot() + QLatin1Char('/') + relative;
    if (QFile::exists(filePath))
        return QUrl::fromLocalFile(QDir::cleanPath(filePath)).toString();

    const QString embeddedPath = QStringLiteral(":/resources/%1").arg(relative);
    if (QFile::exists(embeddedPath))
        return QStringLiteral("qrc%1").arg(embeddedPath);

    return QUrl::fromLocalFile(QDir::cleanPath(filePath)).toString();
}

#include "DexController.h"

#include "DexRepository.h"

#include <QDateTime>
#include <QLocale>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QScopeGuard>
#include <QSettings>
#include <QUrl>

namespace {

constexpr qsizetype MaxRecentPets = 8;

const QUrl kRemoteBundleUrl(QStringLiteral(
    "https://raw.githubusercontent.com/MocheNYC/RocoDex/main/public/data/dex-bundle.json"));

QStringList sortedKeys(const QSet<QString> &keys)
{
    QStringList list(keys.cbegin(), keys.cend());
    list.sort();
    return list;
}

QDateTime snapshotDateTime(const QString &value)
{
    QDateTime dateTime = QDateTime::fromString(value, Qt::ISODateWithMs);
    if (!dateTime.isValid())
        dateTime = QDateTime::fromString(value, Qt::ISODate);
    return dateTime;
}

} // namespace

DexController::DexController(QObject *parent)
    : QObject(parent)
{
    const QSettings settings;
    for (const auto &key : settings.value(QStringLiteral("favorites")).toStringList())
        m_favorites.insert(key);
    m_recent = settings.value(QStringLiteral("recent")).toStringList();

    loadData();
}

PetListModel *DexController::petModel()
{
    return &m_petModel;
}

QVariantMap DexController::selectedPet() const
{
    const Pet *pet = m_runtime.petByKey(m_selectedKey);
    if (!pet)
        return {};

    auto detail = m_runtime.petDetail(*pet, m_favorites.contains(pet->key));
    const int row = m_petModel.rowOfKey(pet->key);
    detail.insert(QStringLiteral("row"), row);
    detail.insert(QStringLiteral("hasPrevious"), row > 0);
    detail.insert(QStringLiteral("hasNext"), row >= 0 && row < m_petModel.count() - 1);
    return detail;
}

QVariantList DexController::attributes() const
{
    return m_runtime.attributeOptions();
}

QVariantList DexController::stages() const
{
    return m_runtime.stageOptions();
}

QString DexController::query() const
{
    return m_query;
}

void DexController::setQuery(const QString &query)
{
    if (m_query == query)
        return;
    m_query = query;
    applyFilters();
}

QString DexController::stage() const
{
    return m_stage;
}

void DexController::setStage(const QString &stage)
{
    if (m_stage == stage)
        return;
    m_stage = stage;
    applyFilters();
}

QString DexController::sortKey() const
{
    return m_sortKey;
}

void DexController::setSortKey(const QString &sortKey)
{
    if (m_sortKey == sortKey)
        return;
    m_sortKey = sortKey;
    applyFilters();
}

QString DexController::attributeMode() const
{
    return m_attributeMode;
}

void DexController::setAttributeMode(const QString &attributeMode)
{
    if (m_attributeMode == attributeMode)
        return;
    m_attributeMode = attributeMode;
    applyFilters();
}

QStringList DexController::selectedAttributes() const
{
    return sortedKeys(m_selectedAttributes);
}

bool DexController::onlyDistributed() const
{
    return m_onlyDistributed;
}

void DexController::setOnlyDistributed(bool onlyDistributed)
{
    if (m_onlyDistributed == onlyDistributed)
        return;
    m_onlyDistributed = onlyDistributed;
    applyFilters();
}

int DexController::resultCount() const
{
    return m_petModel.count();
}

int DexController::petCount() const
{
    return m_runtime.bundle().pets.size();
}

int DexController::skillCount() const
{
    return m_runtime.bundle().skills.size();
}

QString DexController::dataStatus() const
{
    return m_dataStatus;
}

QString DexController::snapshotTime() const
{
    const QString importedAt = m_runtime.bundle().snapshot.importedAt;
    if (importedAt.isEmpty())
        return QStringLiteral("未知");

    const QDateTime dateTime = snapshotDateTime(importedAt);
    if (!dateTime.isValid())
        return importedAt;

    const QLocale locale(QLocale::Chinese, QLocale::China);
    return locale.toString(dateTime.toLocalTime(), QStringLiteral("yyyy年MM月dd日 HH:mm"));
}

QString DexController::updateMessage() const
{
    return m_updateMessage;
}

bool DexController::updateBusy() const
{
    return m_updateBusy;
}

QVariantList DexController::favoritePets() const
{
    QVariantList pets;
    for (const auto &key : favoriteKeys()) {
        const auto *pet = m_runtime.petByKey(key);
        if (pet)
            pets.push_back(m_runtime.petSummary(*pet, true));
    }
    return pets;
}

QVariantList DexController::recentPets() const
{
    QVariantList pets;
    for (const auto &key : m_recent) {
        const auto *pet = m_runtime.petByKey(key);
        if (pet)
            pets.push_back(m_runtime.petSummary(*pet, m_favorites.contains(key)));
    }
    return pets;
}

void DexController::selectPet(const QString &key)
{
    if (key.isEmpty() || m_selectedKey == key)
        return;

    if (!m_runtime.petByKey(key))
        return;

    m_selectedKey = key;
    rememberRecent(key);
    emit selectedPetChanged();
}

void DexController::selectRow(int row)
{
    selectPet(m_petModel.keyAt(row));
}

void DexController::selectPrevious()
{
    const int row = m_petModel.rowOfKey(m_selectedKey);
    if (row > 0)
        selectRow(row - 1);
}

void DexController::selectNext()
{
    const int row = m_petModel.rowOfKey(m_selectedKey);
    if (row >= 0 && row < m_petModel.count() - 1)
        selectRow(row + 1);
}

void DexController::clearFilters()
{
    m_query.clear();
    m_selectedAttributes.clear();
    m_attributeMode = QStringLiteral("any");
    m_stage = QStringLiteral("全部");
    m_onlyDistributed = false;
    m_sortKey = QStringLiteral("id");
    applyFilters();
}

void DexController::toggleAttribute(const QString &key)
{
    if (m_selectedAttributes.contains(key))
        m_selectedAttributes.remove(key);
    else
        m_selectedAttributes.insert(key);
    applyFilters();
}

void DexController::toggleFavorite()
{
    if (m_selectedKey.isEmpty())
        return;

    if (m_favorites.contains(m_selectedKey))
        m_favorites.remove(m_selectedKey);
    else
        m_favorites.insert(m_selectedKey);

    saveFavorites();
    m_petModel.setFavorites(m_favorites);
    emit selectedPetChanged();
    emit quickPetsChanged();
}

void DexController::checkForUpdates()
{
    if (m_updateBusy)
        return;

    setUpdateBusy(true);
    setUpdateMessage(QStringLiteral("正在检查在线数据..."));

    const QUrl url(QStringLiteral("%1?t=%2").arg(
        kRemoteBundleUrl.toString(),
        QString::number(QDateTime::currentMSecsSinceEpoch())));
    QNetworkRequest request(url);
    request.setRawHeader(QByteArrayLiteral("Accept"), QByteArrayLiteral("application/json"));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setTransferTimeout(15000);

    QNetworkReply *reply = m_network.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        const auto guard = qScopeGuard([&] {
            reply->deleteLater();
            setUpdateBusy(false);
        });

        const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (reply->error() != QNetworkReply::NoError || httpStatus >= 400) {
            const QString detail = reply->error() == QNetworkReply::NoError
                ? QStringLiteral("HTTP %1").arg(httpStatus)
                : reply->errorString();
            setUpdateMessage(QStringLiteral("检查失败：%1").arg(detail));
            return;
        }

        const QByteArray payload = reply->readAll();
        QString parseError;
        DexDataBundle remoteBundle = DexRepository::parseBundle(payload, &parseError);
        if (remoteBundle.pets.isEmpty() || remoteBundle.attributes.isEmpty()) {
            setUpdateMessage(QStringLiteral("检查失败：%1").arg(
                parseError.isEmpty() ? QStringLiteral("远端数据包格式无效") : parseError));
            return;
        }

        if (!isRemoteNewer(remoteBundle)) {
            setUpdateMessage(QStringLiteral("当前已是最新数据"));
            return;
        }

        QString saveError;
        if (!DexRepository::saveUserBundle(payload, &saveError)) {
            setUpdateMessage(QStringLiteral("更新失败：%1").arg(saveError));
            return;
        }

        const QString previousSelection = m_selectedKey;
        applyBundle(std::move(remoteBundle), QStringLiteral("在线更新数据"), previousSelection);
        setUpdateMessage(QStringLiteral("已更新到 %1 的在线数据").arg(snapshotTime()));
    });
}

void DexController::resetToEmbeddedData()
{
    DexRepository::clearUserBundle();

    QString error;
    DexDataBundle bundle = DexRepository::loadDefaultBundle(&error);
    if (bundle.pets.isEmpty() || bundle.attributes.isEmpty()) {
        setUpdateMessage(QStringLiteral("恢复失败：%1").arg(error));
        return;
    }

    const QString previousSelection = m_selectedKey;
    applyBundle(std::move(bundle), QStringLiteral("内置离线数据"), previousSelection);
    setUpdateMessage(QStringLiteral("已恢复内置离线数据"));
}

void DexController::loadData()
{
    QString sourceLabel;
    QString error;
    DexDataBundle bundle = DexRepository::loadPreferredBundle(&sourceLabel, &error);
    if (bundle.pets.isEmpty() || bundle.attributes.isEmpty()) {
        m_dataStatus = QStringLiteral("数据加载失败：%1").arg(error);
        setUpdateMessage(m_dataStatus);
        emit dataChanged();
        return;
    }

    applyBundle(std::move(bundle), sourceLabel);
    setUpdateMessage(QStringLiteral("已加载%1").arg(sourceLabel));
}

void DexController::applyBundle(DexDataBundle bundle, const QString &sourceLabel, const QString &preferredKey)
{
    QString nextKey = preferredKey.isEmpty() ? m_selectedKey : preferredKey;

    m_runtime.setBundle(std::move(bundle));
    m_petModel.setRuntime(&m_runtime);
    m_petModel.setFavorites(m_favorites);
    m_dataStatus = sourceLabel;

    if (nextKey.isEmpty() || !m_runtime.petByKey(nextKey))
        nextKey = m_runtime.bundle().pets.isEmpty() ? QString() : m_runtime.bundle().pets.first().key;
    m_selectedKey = nextKey;

    applyFilters();
    emit dataChanged();
    emit quickPetsChanged();
}

void DexController::applyFilters()
{
    m_petModel.setFilters(m_query, m_selectedAttributes, m_attributeMode, m_stage, m_onlyDistributed, m_sortKey);

    if (m_petModel.count() > 0 && m_petModel.rowOfKey(m_selectedKey) < 0)
        m_selectedKey = m_petModel.keyAt(0);

    emit filtersChanged();
    emit resultCountChanged();
    emit selectedPetChanged();
}

void DexController::rememberRecent(const QString &key)
{
    m_recent.removeAll(key);
    m_recent.prepend(key);
    while (m_recent.size() > MaxRecentPets)
        m_recent.removeLast();

    QSettings settings;
    settings.setValue(QStringLiteral("recent"), m_recent);
    emit quickPetsChanged();
}

QStringList DexController::favoriteKeys() const
{
    return sortedKeys(m_favorites);
}

void DexController::saveFavorites() const
{
    QSettings settings;
    settings.setValue(QStringLiteral("favorites"), favoriteKeys());
}

void DexController::setUpdateMessage(const QString &message)
{
    if (m_updateMessage == message)
        return;
    m_updateMessage = message;
    emit updateStateChanged();
}

void DexController::setUpdateBusy(bool busy)
{
    if (m_updateBusy == busy)
        return;
    m_updateBusy = busy;
    emit updateStateChanged();
}

bool DexController::isRemoteNewer(const DexDataBundle &remote) const
{
    const auto &current = m_runtime.bundle();
    if (remote.snapshot.updatePackageVersion != current.snapshot.updatePackageVersion)
        return remote.snapshot.updatePackageVersion > current.snapshot.updatePackageVersion;

    const QDateTime currentTime = snapshotDateTime(current.snapshot.importedAt);
    const QDateTime remoteTime = snapshotDateTime(remote.snapshot.importedAt);
    if (currentTime.isValid() && remoteTime.isValid() && remoteTime != currentTime)
        return remoteTime > currentTime;

    return dataSizeScore(remote) > dataSizeScore(current);
}

qint64 DexController::dataSizeScore(const DexDataBundle &bundle) const
{
    return static_cast<qint64>(bundle.pets.size()) * 1000000
        + static_cast<qint64>(bundle.skills.size()) * 1000
        + bundle.attributes.size();
}

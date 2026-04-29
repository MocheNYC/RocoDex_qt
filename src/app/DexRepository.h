#pragma once

#include "../models/DexTypes.h"

#include <QString>

class DexRepository {
public:
    static DexDataBundle loadPreferredBundle(QString *sourceLabel = nullptr, QString *errorMessage = nullptr);
    static DexDataBundle loadDefaultBundle(QString *errorMessage = nullptr);
    static DexDataBundle loadBundleFromFile(const QString &filePath, QString *errorMessage = nullptr);
    static DexDataBundle parseBundle(const QByteArray &payload, QString *errorMessage = nullptr);

    static QString defaultBundlePath();
    static QString userBundlePath();
    static bool saveUserBundle(const QByteArray &payload, QString *errorMessage = nullptr);
    static void clearUserBundle();
    static QString imageSource(const QString &bundleImagePath);
};

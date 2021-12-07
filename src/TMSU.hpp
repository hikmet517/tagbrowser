#pragma once
#include <QString>
#include <QList>

#include <optional>

using std::optional;

namespace TMSU{
    optional<QString> getDatabasePath(const QString& dirpath);
    QList<QList<QString>> getTags(const QString& dbPath);
    int addTag(const QString& tag, const QStringList& files);
    int removeTag(const QString& tag, const QStringList& files);
    int query(const QString &query, const QString &wdir, QStringList &output);
    int untagged(const QString &wdir, QStringList &output);
};

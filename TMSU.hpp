#pragma once
#include <QString>
#include <QList>


namespace TMSU{
    QString getDatabasePath(const QString& dirpath);
    QList<QList<QString>> getTags(const QString& dbPath);
    int addTag(const QString& tag, const QStringList& files);
    int removeTag(const QString& tag, const QStringList& files);
    int query(const QStringList &args, const QString &wdir, QStringList &output);
    int untagged(QString &wdir, QStringList &output);
};

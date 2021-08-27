#pragma once
#include <QVariant>
#include <QHash>

#include <sqlite3.h>


class ThumbnailJobSQLite
{
public:
    ThumbnailJobSQLite();
    QVariant getThumbnail(const QString &filepath);
    bool putThumbnailToDb(const QString &filepath, const QPixmap &pm);
    ~ThumbnailJobSQLite();
private:
    bool createDb(const QString &dbPath);
    sqlite3* openDb(const QString &dbPath);
    QString getDbPath(const QString &filepath);
    QVariant getThumbnailFromDb(sqlite3 *db, const QString &filepath);
    QString cacheLocation;
    QHash<QString, sqlite3 *> dbMap;  // stores opened dbs
};

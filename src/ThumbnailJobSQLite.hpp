#pragma once
#include <QHash>
#include <QPixmap>

#include <optional>

#include <sqlite3.h>

using std::optional;

class ThumbnailJobSQLite
{
public:
    ThumbnailJobSQLite();
    optional<QPixmap> getThumbnail(const QString &filepath);
    bool putThumbnailToDb(const QString &filepath, const QPixmap &pm);
    ~ThumbnailJobSQLite();
private:
    bool createDb(const QString &dbPath);
    sqlite3* openDb(const QString &dbPath);
    QString getDbPath(const QString &filepath);
    optional<QPixmap> getThumbnailFromDb(sqlite3 *db, const QString &filepath);
    QString cacheLocation;
    QHash<QString, sqlite3 *> dbMap;  // stores opened dbs
};

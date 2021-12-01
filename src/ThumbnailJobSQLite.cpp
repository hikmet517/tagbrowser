#include "ThumbnailJobSQLite.hpp"

#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QStandardPaths>
#include <QBuffer>
#include <QCryptographicHash>
#include <QDebug>

#include <iostream>
#include <vector>


ThumbnailJobSQLite::ThumbnailJobSQLite()
{
    qDebug() << "ThumbnailJobSQLite::ThumbnailJobSQLite()";
    cacheLocation = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if(cacheLocation == "") {
        QChar sep = QDir::separator();
        cacheLocation = QDir::homePath() + sep + ".cache" + sep + QCoreApplication::organizationName()
            + sep + QCoreApplication::applicationName();
    }
    QDir(cacheLocation).mkpath(".");
}


optional<QPixmap>
ThumbnailJobSQLite::getThumbnail(const QString &filepath)
{
    // qDebug() << "ThumbnailJobSQLite::getThumbnail()";
    QString dbPath = getDbPath(filepath);

    if ( !QFileInfo(dbPath).exists() ) {
        // db file doesn't exist, create db
        createDb(dbPath);
        return {};
    }
    else {
        // db file exists, query db
        sqlite3 *db = NULL;
        if( dbMap.contains(dbPath) ) {
            // db is opened already
            db = dbMap[dbPath];
        }
        else {
            // open db first
            db = openDb(dbPath);
        }
        // and query
        return getThumbnailFromDb(db, filepath);
    }
    return {};
}


QString
ThumbnailJobSQLite::getDbPath(const QString &filepath)
{
    QString parentDir = QFileInfo(filepath).dir().path();
    QByteArray percent = parentDir.toUtf8().toPercentEncoding({'/', ':'});
    QByteArray hex = QCryptographicHash::hash(percent, QCryptographicHash::Md5).toHex();

    return cacheLocation + QDir::separator() + hex + ".db";
}


sqlite3 *
ThumbnailJobSQLite::openDb(const QString &dbPath)
{
    sqlite3 *db;
    int rc = sqlite3_open_v2(dbPath.toStdString().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if(rc){
        std::cerr << "openDb() Error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return NULL;
    }
    dbMap[dbPath] = db;
    return db;
}


bool
ThumbnailJobSQLite::createDb(const QString &dbPath)
{
    qDebug() << "ThumbnailJobSQLite::createDb()";
    sqlite3 *db = openDb(dbPath);

    char sql[] = "CREATE TABLE THUMBNAILS (   \
                  file TEXT NOT NULL,         \
                  thumbnail BLOB NOT NULL,    \
                  PRIMARY KEY(file) );        ";

    char *zErrMsg = 0;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
    if(rc) {
        std::cerr << "createDb() Error in sqlite3_exec: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        return false;
    }
    return true;
}


optional<QPixmap>
ThumbnailJobSQLite::getThumbnailFromDb(sqlite3 *db, const QString &filepath)
{
    // qDebug() << "ThumbnailJobSQLite::getThumbnailFromDb()" << filepath;
    sqlite3_stmt *ppStmt;
    char sql[] = "SELECT thumbnail FROM THUMBNAILS WHERE file=?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &ppStmt, NULL);
    if(rc != SQLITE_OK || ppStmt == NULL) {
        std::cerr << "getThumbnailFromDb Error in sqlite3_prepare_v2, returned: " << rc << std::endl;
        return {};
    }

    rc = sqlite3_bind_text(ppStmt, 1, QFileInfo(filepath).fileName().toUtf8().data(), -1, SQLITE_STATIC);
    if(rc != SQLITE_OK) {
        std::cerr << "getThumbnailFromDb Error in sqlite3_bind_text, returned: " << rc << std::endl;
        return {};
    }

    std::vector<unsigned char> buffer;
    while ( true ) {
        int rc = sqlite3_step(ppStmt);
        if (rc == SQLITE_DONE)
            break;
        else if (rc == SQLITE_ROW) {
            const char *data = (char*)sqlite3_column_blob(ppStmt, 0);
            int size = sqlite3_column_bytes(ppStmt, 0);
            buffer.insert(buffer.end(), data, data + size);
        }
        else {
            std::cerr << "getThumbnailFromDb Error in sqlite3_step, returned: " << rc << std::endl;
            sqlite3_reset(ppStmt);
            sqlite3_finalize(ppStmt);
            return {};
        }
    }

    // file is not in database
    if(buffer.empty())
        return {};

    QPixmap pm;
    pm.loadFromData((uchar*)&buffer[0], buffer.size());

    sqlite3_reset(ppStmt);
    sqlite3_finalize(ppStmt);
    return pm;
}


bool
ThumbnailJobSQLite::putThumbnailToDb(const QString &filepath, const QPixmap &pm)
{
    // qDebug() << "ThumbnailJobSQLite::putThumbnailToDb()";
    QString dbPath = getDbPath(filepath);
    sqlite3 *db = dbMap[dbPath];

    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    pm.save(&buffer, "PNG");

    sqlite3_stmt *ppStmt;
    char sql[] = "INSERT INTO THUMBNAILS VALUES (?, ?)";
    int rc = sqlite3_prepare_v2(db, sql, -1, &ppStmt, NULL);
    if(rc != SQLITE_OK || ppStmt == NULL) {
        std::cerr << "putThumbnailToDb() Error in sqlite3_prepare_v2, returned: " << rc << std::endl;
    }
    else {
        sqlite3_bind_text(ppStmt, 1, QFileInfo(filepath).fileName().toUtf8().data(), -1, SQLITE_STATIC);
        sqlite3_bind_blob(ppStmt, 2, bytes.data(), bytes.size(), SQLITE_STATIC);
    }

    rc = sqlite3_step(ppStmt);
    if( rc != SQLITE_DONE ) {
        std::cerr << "putThumbnailToDb() Error in sqlite3_step, returned: " << rc << std::endl;
        std::cerr << sqlite3_errmsg(db) << std::endl;
        sqlite3_reset(ppStmt);
        sqlite3_finalize(ppStmt);
        return false;
    }
    sqlite3_reset(ppStmt);
    sqlite3_finalize(ppStmt);
    return true;
}


ThumbnailJobSQLite::~ThumbnailJobSQLite()
{
    qDebug() << "ThumbnailJobSQLite::~ThumbnailJobSQLite()";
    QHashIterator<QString, sqlite3 *> i(dbMap);
    while (i.hasNext()) {
        i.next();
        sqlite3_close(i.value());
    }
    dbMap.clear();
}

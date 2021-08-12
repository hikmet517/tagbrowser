#include <QDir>
#include <QProcess>
#include <iostream>
#include <qdebug.h>
#include <qprocess.h>
#include <sqlite3.h>
#include "TMSU.hpp"


QString
TMSU::getDatabasePath(const QString& dirpath)
{
    QDir dir(dirpath);
    do {
        if(dir.exists(".tmsu")) {
            QDir temp(dir);
            temp.cd(".tmsu");
            if(temp.exists("db")) {
                return temp.absoluteFilePath("db");
            }
        }
    }
    while(dir.cdUp());
    std::cerr << "Cannot find TMSU database." << std::endl;
    return "";
}


static int
callback(void *data_void, int argc, char **argv, char **azColName)
{
    (void)azColName;
    QList<QList<QString>> *data = (QList<QList<QString>> *)data_void;
    QList<QString> newRow;
    for(int i = 0; i < argc; i++) {
        newRow.append(argv[i]);
    }
    data->append(newRow);
    return 0;
}


QList<QList<QString>>
TMSU::getTags(const QString& dbPath)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc = sqlite3_open(dbPath.toStdString().c_str(), &db);
    if(rc){
        std::cerr << "DB Error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return {};
    }

    char sql[] = "SELECT file.directory, file.name as filename, tag.name AS tag, value.name value FROM \
                  file \
                  INNER JOIN file_tag ON file.id = file_tag.file_id \
                  INNER JOIN tag ON file_tag.tag_id = tag.id \
                  LEFT JOIN value ON file_tag.value_id = value.id";

    QList<QList<QString>> data;
    rc = sqlite3_exec(db, sql, callback, &data, &zErrMsg);
    if(rc){
        std::cerr << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
    }

    sqlite3_close(db);

    return data;
}


int
TMSU::addTag(const QString& tag, const QStringList& files)
{
    qDebug() << "TMSU::addTag()" << tag << files;
    if(files.size() == 0)
        return 0;
    QProcess p;
    QStringList args;
    args << "tag" << "--tags" << tag;
    QDir wdir(files[0]);
    wdir.cdUp();
    p.setWorkingDirectory(wdir.path());
    for(const auto& file : files)
        args.append(file);

    p.start("tmsu", args);
    p.waitForFinished(-1);
    return p.exitCode();
}


int
TMSU::removeTag(const QString& tag, const QString& filename)
{
    qDebug() << "TMSU::removeTag()" << tag << filename;
    QProcess p;
    QStringList args;
    args << "untag" << filename << tag;
    QDir wdir(filename);
    wdir.cdUp();
    p.setWorkingDirectory(wdir.path());
    p.start("tmsu", args);
    p.waitForFinished(-1);
    return p.exitCode();
}

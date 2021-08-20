#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QProcess>

#include <iostream>
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

    char sql[] = "SELECT file.directory, file.name AS filename, tag.name AS tag, value.name AS value FROM \
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
    qDebug() << "TMSU::addTag()";
    if(files.size() == 0)
        return 0;
    QProcess p;
    p.setWorkingDirectory(QFileInfo(files[0]).dir().path());
    p.start("tmsu", QStringList() << "tag" << "--tags" << tag << files);
    p.waitForFinished(-1);
    return p.exitCode();
}


int
TMSU::removeTag(const QString& tag, const QStringList& files)
{
    qDebug() << "TMSU::removeTag()";
    if(files.size() == 0)
        return 0;
    QProcess p;
    p.setWorkingDirectory(QFileInfo(files[0]).dir().path());
    p.start("tmsu", QStringList() << "untag" << "--tags" << tag << files);
    p.waitForFinished(-1);
    return p.exitCode();
}


int
TMSU::query(const QString &query, const QString &wdir, QStringList &output)
{
    qDebug() << "TMSU::query()";
    QProcess p;
    p.setProgram("tmsu");
    p.setArguments(QStringList() << "files" << query);
    p.setWorkingDirectory(wdir);
    p.setReadChannel(QProcess::StandardOutput);
    p.start();
    p.waitForFinished(-1);

    int res = p.exitCode();
    if(res != 0) {
        output = QStringList();
        return res;
    }

    QString out = QString(p.readAllStandardOutput()).trimmed();
    QStringList lines = out.split('\n');
    for(const auto& line : lines) {
        if(line.isEmpty())
            continue;
        output.append(QDir(wdir + "/" + line).canonicalPath());
    }
    return res;
}


int
TMSU::untagged(QString &wdir, QStringList &output)
{
    qDebug() << "TMSU::untagged()";
    QProcess p;
    p.setProgram("tmsu");
    p.setArguments(QStringList() << "untagged");
    p.setWorkingDirectory(wdir);
    p.setReadChannel(QProcess::StandardOutput);
    p.start();
    p.waitForFinished(-1);

    int res = p.exitCode();
    if(res != 0) {
        output = QStringList();
        return res;
    }

    QString out = QString(p.readAllStandardOutput()).trimmed();
    QStringList lines = out.split('\n');
    for(const auto& line : lines) {
        if(line.isEmpty())
            continue;
        output.append(QDir(wdir + "/" + line).canonicalPath());
    }
    return res;
}

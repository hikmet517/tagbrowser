#pragma once
#include <QUrl>
#include <QPixmap>
#include <QSet>
#include <QDateTime>
#include <QFileInfo>

struct FileData{
    QUrl url;
    QPixmap pm;
    bool hasPm = false;
    QSet<QString> tags;
    QDateTime modified;

    FileData() {};

    FileData(const QUrl& u, const QPixmap& p, bool hasP=false, const QSet<QString>& t = {})
        : url(u), pm(p), hasPm(hasP), tags(t) {
        modified = QFileInfo(u.path()).lastModified();
    };

    // friend bool operator< (const FileData& left, const FileData& right) {
    //     return QString::compare(left.url.path(), right.url.path(), Qt::CaseInsensitive) < 0;
    // };

    // static bool modifiedRecent(const FileData& left, const FileData& right) {
    //     return left.modified > right.modified;
    // };
};

// Q_DECLARE_METATYPE(FileData);

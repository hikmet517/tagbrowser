#pragma once
#include <QUrl>
#include <QPixmap>
#include <QSet>


struct FileData{
    QUrl url;
    QPixmap pm;
    QSet<QString> tags;
    friend bool operator< (const FileData& left, const FileData& right) {
        return QString::compare(left.url.path(), right.url.path(), Qt::CaseInsensitive) < 0;
    };
};

Q_DECLARE_METATYPE(FileData);

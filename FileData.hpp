#pragma once
#include <QUrl>
#include <QPixmap>
#include <QSet>


struct FileData{
    QUrl url;
    QPixmap pm;
    QSet<QString> tags;
};

Q_DECLARE_METATYPE(FileData);

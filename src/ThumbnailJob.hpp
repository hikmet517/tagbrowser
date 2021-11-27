#pragma once
#include <QThread>
#include <QStringList>
#include <QMimeDatabase>

#include <vector>

#include <libffmpegthumbnailer/videothumbnailer.h>
#include <libffmpegthumbnailer/filmstripfilter.h>

#include "ThumbnailJobSQLite.hpp"

using ffmpegthumbnailer::VideoThumbnailer;
using ffmpegthumbnailer::FilmStripFilter;
using std::vector;


class ThumbnailJob : public QObject
{
    Q_OBJECT
public:
    ThumbnailJob(const QStringList &files, QObject *parent = nullptr);
    ~ThumbnailJob() override;

signals:
    void thumbnailReady(const QString &filepath, const QPixmap &pm);

public slots:
    void getThumbnails(int beg, int end);

private:
    QPixmap alignPixmap(const QPixmap &pm);

    QStringList mFiles;
    VideoThumbnailer *mThumbnailer;
    FilmStripFilter *mStripFilter;
    vector<uint8_t> mBuffer;
    QMimeDatabase mMimeDB;
    ThumbnailJobSQLite sqlite;
};

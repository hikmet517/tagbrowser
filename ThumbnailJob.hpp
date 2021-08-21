#pragma once
#include <QThread>
#include <QStringList>
#include <QMimeDatabase>

#include <vector>

#include <libffmpegthumbnailer/videothumbnailer.h>
#include <libffmpegthumbnailer/filmstripfilter.h>


using ffmpegthumbnailer::VideoThumbnailer;
using ffmpegthumbnailer::FilmStripFilter;
using std::vector;


class ThumbnailJob : public QThread
{
    Q_OBJECT
public:
    ThumbnailJob(const QStringList &files, QObject *parent = nullptr);
    ~ThumbnailJob() override;

signals:
    void thumbnailReady(const QString &filepath, const QPixmap &pm);

private:
    void run() override;
    QStringList mFiles;
    VideoThumbnailer *mThumbnailer;
    FilmStripFilter *mStripFilter;
    vector<uint8_t> mBuffer;
    QMimeDatabase mMimeDB;
};

#include <iostream>

#include <QImage>
#include <QPixmap>
#include <QIcon>
#include <QDebug>

#include "ThumbnailJob.hpp"

using ffmpegthumbnailer::VideoThumbnailer;
using ffmpegthumbnailer::FilmStripFilter;


ThumbnailJob::ThumbnailJob(const QStringList &files, QObject *parent)
    : QThread(parent)
{
    mFiles = files;
    mThumbnailer = new VideoThumbnailer(256, true, true, 8, true);
    mStripFilter = new FilmStripFilter;
    mThumbnailer->addFilter(mStripFilter);
}


void
ThumbnailJob::run()
{
    for(int i=0; i<mFiles.size(); i++) {
        const QString &filepath = mFiles[i];
        QMimeType mt = mMimeDB.mimeTypeForFile(filepath);
        QString type = mt.name().split('/')[0];

        // check cached db first
        QVariant res = sqlite.getThumbnail(filepath);
        QPixmap pm = res.value<QPixmap>();
        bool dbSuccessful = !pm.isNull();
        if(dbSuccessful) {
            emit thumbnailReady(filepath, pm);
            continue;
        }

        if(type == "image") {
            try {
                QPixmap img(filepath);
                img = img.scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                sqlite.putThumbnailToDb(filepath, img);
                emit thumbnailReady(filepath, img);
            }
            catch (std::exception &e) {
                std::cerr << e.what() << std::endl;
                continue;
            }
        }
        else if( type == "video" ) {
            try {
                mThumbnailer->generateThumbnail(filepath.toStdString(),
                                                ThumbnailerImageTypeEnum::Png,
                                                mBuffer);
                QPixmap pm;
                pm.loadFromData(&mBuffer[0], mBuffer.size());
                sqlite.putThumbnailToDb(filepath, pm);
                emit thumbnailReady(filepath, pm);
            }
            catch (std::exception &e) {
                std::cerr << e.what() << std::endl;
                continue;
            }
            if( (i+1)%4 == 0 ) {
                if(isInterruptionRequested()) {
                    std::cout << "Interruption Requested" << std::endl;
                    return;
                }
            }
        }
        else {
            QPixmap pm = QIcon::fromTheme(mt.genericIconName()).pixmap(256, 256);
            emit thumbnailReady(filepath, pm);
        }
    }
}


ThumbnailJob::~ThumbnailJob()
{
    qDebug() << "ThumbnailJob::~ThumbnailJob()";
    delete mThumbnailer;
    delete mStripFilter;
}

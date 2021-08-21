#include <iostream>
#include <QImage>
#include <QPixmap>
#include <QIcon>

#include "ThumbnailJob.hpp"


using ffmpegthumbnailer::VideoThumbnailer;
using ffmpegthumbnailer::FilmStripFilter;


ThumbnailJob::ThumbnailJob(const QStringList &files, QObject *parent)
    : QThread(parent)
{
    mFiles = files;
    mThumbnailer = new VideoThumbnailer (256, true, true, 8, true);
    mStripFilter = new FilmStripFilter;
}


void
ThumbnailJob::run()
{
    for(int i=0; i<mFiles.size(); i++) {
        QMimeType mt = mMimeDB.mimeTypeForFile(mFiles[i]);
        QString type = mt.name().split('/')[0];
        if(type == "image") {
            try {
                QPixmap img(mFiles[i]);
                img = img.scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                emit thumbnailReady(mFiles[i], img);
            }
            catch (std::exception &e) {
                std::cerr << e.what() << std::endl;
                continue;
            }
        }
        else if(type == "video") {
            try {
                mThumbnailer->generateThumbnail(mFiles[i].toStdString(),
                                                ThumbnailerImageTypeEnum::Png,
                                                mBuffer);
                QPixmap pm;
                pm.loadFromData(&mBuffer[0], mBuffer.size());
                emit thumbnailReady(mFiles[i], pm);
            }
            catch (std::exception &e) {
                std::cerr << e.what() << std::endl;
                continue;
            }
            if((i+1)%2 == 0) {
                if(isInterruptionRequested()) {
                    std::cout << "Interruption Requested" << std::endl;
                    return;
                }
            }
        }
        else {
            QPixmap pm = QIcon::fromTheme(mt.genericIconName()).pixmap(256, 256);
            emit thumbnailReady(mFiles[i], pm);
        }
    }
}


ThumbnailJob::~ThumbnailJob()
{
    std::cout << "ThumbnailJob::~ThumbnailJob()" << std::endl;
    delete mThumbnailer;
    delete mStripFilter;
}

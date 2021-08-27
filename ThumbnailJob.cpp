#include <iostream>

#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include <QDebug>
#include <qnamespace.h>
#include <qpixmap.h>

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
            emit thumbnailReady(filepath, alignPixmap(pm));
            continue;
        }

        // generate thumbnail
        if (type == "image") {
            try {
                QPixmap pm(filepath);
                pm = pm.scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                sqlite.putThumbnailToDb(filepath, pm);
                emit thumbnailReady(filepath, alignPixmap(pm));
            }
            catch (std::exception &e) {
                std::cerr << e.what() << std::endl;
                continue;
            }
        }
        else if ( type == "video" ) {
            try {
                mThumbnailer->generateThumbnail(filepath.toStdString(),
                                                ThumbnailerImageTypeEnum::Png,
                                                mBuffer);
                QPixmap pm;
                pm.loadFromData(&mBuffer[0], mBuffer.size());
                sqlite.putThumbnailToDb(filepath, pm);
                emit thumbnailReady(filepath, alignPixmap(pm));
            }
            catch (std::exception &e) {
                std::cerr << e.what() << std::endl;
                continue;
            }
        }
        // no thumbnail, load icon instead
        else {
            QPixmap pm = QIcon::fromTheme(mt.genericIconName()).pixmap(256, 256);
            emit thumbnailReady(filepath, alignPixmap(pm));
        }

        // check for termination
        if( (i+1)%5 == 0 ) {
            if(QThread::currentThread()->isInterruptionRequested()) {
                qDebug() << "Interruption Requested";
                return;
            }
        }
    }
}


QPixmap
ThumbnailJob::alignPixmap(const QPixmap &pm)
{
    QPixmap temp(256, 256);
    temp.fill(QColor(0, 0, 0, 0));
    QPainter painter(&temp);
    painter.drawPixmap((256-pm.width())/2, 256-pm.height(), pm);
    return temp;
}


ThumbnailJob::~ThumbnailJob()
{
    qDebug() << "ThumbnailJob::~ThumbnailJob()";
    delete mThumbnailer;
    delete mStripFilter;
}

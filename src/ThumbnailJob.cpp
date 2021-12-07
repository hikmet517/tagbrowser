#include <iostream>

#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include <QDebug>

#include "ThumbnailJob.hpp"

using ffmpegthumbnailer::VideoThumbnailer;
using ffmpegthumbnailer::FilmStripFilter;


ThumbnailJob::ThumbnailJob(QObject *parent)
{
    mThumbnailer = new VideoThumbnailer(256, true, true, 8, true);
    mStripFilter = new FilmStripFilter;
    mThumbnailer->addFilter(mStripFilter);
}


void
ThumbnailJob::getThumbnail(QString filepath)
{
    QMimeType mt = mMimeDB.mimeTypeForFile(filepath);
    QStringList types = mt.name().split('/');
    QString type = types[0];
    QString subType = types[1];

    // check cached db first
    optional<QPixmap> res = sqlite.getThumbnail(filepath);
    if(res.has_value()) {
        emit thumbnailReady(filepath, alignPixmap(res.value()));
        return;
    }

    // generate thumbnail
    if (type == "image" && !subType.startsWith('x')) {
        try {
            QPixmap pm(filepath);
            pm = pm.scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            sqlite.putThumbnailToDb(filepath, pm);
            emit thumbnailReady(filepath, alignPixmap(pm));
        }
        catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            return;
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
            return;
        }
    }
    // no thumbnail, load icon instead
    else {
        QString iconName = mt.iconName();
        if (iconName.isEmpty())
            iconName = mt.genericIconName();
        QPixmap pm = QIcon::fromTheme(iconName).pixmap(256, 256);
        emit thumbnailReady(filepath, alignPixmap(pm));
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

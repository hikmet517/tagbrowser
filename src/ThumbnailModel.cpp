#include <QDebug>
#include <QDirIterator>
#include <QFileInfo>
#include <QSet>

#include <algorithm>
#include <random>

#include "MainWindow.hpp"
#include "TMSU.hpp"
#include "ThumbnailModel.hpp"


ThumbnailModel::ThumbnailModel(QObject *parent)
    : QAbstractListModel(parent), mJob(nullptr)
{
}


void
ThumbnailModel::loadData(const QString &dir, const QString &dbPath)
{
    qDebug() << "ThumbnailModel::loadData()";
    mDir = dir;

    // clear old data
    beginResetModel();
    mData.clear();
    mSelected.clear();
    endResetModel();

    // prepare data
    beginInsertRows(QModelIndex(), 0, mData.size()-1);
    getFilesFromDir(dir);
    endInsertRows();

    // start preview job
    startPreviewJob();

    // get Tags
    mDBPath = dbPath;
    mRootPath = QFileInfo(QFileInfo(mDBPath).dir().path()).dir().path();
    getTagsFromDB();
}


void
ThumbnailModel::getFilesFromDir(const QString& dir)
{
    QDirIterator it(dir, QDir::Files | QDir::Hidden, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        FileData fi(QUrl::fromLocalFile(it.next()), QPixmap(256, 256));
        mData.append(fi);
    }
}


void
ThumbnailModel::getTagsFromDB()
{
    qDebug() << "ThumbnailModel::getTagsFromDB()";
    // read db
    QList<QList<QString>> tags = TMSU::getTags(mDBPath);
    qDebug() << mDBPath;
    qDebug() << tags.size();

    // create a hash set and a list of all tags
    QSet<QString> allTags;
    QHash<QString, QSet<QString>> tagHashSet;
    for(int i=0; i<tags.size(); i++){
        QString path = QDir(mRootPath + QDir::separator() + tags[i][0]
                            + QDir::separator() + tags[i][1]).canonicalPath();

        QString tag = tags[i][2];
        if(!tags[i][3].isEmpty())
            tag += "=" + tags[i][3];
        if(tagHashSet.contains(path))
            tagHashSet[path].insert(tag);
        else
            tagHashSet[path] = QSet({tag});
        allTags.insert(tag);
    }
    mAllTags = allTags.values();

    // fill tags of FileData (mData)
    for(int i=0; i<mData.size(); i++) {
        const auto& path = mData[i].url.path();
        if(tagHashSet.contains(path))
            mData[i].tags = tagHashSet[path];
        else
            mData[i].tags.clear();
    }
}


int
ThumbnailModel::rowCount(const QModelIndex &parent) const
{
    (void)parent;
    return mData.size();
}


QVariant
ThumbnailModel::data(const QModelIndex &index, int role) const
{
    // qDebug() << "data" << index.row();
    // qDebug() << "ThumbnailModel::data()";
    if(role == Qt::DisplayRole) {
        return mData[index.row()].url.fileName();
    }
    else if(role == Qt::ToolTipRole){
        return mData[index.row()].url.path();
    }
    else if(role == Qt::DecorationRole){
        if (!mData[index.row()].hasPm)
            emit prepareThumbnail(mData[index.row()].url.path());
        return mData[index.row()].pm;
    }
    else if(role == Qt::UserRole) {
        QVariant var;
        var.setValue(mData[index.row()]);
        return var;
    }
    return QVariant();
}


void
ThumbnailModel::startPreviewJob()
{
    mJob.quit();
    mJob.wait();

    ThumbnailJob *job = new ThumbnailJob(this);
    job->moveToThread(&mJob);
    connect(&mJob, &QThread::finished, job, &QObject::deleteLater);
    connect(this, &ThumbnailModel::prepareThumbnail, job, &ThumbnailJob::getThumbnail, Qt::QueuedConnection);
    connect(job, &ThumbnailJob::thumbnailReady, this, &ThumbnailModel::handleThumbnail);
    mJob.start();
}


void
ThumbnailModel::handleThumbnail(const QString &filepath, const QPixmap &pm)
{
    for(int i=0; i<mData.size(); i++) {
        if(mData[i].hasPm)
            continue;
        if(mData[i].url.path() == filepath) {
            mData[i].pm = pm;
            mData[i].hasPm = true;
            QModelIndex topLeft = createIndex(i, 0);
            emit dataChanged(topLeft, topLeft, {Qt::DecorationRole});
            break;
        }
    }
}


QStringList
ThumbnailModel::getSelectedTags()
{
    qDebug() << "ThumbnailModel::getSelectedTags()";
    QSetIterator<int> i(mSelected);
    QSet<QString> set;
    if(i.hasNext())
        set = mData[i.next()].tags;
    else
        return {};

    while (i.hasNext()) {
        set = set.intersect(mData[i.next()].tags);
    }
    return set.values();
}


QStringList
ThumbnailModel::getSelectedPaths()
{
    QStringList paths;
    QSetIterator<int> i(mSelected);
    while (i.hasNext())
        paths.append(mData[i.next()].url.path());
    return paths;
}


QStringList
ThumbnailModel::getAllTags()
{
    return mAllTags;
}


ThumbnailModel::~ThumbnailModel()
{
    qDebug() << "ThumbnailModel::~ThumbnailModel()";
    mJob.quit();
    mJob.wait();
}

#include <QDebug>
#include <QDirIterator>
#include <QFileInfo>
#include <QSet>

#include <algorithm>

#include "MainWindow.hpp"
#include "TMSU.hpp"
#include "ThumbnailModel.hpp"


ThumbnailModel::ThumbnailModel(QObject *parent)
    : QAbstractListModel(parent), job(nullptr)
{
}


void
ThumbnailModel::loadData(const QString &dir)
{
    qDebug() << "ThumbnailModel::loadData()";
    mDir = dir;

    // clear old data
    beginResetModel();
    mData.clear();
    endResetModel();

    // prepare data
    getFilesFromDir(dir);
    std::sort(mData.begin(), mData.end());
    // start preview job
    startPreviewJob();

    // get Tags
    mDBPath = TMSU::getDatabasePath(mDir);
    mRootPath = QFileInfo(QFileInfo(mDBPath).dir().path()).dir().path();
    getTagsFromDB();
}


void
ThumbnailModel::getFilesFromDir(const QString& dir)
{
    QDirIterator it(dir, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()){
        FileData fi;
        fi.url = QUrl::fromLocalFile(it.next());
        fi.pm = QPixmap(256, 256);

        // https://itqna.net/questions/52854/reset-qabstractlistmodel
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        mData.append(fi);
        endInsertRows();
    }
}


void
ThumbnailModel::getTagsFromDB()
{
    qDebug() << "ThumbnailModel::getTagsFromDB()";
    // read db
    QList<QList<QString>> tags = TMSU::getTags(mDBPath);

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
    // qDebug() << "ThumbnailModel::data()";
    if(role == Qt::DisplayRole) {
        return mData[index.row()].url.fileName();
    }
    else if(role == Qt::ToolTipRole){
        return mData[index.row()].url.path();
    }
    else if(role == Qt::DecorationRole){
        return mData[index.row()].pm;
    }
    else if(role == Qt::SizeHintRole){
        return QSize(290, 290);
    }
    else if(role == Qt::TextAlignmentRole) {
        return int(Qt::AlignHCenter | Qt::AlignTop);
    }
    return QVariant();
}


void
ThumbnailModel::startPreviewJob()
{
    // if(job != nullptr) {
    //     job->kill();
    // }

    KFileItemList files;
    for(int i=0; i<mData.size(); i++)
        files.append(mData[i].url);

    // prepare plugins
    QStringList plugins = KIO::PreviewJob::availablePlugins();

    // prepare job
    QSize size(256, 256);
    job = KIO::filePreview(files, size, &plugins);
    connect(job, &KIO::PreviewJob::gotPreview, this, &ThumbnailModel::handleThumbSuccess);
    connect(job, &KIO::PreviewJob::failed, this, &ThumbnailModel::handleThumbFail);
    job->start();
}


void
ThumbnailModel::handleThumbSuccess(const KFileItem& item, const QPixmap& preview)
{
    // qDebug() << "handleThumbSuccess";
    for(int i=0; i<mData.size(); i++){
        if(mData[i].url == item.url()) {
            mData[i].pm = preview;
            QModelIndex topLeft = createIndex(i, 0);
            emit dataChanged(topLeft, topLeft, {Qt::DecorationRole});
            break;
        }
    }
}


void
ThumbnailModel::handleThumbFail(const KFileItem& item)
{
    qDebug() << "ThumbnailModel::handleThumbFail()";
    for(int i=0; i<mData.size(); i++){
        if(mData[i].url == item.url()) {
            QMimeType mt = mMimeDB.mimeTypeForFile(mData[i].url.path());
            mData[i].pm = QIcon::fromTheme(mt.name()).pixmap(256);
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

bool
ThumbnailModel::hasSelected()
{
    return !mSelected.isEmpty();
}


QStringList
ThumbnailModel::getAllTags()
{
    return mAllTags;
}

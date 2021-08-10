#include "ThumbnailModel.hpp"
#include "TMSU.hpp"
#include "MainWindow.hpp"

#include <QAbstractItemModel>
#include <QDirIterator>
#include <QListView>
#include <QProcess>
#include <QVariant>
#include <QDebug>
#include <QIcon>

#include <KIO/PreviewJob>


ThumbnailModel::ThumbnailModel(const QString &dir, QObject *parent)
    : QAbstractListModel(parent)
{
    mDir = dir;

    // prepare data
    getFilesFromDir(dir);

    // start preview job
    startPreviewJob();

    // get Tags
    mDBPath = TMSU::getDatabasePath(mDir);
    getTagsFromDB();
}


ThumbnailModel::ThumbnailModel(const QStringList &dirs, QObject *parent)
    : QAbstractListModel(parent)
{
    // prepare data
    for(const auto& dir : dirs) {
        getFilesFromDir(dir);
    }

    // start preview job
    startPreviewJob();

    // get Tags
    mDBPath = TMSU::getDatabasePath(mDir);
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
        mData.append(fi);
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
        QDir root = QDir(mDBPath);
        root.cdUp();
        root.cdUp();
        QString path = root.path() + QDir::separator() + tags[i][0]
            + QDir::separator() + tags[i][1];
        if(tagHashSet.contains(path))
            tagHashSet[path].insert(tags[i][2]);
        else
            tagHashSet[path] = QSet({tags[i][2]});
        allTags.insert(tags[i][2]);
    }
    mAllTags = allTags.values();

    // fill tags of FileData (mData)
    for(int i=0; i<mData.size(); i++) {
        const auto& path = mData[i].url.path();
        if(tagHashSet.contains(path))
            mData[i].tags = tagHashSet[path];
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
    if(role == Qt::DisplayRole || role == Qt::ToolTipRole) {
        return mData[index.row()].url.fileName();
    }
    else if(role == Qt::DecorationRole){
        return mData[index.row()].pm;
    }
    // else if(role == Qt::SizeHintRole){
    //     return QSize(290, 290);
    // }
    return QVariant();
}


void
ThumbnailModel::startPreviewJob()
{
    KFileItemList files;
    for(int i=0; i<mData.size(); i++)
        files.append(mData[i].url);

    // prepare plugins
    QStringList plugins = KIO::PreviewJob::availablePlugins();

    // prepare job
    QSize size(256, 256);
    KIO::PreviewJob *job = KIO::filePreview(files, size, &plugins);
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


void
ThumbnailModel::handleSelection(const QItemSelection &selected, const QItemSelection &deselected)
{
    qDebug() << "ThumbnailModel::handleSelection()";
    for(const auto& index : selected.indexes()) {
        mSelected.insert(index.row());
    }
    for(const auto& index : deselected.indexes()) {
        mSelected.remove(index.row());
    }
    MainWindow *mw = static_cast<MainWindow*>(parent());
    mw->refreshTagWidget();
}


QStringList
ThumbnailModel::getSelectedTags()
{
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


void
ThumbnailModel::handleDoubleClick(const QModelIndex &index)
{
    // for windows start (cmd) or Invoke-Item (powershell)
    QProcess p(this);
    QStringList args;
    args << mData[index.row()].url.path();
    p.setProgram("xdg-open");
    p.setArguments(args);
    qint64 pid;
    p.startDetached(&pid);
}

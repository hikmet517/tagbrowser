#pragma once
#include <QWidget>
#include <QAbstractListModel>
#include <QAbstractItemModel>
#include <QMimeDatabase>

#include "FileData.hpp"
#include "TagWidget.hpp"
#include "ThumbnailJob.hpp"


class ThumbnailModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ThumbnailModel(QObject *parent = nullptr);
    ~ThumbnailModel();

    void loadData(const QString &dir);
    void loadData(const QStringList &files);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    void getFilesFromDir(const QString& dir);
    void getFilesFromFiles(const QStringList& files);
    void startPreviewJob();

    void handleThumbnail(const QString &filepath, const QPixmap &pm);

    void getTagsFromDB();
    QStringList getSelectedTags();
    QStringList getSelectedPaths();
    bool hasSelected();
    QStringList getAllTags();

    // fix here
    QList<FileData> mData;
    QSet<int> mSelected;
    QString mRootPath;
    QMimeDatabase mMimeDB;
    QString mDBPath;
    QStringList mAllTags;
    QThread mJob;
    int mProcessedCount = 0;

signals:
    void prepareThumbnail(int beg, int end);

private:
};

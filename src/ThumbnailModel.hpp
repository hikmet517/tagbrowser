#pragma once
#include <QWidget>
#include <QAbstractListModel>
#include <QAbstractItemModel>
#include <QMimeDatabase>
#include <QHash>

#include "FileData.hpp"
#include "TagWidget.hpp"
#include "ThumbnailJob.hpp"


class ThumbnailModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ThumbnailModel(QObject *parent = nullptr);
    ~ThumbnailModel();

    void load(const QString& dbpath);
    void load(const QStringList& files);
    QStringList getAllFiles();
    void getFilesForData(const QStringList& files);
    void getTagsForData();
    void clearData();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    void startPreviewJob();

    void handleThumbnail(const QString &filepath, const QPixmap &pm);

    QStringList getSelectedTags();
    QStringList getSelectedPaths();
    bool hasSelected();

    // fix here
    QList<FileData> mData;
    QList<FileData> mFullData;
    QString mDBPath;
    QString mRootPath;
    QStringList mAllTags;

    QSet<int> mSelected;
    QMimeDatabase mMimeDB;
    QThread mJob;
    int mProcessedCount = 0;

signals:
    void prepareThumbnail(int beg, int end);
};

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

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void getFilesFromDir(const QString& dir);
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
    QString mDir;
    QString mRootPath;
    QMimeDatabase mMimeDB;
    QString mDBPath;
    QStringList mAllTags;
    ThumbnailJob *mJob;

private:
};
